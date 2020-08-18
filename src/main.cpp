// Copyright © 2020 Jonathan Moscardini. All rights reserved.

#include <iostream>
#include <string>
#include <signal.h>

#include "src/menuview/TextMenuView.h"
#include "src/display/ssd1306/SSD1306MenuRenderer.h"
#include "src/display/adafruit-16x2-buttons/Adafruit16x2MenuRenderer.h"
#include "src/input/EvdevRotaryController.h"

textmenu::TextMenuView oled{std::make_unique<textmenu::display::SSD1306MenuRenderer>(4, 1, 0x3D)};
//textmenu::TextMenuView oled{std::make_unique<textmenu::display::Adafruit16x2MenuRenderer>()};
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
    textmenu::input::EvdevRotaryController rotary_controller{ROTARY_DEV_PATH, BUTTON_DEV_PATH};

    oled.UpdateEntries(default_list);

    bool exit_requested{ false };

    rotary_controller.RegisterPushButtonHandler([&](bool selected){
        if (selected && oled.GetCurrentIndex() == 3)
        {
            exit_requested = true;
        }
    });

    rotary_controller.RegisterRelativeRotationHandler([&](int rel_offset){
        oled.RequestIndexChange(rel_offset);
    });

    rotary_controller.Run();

    while (!exit_requested)
    {
        usleep(500);
    }

    oled.Stop();
    rotary_controller.Stop();
    exit(0);
}
