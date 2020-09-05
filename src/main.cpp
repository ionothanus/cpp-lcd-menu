// Copyright © 2020 Jonathan Moscardini. All rights reserved.

#include <iostream>
#include <string>
#include <memory>
#include <signal.h>

#include "src/menuview/TextMenuView.h"
#include "src/display/ssd1306/SSD1306MenuRenderer.h"
#include "src/display/adafruit-16x2-buttons/Adafruit16x2MenuRenderer.h"
#include "src/input/adafruit-16x2-buttons/Adafruit16x2ButtonController.h"
#include "src/input/evdev/EvdevRotaryController.h"
#include "src/driver/Adafruit16x2DeviceWrapper.h"
#include "src/menu/MenuLoader.h"
#include "src/menu/MenuController.h"

bool exit_requested{ false };

void signal_handler(int signum)
{
    if (signum == SIGINT)
    {
        exit_requested = true;
    }
}

const std::string ROTARY_DEV_PATH{"/dev/input/event1"};
const std::string BUTTON_DEV_PATH{"/dev/input/event0"};



int main(int argc, char** argv)
{
    textmenu::MenuEntry menu_list{ textmenu::menu::LoadYamlMenu(std::string{argv[1]}) };

    if (signal(SIGINT, signal_handler) == SIG_ERR)
    {
        std::cout << "Warning: can't handle SIGNIT" << std::endl << std::endl;
    }

    std::cout << "text-menu demo 2" << std::endl;

    std::unique_ptr<textmenu::display::IMenuRenderer> renderer{std::make_unique<textmenu::display::SSD1306MenuRenderer>(4, 1, 0x3D)};
    std::shared_ptr<textmenu::TextMenuView> menuview{std::make_unique<textmenu::TextMenuView>(std::move(renderer))};
    std::shared_ptr<textmenu::ITask> menuviewtask{menuview};

    std::shared_ptr<textmenu::input::EvdevRotaryController> evdev_rotary_controller{ std::make_shared<textmenu::input::EvdevRotaryController>(ROTARY_DEV_PATH, BUTTON_DEV_PATH) };
    std::shared_ptr<textmenu::input::IMenuInputController> controller{ evdev_rotary_controller };
    std::shared_ptr<textmenu::ITask> controller_task{ evdev_rotary_controller };

    textmenu::menu::MenuController menu_controller{menu_list.submenu, std::move(menuview), controller};

    menu_controller.RegisterAction("Quit", textmenu::menu::MenuAction{ [&](){
        exit_requested = true;
        return true;
    }, false });

    //testing
    menu_controller.RegisterAction("fan_speed_up", textmenu::menu::MenuAction{ [&](){
        return true;
    }, true });

    menuviewtask->Run();
    controller_task->Run();

    while (!exit_requested)
    {
        usleep(500);
    }

    menuviewtask->Stop();
    controller_task->Stop();

    return 0;
}
