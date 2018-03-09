#include <cstring>
#include <libbase/k60/mcg.h>
#include <cstdlib>
#include <libsc/system.h>
namespace libbase
{
    namespace k60
    {

        Mcg::Config Mcg::GetMcgConfig()
        {
            Mcg::Config config;
            config.external_oscillator_khz = 50000;
            config.core_clock_khz = 150000;
            return config;
        }

    }
}

using namespace libsc;
using namespace libbase::k60;

#include <libsc/lcd.h>
#include <libsc/st7735r.h>
#include <libsc/joystick.h>
#include <libsc/k60/ov7725.h>
#include <ui/menus/menu_group.h>
#include <ui/color_schemes/futurism.h>
#include <ui/menus/menu_number.h>
#include <flash_storage.h>
#include <ui/font_families/ubuntu_mono.h>

int main()
{
    System::Init();

    //Initialize components
    auto* lcd_ptr = new libsc::St7735r([](){
        libsc::St7735r::Config config;
        config.fps = 60;
        config.is_bgr = false;
        config.orientation = 2;
        return config;
    }());

    libsc::BatteryMeter::Config battery_meter_config{0.4};

    auto* battery_meter = new libsc::BatteryMeter(battery_meter_config);

    auto* flash = new libbase::k60::Flash([](){
        libbase::k60::Flash::Config config{};
        return config;
    }());

    ui::Context::lcd_ptr = lcd_ptr;
    ui::Context::batteryMeter = battery_meter;
    ui::Context::setColorScheme([](){
        ui::color_schemes::Futurism color_scheme;
        return color_scheme;
    }());

    ui::Context::joystick_config_base.id = 0;
    ui::Context::joystick_config_base.is_active_low = true;

    ui::Context::prepareListenerEvents();
    ui::Context::setJoystickRotation(ui::Context::QUARTER_CCW);

    FlashStorage::flash_ptr = flash;
    FlashStorage::load();

    ui::MenuGroup menu_group("Startup Menu");
    menu_group.setHasBackArrow(false);

    class ConfigFlashFloat: public ui::MenuNumber<float> {
    public:
        explicit ConfigFlashFloat(float* data_ptr):
                data_ptr(data_ptr) {}

        void onEnter() override {
            this->setValue(*data_ptr);
        }

        void onExit() override {
            //save to flash
            *data_ptr = value;
            FlashStorage::save();
        }

    private:
        float* data_ptr;

    };

    class ResetFlashAction: public ui::MenuAction {
    public:

        int run() override {
            FlashStorage::Data data{};
            FlashStorage::data = data;
            //save to flash
            FlashStorage::save();

            return SUCCESS;
        }
    };

    auto* config_p = new ConfigFlashFloat(&FlashStorage::data.P);
    config_p->setName("Set P-Value");
    config_p->setStep(0.1f);
    auto* config_i = new ConfigFlashFloat(&FlashStorage::data.I);
    config_i->setName("Set I-Value");
    config_i->setStep(0.1f);
    auto* config_d = new ConfigFlashFloat(&FlashStorage::data.D);
    config_d->setName("Set D-Value");
    config_d->setStep(0.1f);
    auto* reset_flash_action = new ResetFlashAction;
    reset_flash_action->setName("Reset Flash");

    class RunAction: public ui::MenuAction {
    public:
        explicit RunAction(ui::MenuGroup* menu_group_ptr): menu_group_ptr(menu_group_ptr) {
            this->setName("Run");
        }
        int run() override {
            menu_group_ptr->exitMenu();
            return SUCCESS;
        }

    private:
        ui::MenuGroup* menu_group_ptr;
    };

    auto* run_action = new RunAction(&menu_group);

    class CameraPreviewAction: public ui::MenuAction {
    public:
        CameraPreviewAction() {
            this->setName("Camera Preview");
        }
        int run() override {
            //Change screen
            ui::Context::lcd_ptr->SetRegion(ui::Context::full_screen);
            ui::Context::lcd_ptr->FillColor(ui::Context::color_scheme.GRAY_DARKER);

            ui::Toolbar toolbar;
            toolbar.setHasBackArrow(true);
            toolbar.setName("Camera Preview");
            toolbar.setRegion(0, 0, 128, 16);
            toolbar.render();

            //Render loop
            Timer::TimerInt time = System::Time();
            libsc::Lcd::Rect image_region = libsc::Lcd::Rect(0, 18, 128, 96);

            bool is_exit = false;

            std::function<void(ui::E)> joystick_handler = [&](ui::E e){
                if (e.JOYSTICK_STATE == ui::Context::JOYSTICK_LEFT) {
                    is_exit = true;
                }
            };

            ui::Context::addEventListener(ui::Event::JOYSTICK_DOWN, &joystick_handler);

            libsc::k60::Ov7725::Config camera_config;
            camera_config.id = 0;
            camera_config.w = 128;
            camera_config.h = 96;
            camera_config.fps = libsc::k60::Ov7725Configurator::Config::Fps::kHigh;
            libsc::k60::Ov7725 camera(camera_config);
            camera.Start();
            Uint buffer_size = camera.GetBufferSize();


            while (!is_exit) {
                if (time != System::Time()) {
                    time = System::Time();
                    if (time % 200 == 0) {
                        //Updates image
                        const Byte* buffer = camera.LockBuffer();
                        ui::Context::lcd_ptr->SetRegion(image_region);
                        ui::Context::lcd_ptr->FillBits(ui::Context::color_scheme.BLACK, ui::Context::color_scheme.WHITE, buffer, buffer_size*8);
                        camera.UnlockBuffer();

                        ui::Context::lcd_ptr->SetRegion(libsc::Lcd::Rect(64, 18, 1, 96));
                        ui::Context::lcd_ptr->FillColor(ui::Context::color_scheme.PRIMARY);
                    }
                }
            }

            ui::Context::removeEventListener(ui::Event::JOYSTICK_DOWN, &joystick_handler);
            camera.Stop();

            return SUCCESS;
        }
    };

    auto* camera_preview_action = new CameraPreviewAction;

    menu_group.addMenuAction(run_action);
    menu_group.addMenuAction(config_p);
    menu_group.addMenuAction(config_i);
    menu_group.addMenuAction(config_d);
    menu_group.addMenuAction(camera_preview_action);
    menu_group.addMenuAction(reset_flash_action);
    menu_group.run();

    while(true) {}

    return 0;
}
