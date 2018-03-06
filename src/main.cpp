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
#include <ui/menus/menu_group.h>
#include <ui/color_schemes/futurism.h>
#include <ui/menus/menu_number.h>
#include <flash_storage.h>

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

    libsc::Joystick::Config joystick_config;
    joystick_config.id = 0;
    joystick_config.is_active_low = true;

    auto* joystick_ptr = new libsc::Joystick(joystick_config);

    libsc::BatteryMeter::Config battery_meter_config{0.4};

    auto* battery_meter = new libsc::BatteryMeter(battery_meter_config);

    auto* flash = new libbase::k60::Flash([](){
        libbase::k60::Flash::Config config{};
        return config;
    }());

    ui::Context::lcd_ptr = lcd_ptr;
    ui::Context::batteryMeter = battery_meter;
    ui::Context::joystick_ptr = joystick_ptr;
    ui::Context::setColorScheme([](){
        ui::color_schemes::Futurism color_scheme;
        return color_scheme;
    }());
    //ui::Context::set_joystick_rotation(ui::Context::QUARTER_CCW);

    /*ui::text::TextEngine textEngine;
    textEngine.setColor(Lcd::kWhite);
    textEngine.setRegion(Lcd::Rect(0, 0, 128, 15));
    textEngine.setFont(&ui::fonts::blocky);
    textEngine.setTextWrap(ui::text::ELLIPSIS);
    textEngine.render("The quick brown fox jumps over a lazy dog.");

    textEngine.setRegion(Lcd::Rect(0, 15, 128, 11));
    textEngine.setFont(&ui::fonts::ubuntu_mono);
    textEngine.setOverflow(ui::text::HIDDEN);
    textEngine.setTextWrap(ui::text::NO_WRAP);
    textEngine.render("The quick brown fox jumps over a lazy dog.");

    textEngine.setRegion(Lcd::Rect(0, 30, 128, 11));
    textEngine.setFont(&ui::fonts::ubuntu_mono_italic);
    textEngine.setOverflow(ui::text::HIDDEN);
    textEngine.setTextWrap(ui::text::NO_WRAP);
    textEngine.render("The quick brown fox jumps over a lazy dog.");

    textEngine.setRegion(Lcd::Rect(0, 45, 128, 100));
    textEngine.setFont(&ui::fonts::noto);
    textEngine.setTextWrap(ui::text::ELLIPSIS);
    textEngine.render("The quick brown fox jumps over a lazy dog.");

    textEngine.setRegion(Lcd::Rect(0, 100, 128, 50));
    textEngine.setFont(&ui::fonts::humanist);
    textEngine.setTextWrap(ui::text::WRAP);
    textEngine.render("The quick brown fox jumps over a lazy dog.");*/

    FlashStorage flash_storage;
    flash_storage.flash_ptr = flash;
    flash_storage.load();

    ui::MenuGroup menu_group("Startup Menu");
    menu_group.setHasBackArrow(false);

    class ConfigFlashFloat: public ui::MenuNumber<float> {
    public:
        explicit ConfigFlashFloat(FlashStorage* flash_storage_ptr, float* data_ptr):
                flash_storage_ptr(flash_storage_ptr),
                data_ptr(data_ptr) {}

        int onEnter() override {
            this->setValue(*data_ptr);
            return SUCCESS;
        }

        void onExit() override {
            //save to flash
            *data_ptr = value;
            flash_storage_ptr->save();
        }

    private:
        FlashStorage* flash_storage_ptr;
        float* data_ptr;

    };

    auto* config_p = new ConfigFlashFloat(&flash_storage, &flash_storage.data.P);
    config_p->setName("Set P-Value");
    auto* config_i = new ConfigFlashFloat(&flash_storage, &flash_storage.data.I);
    config_i->setName("Set I-Value");
    auto* config_d = new ConfigFlashFloat(&flash_storage, &flash_storage.data.D);
    config_d->setName("Set D-Value");

    class RunAction: public ui::MenuAction {
    public:
        RunAction() {
            this->setName("Run");
        }
        int onEnter() override {
            //TODO call main function
            return SUCCESS;
        }
    };

    auto* run_action = new RunAction;

    class CameraPreviewAction: public ui::MenuAction {
    public:
        CameraPreviewAction() {
            this->setName("Camera Preview");
        }
        int onEnter() override {
            //Change screen

            //Render loop
            Timer::TimerInt time = System::Time();
            libsc::Lcd::Rect image_region = libsc::Lcd::Rect(0, 0, 120, 160);

            while (true) {
                if (time != System::Time()) {
                    time = System::Time();
                    if (time % 200 == 0) {
                        //TODO Update image


                        if (ui::Context::joystick_ptr->GetState() == ui::Context::JOYSTICK_LEFT) {
                            //Return to menu
                            break;
                        }
                    }
                }
            }

            return SUCCESS;
        }
    };

    auto* camera_preview_action = new CameraPreviewAction;

    menu_group.addMenuAction(run_action);
    menu_group.addMenuAction(config_p);
    menu_group.addMenuAction(config_i);
    menu_group.addMenuAction(config_d);
    menu_group.addMenuAction(camera_preview_action);
    menu_group.run();

    while(true) {}

    return 0;
}
