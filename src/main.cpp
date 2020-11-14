// Copyright (c) 2020 Jonathan Moscardini
// 
// Licensed under the MIT License
// http://www.opensource.org/licenses/MIT

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

const std::string BUTTON_DEV_PATH{"/dev/input/event0"};
const std::string ROTARY_DEV_PATH{"/dev/input/event1"};

int main(int argc, char** argv)
{
    using textmenu::display::IMenuRenderer;
    using textmenu::input::IMenuInputController;
    using textmenu::display::SSD1306MenuRenderer;
    using textmenu::driver::Adafruit16x2DeviceWrapper;
    using textmenu::display::Adafruit16x2MenuRenderer;
    using textmenu::input::Adafruit16x2ButtonController;

    using textmenu::input::EvdevRotaryController;

    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////

    // Here, we'll load the YAML menu from the file.
    // This contains the definition of the display to be initialized,
    // the definition of the controller to be initialized,
    // and the description of the menu to be loaded on that display
    // and used with that controller.
    // The file format demands exactly one menu to be defined alongside
    // one display and/or one controller.
    textmenu::menu::MenuDefinition menu_definition{ textmenu::menu::LoadYamlMenu(std::string{argv[1]}) };

    // We can extract the menu definition from the configuration here.
    textmenu::menu::MenuEntry menu_list{ menu_definition.first };

    if (signal(SIGINT, signal_handler) == SIG_ERR)
    {
        std::cout << "Warning: can't handle SIGNIT" << std::endl << std::endl;
    }

    std::cout << "text-menu demo 2" << std::endl;

    // We'll manually create a display from the configuration here,
    // interpreting the concrete object through the interfaces to demonstrate
    // their use.
    std::unique_ptr<IMenuRenderer> renderer{ SSD1306MenuRenderer::ConstructFromConfigMap(menu_definition.second.m_display_settings) };
    std::shared_ptr<textmenu::TextMenuView> menuview{std::make_unique<textmenu::TextMenuView>(std::move(renderer))};
    std::shared_ptr<textmenu::ITask> menuviewtask{ menuview} ;

    // We'll do the same for a control device.
    std::shared_ptr<EvdevRotaryController> evdev_rotary_controller{ EvdevRotaryController::ConstructFromConfigMap(menu_definition.second.m_controller_settings) };
    std::shared_ptr<IMenuInputController> controller{ evdev_rotary_controller };
    std::shared_ptr<textmenu::ITask> controller_task{ evdev_rotary_controller };

    textmenu::menu::MenuController menu_controller{ menu_list.submenu, std::move(menuview), controller };

    // Here's an example "exit" action, for a menu entry called "Quit".
    menu_controller.RegisterAction("Quit", textmenu::menu::MenuAction{ [&](){
        exit_requested = true;
        return true;
    }, false });

    // This menu action doesn't actually do anything - you could insert
    // any function call you like in this lambda.
    menu_controller.RegisterAction("fan_speed_up", textmenu::menu::MenuAction{ [&](){
        return true;
    }, true });

    // ITask objects must be started - this launches their individual threads.
    menuviewtask->Run();
    controller_task->Run();

    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////

    // This defines a SettingsMap without loading it from YAML,
    // and constructs a menu from it in the same manner
    // as the YAML output was used above.
    textmenu::menu::SettingsMap test_map{ {"i2c_bus", "1" }, {"i2c_address", "0x20"} };
    std::shared_ptr<Adafruit16x2DeviceWrapper> adafruit_driver{ Adafruit16x2DeviceWrapper::ConstructFromConfigMap(test_map) };
    std::unique_ptr<Adafruit16x2MenuRenderer> adafruit_display{ std::make_unique<Adafruit16x2MenuRenderer>(adafruit_driver) };

    std::shared_ptr<Adafruit16x2ButtonController> adafruit_buttons{ std::make_shared<Adafruit16x2ButtonController>(adafruit_driver) };
    std::shared_ptr<IMenuInputController> buttons{ adafruit_buttons };
    std::shared_ptr<textmenu::ITask> button_task{ adafruit_buttons };

    std::unique_ptr<IMenuRenderer> lcd{ std::move(adafruit_display) };
    std::shared_ptr<textmenu::TextMenuView> lcd_menuview{ std::make_unique<textmenu::TextMenuView>(std::move(lcd)) };
    std::shared_ptr<textmenu::ITask> lcd_task{ lcd_menuview };

    textmenu::menu::MenuController lcd_controller{ menu_list.submenu, lcd_menuview, buttons };

    lcd_task->Run();
    button_task->Run();

    lcd_controller.RegisterAction("Quit", textmenu::menu::MenuAction{ [&](){
        exit_requested = true;
        return true;
    }, false });

    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////

    // We'll wait patiently for the "Quit" command, with its action
    // registered above, to be selected. Once it is, `exit_requested`
    // will become true, the loop will exit, and we'll clean up
    // our resources.

    while (!exit_requested)
    {
        usleep(500);
    }

    menuviewtask->Stop();
    controller_task->Stop();
    lcd_task->Stop();
    button_task->Stop();

    return 0;
}
