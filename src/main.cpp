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

    textmenu::menu::MenuDefinition menu_definition{textmenu::menu::LoadYamlMenu(std::string{argv[1]})};

    textmenu::menu::MenuEntry menu_list{ menu_definition.first };

    if (signal(SIGINT, signal_handler) == SIG_ERR)
    {
        std::cout << "Warning: can't handle SIGNIT" << std::endl << std::endl;
    }

    std::cout << "text-menu demo 2" << std::endl;

    std::unique_ptr<IMenuRenderer> renderer{SSD1306MenuRenderer::ConstructFromConfigMap(menu_definition.second.m_display_settings)};
    std::shared_ptr<textmenu::TextMenuView> menuview{std::make_unique<textmenu::TextMenuView>(std::move(renderer))};
    std::shared_ptr<textmenu::ITask> menuviewtask{menuview};

    std::shared_ptr<EvdevRotaryController> evdev_rotary_controller{ EvdevRotaryController::ConstructFromConfigMap(menu_definition.second.m_controller_settings) };
    std::shared_ptr<IMenuInputController> controller{ evdev_rotary_controller };
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

    //testing
    textmenu::menu::SettingsMap test_map{{"i2c_bus", "1" }, {"i2c_address", "0x20"}};
    std::shared_ptr<Adafruit16x2DeviceWrapper> adafruit_driver{Adafruit16x2DeviceWrapper::ConstructFromConfigMap(test_map)};
    std::unique_ptr<Adafruit16x2MenuRenderer> adafruit_display{std::make_unique<Adafruit16x2MenuRenderer>(adafruit_driver)};

    std::shared_ptr<Adafruit16x2ButtonController> adafruit_buttons{std::make_shared<Adafruit16x2ButtonController>(adafruit_driver)};
    std::shared_ptr<IMenuInputController> buttons{adafruit_buttons};
    std::shared_ptr<textmenu::ITask> button_task{adafruit_buttons};

    std::unique_ptr<IMenuRenderer> lcd{std::move(adafruit_display)};
    std::shared_ptr<textmenu::TextMenuView> lcd_menuview{std::make_unique<textmenu::TextMenuView>(std::move(lcd))};
    std::shared_ptr<textmenu::ITask> lcd_task{lcd_menuview};

    textmenu::menu::MenuController lcd_controller{menu_list.submenu, lcd_menuview, buttons};

    lcd_task->Run();
    button_task->Run();

    lcd_controller.RegisterAction("Quit", textmenu::menu::MenuAction{ [&](){
        exit_requested = true;
        return true;
    }, false });

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
