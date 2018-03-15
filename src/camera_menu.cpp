//
// Created by Daniel on 15/3/2018.
//

#include "camera_menu.h"

void CameraMenu::run() {
    ui::fonts::Humanist humanist;
    ui::fonts::Blocky blocky;

    ui::Context::addFontToRepo("Humanist", &humanist);
    ui::Context::addFontToRepo("Blocky", &blocky);

    if (ui::Context::lcd_ptr == nullptr) {
        ui::Context::lcd_ptr = new libsc::St7735r([](){
            libsc::St7735r::Config config;
            config.fps = 60;
            config.is_bgr = false;
            config.orientation = 2;
            return config;
        }());
    }

    if (ui::Context::batteryMeter == nullptr) {
        libsc::BatteryMeter::Config battery_meter_config{0.4};
        ui::Context::batteryMeter = new libsc::BatteryMeter(battery_meter_config);
    }

    if (ui::Context::joystick_ptr == nullptr) {
        ui::Context::joystick_config_base.id = 0;
        ui::Context::joystick_config_base.is_active_low = false;

        ui::Context::prepareListenerEvents();
        ui::Context::setJoystickRotation(ui::Context::QUARTER_CCW);
    }

    ui::Context::setColorScheme([](){
        ui::color_schemes::Futurism color_scheme;
        return color_scheme;
    }());

    //Initialize components
    auto* flash = new libbase::k60::Flash([](){
        libbase::k60::Flash::Config config{};
        return config;
    }());



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

    ConfigFlashFloat config_p(&FlashStorage::data.P);
    config_p.setName("Set P-Value");
    config_p.setStep(0.1f);
    ConfigFlashFloat config_i(&FlashStorage::data.I);
    config_i.setName("Set I-Value");
    config_i.setStep(0.1f);
    ConfigFlashFloat config_d(&FlashStorage::data.D);
    config_d.setName("Set D-Value");
    config_d.setStep(0.1f);
    ResetFlashAction reset_flash_action;
    reset_flash_action.setName("Reset Flash");

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

    RunAction run_action(&menu_group);

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

    CameraPreviewAction camera_preview_action;

    int i = 0;

    libsc::Servo* servo_ptr = new libsc::Servo([](){
        libsc::Servo::Config servo_config {};
        servo_config.id = 0;
        servo_config.period = 3333;
        servo_config.min_pos_width = 1000;
        servo_config.max_pos_width = 2000;
        return servo_config;
    }());

    class ServoInteger: public ui::MenuNumber<int> {
    public:
        explicit ServoInteger(int* i_ptr, libsc::Servo* servo_ptr): i_ptr(i_ptr), servo_ptr(servo_ptr) {}

        void onEnter() override {
            this->value = *i_ptr;
        }

        void onChange() override {
            servo_ptr->SetDegree((uint16_t) (900 + value));
        }

    private:
        int* i_ptr;
        libsc::Servo* servo_ptr;
    };

    ServoInteger servo_integer(&i, servo_ptr);
    servo_integer.setName("Turn Servo");
    servo_integer.setValue(i);
    servo_integer.setStep(5);

    menu_group.addMenuAction(&run_action);
    menu_group.addMenuAction(&servo_integer);
    menu_group.addMenuAction(&config_p);
    menu_group.addMenuAction(&config_i);
    menu_group.addMenuAction(&config_d);
    menu_group.addMenuAction(&camera_preview_action);
    menu_group.addMenuAction(&reset_flash_action);
    menu_group.run();

    ui::Context::removeFontFromRepo("Humanist");
    ui::Context::removeFontFromRepo("Blocky");
}
