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

//textmenu::TextMenuView oled{std::make_unique<textmenu::display::SSD1306MenuRenderer>(4, 1, 0x3D)};
std::shared_ptr<textmenu::driver::Adafruit16x2DeviceWrapper> wrapper{std::make_shared<textmenu::driver::Adafruit16x2DeviceWrapper>(1, 0x20)};

textmenu::TextMenuView oled{std::make_unique<textmenu::display::Adafruit16x2MenuRenderer>(wrapper)};
textmenu::MenuList default_list{textmenu::MenuEntry{"Hello, not cruel world! I am so thrilled to be here today!", textmenu::MenuList{}},
                                textmenu::MenuEntry{"Goodbye, sort of cruel world!", textmenu::MenuList{}},
                                textmenu::MenuEntry{"Goodbye, crueller world!", textmenu::MenuList{}},
                                textmenu::MenuEntry{"Goodbye, cruelest world!", textmenu::MenuList{}},
                                textmenu::MenuEntry{"Goodbye, less-cruel world!", textmenu::MenuList{}}
                                };

void signal_handler(int signum)
{
    oled.UpdateEntries(textmenu::MenuList{});
    oled.Stop();
    exit(signum);
}

const int MIN_INDEX = 0;
const int MAX_INDEX = 3;

const std::string ROTARY_DEV_PATH{"/dev/input/event1"};
const std::string BUTTON_DEV_PATH{"/dev/input/event0"};

int main()
{
    std::cout << "text-menu demo 2" << std::endl;

    oled.Run();

    // std::shared_ptr<textmenu::input::EvdevRotaryController> evdev_rotary_controller{ std::make_shared<textmenu::input::EvdevRotaryController>(ROTARY_DEV_PATH, BUTTON_DEV_PATH) };
    // std::shared_ptr<textmenu::input::IMenuInputController> controller{ evdev_rotary_controller };
    // std::shared_ptr<textmenu::ITask> controller_task{ evdev_rotary_controller };

    std::shared_ptr<textmenu::input::Adafruit16x2ButtonController> adafruit_button_controller{ std::make_shared<textmenu::input::Adafruit16x2ButtonController>(wrapper) };
    std::shared_ptr<textmenu::input::IMenuInputController> controller{ adafruit_button_controller };
    std::shared_ptr<textmenu::ITask> controller_task{ adafruit_button_controller };
    
    oled.UpdateEntries(default_list);

    bool exit_requested{ false };

    controller->RegisterSelectButtonHandler([&](bool selected){
        if (selected && oled.GetCurrentIndex() == 3)
        {
            exit_requested = true;
        }
    });

    controller->RegisterRelativeVerticalHandler([&](int rel_offset){
        oled.RequestIndexChange(rel_offset);
    });

    controller_task->Run();

    while (!exit_requested)
    {
        usleep(500);
    }

    oled.Stop();
    controller_task->Stop();
    exit(0);
}
