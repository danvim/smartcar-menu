//
// Created by Daniel on 15/3/2018.
//

#ifndef INNO14_D_2017_INNO_MENU_H
#define INNO14_D_2017_INNO_MENU_H

#include <libsc/lcd.h>
#include <libsc/st7735r.h>
#include <libsc/joystick.h>
#include <libsc/k60/ov7725.h>
#include <ui/menus/menu_group.h>
#include <flash_storage.h>
#include <libsc/servo.h>
#include <ui/color_schemes/futurism.h>
#include <ui/menus/menu_number.h>
#include <ui/fonts/humanist.h>
#include <ui/fonts/blocky.h>


using namespace libsc;
using namespace libbase::k60;

class CameraMenu {
public:
    static void run();
};


#endif //INNO14_D_2017_INNO_MENU_H
