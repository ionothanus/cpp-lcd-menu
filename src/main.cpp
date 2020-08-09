// Copyright © 2020 Jonathan Moscardini. All rights reserved.

#include <iostream>
#include <sys/types.h>
#include <cstdio>
#include <string.h>
#include <sys/stat.h>
#include <linux/input.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include "libevdev-1.0/libevdev/libevdev.h"
#include "libSSD1306/lib/OledFont8x16.h"
#include "libSSD1306/lib/OledGraphics.h"
#include "libSSD1306/lib/OledI2C.h"
    SSD1306::OledI2C* oled;
void signal_handler(int signum)
{
    if (oled != nullptr)
    {
        oled->displayOff();
        std::cout << signum << std::endl;
    }
    exit(signum);
}

const int MIN_INDEX = 0;
const int MAX_INDEX = 3;

int menu_index = 0;

void redraw()
{
    //oled->clear();
    //drawString8x16(SSD1306::OledPoint{0, 16}, "Goodbye World!", SSD1306::PixelStyle::Set, *oled);
    //oled->displayInverse();
    SSD1306::OledBitmap<128, 16> menu_selected{};
    SSD1306::OledBitmap<128, 64> buffer{};
    menu_selected.fill();
    buffer.clear();

    buffer.setFrom(menu_selected, SSD1306::OledPoint{0, menu_index * 16});

    drawString8x16(SSD1306::OledPoint{0, 0}, "Hello World!", (menu_index == 0 ? SSD1306::PixelStyle::Unset : SSD1306::PixelStyle::Set), buffer);
    drawString8x16(SSD1306::OledPoint{0, 16}, "Hello World?", (menu_index == 1 ? SSD1306::PixelStyle::Unset : SSD1306::PixelStyle::Set), buffer);
    drawString8x16(SSD1306::OledPoint{0, 32}, "World? Hello!", (menu_index == 2 ? SSD1306::PixelStyle::Unset : SSD1306::PixelStyle::Set), buffer);
    drawString8x16(SSD1306::OledPoint{0, 48}, "Goodbye World!", (menu_index == 3 ? SSD1306::PixelStyle::Unset : SSD1306::PixelStyle::Set), buffer);

    oled->setFrom(buffer);

    oled->displayUpdate();
}

int main()
{
    std::cout << "Hello worlds!" << std::endl;

    bool displayToggle = true;

    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);

    try {
        oled = new SSD1306::OledI2C("/dev/i2c-1", 0x3D);
        oled->clear();
        oled->displayUpdate();
    }
    catch (std::exception e)
    {
        std::cout << "exception!" << std::endl;
        std::cout << e.what() << std::endl;
        return -1;
    }

    struct libevdev *dev_rotate = NULL, *dev_click = NULL;
    int fd_rotate, fd_click;
    int rc = 1;
    
    fd_rotate = open("/dev/input/event1", O_RDONLY|O_NONBLOCK);
    rc = libevdev_new_from_fd(fd_rotate, &dev_rotate);
    if (rc < 0) {
            fprintf(stderr, "Failed to init libevdev (%s)\n", strerror(-rc));
            exit(1);
    }

    fd_click = open("/dev/input/event0", O_RDONLY|O_NONBLOCK);
    rc = libevdev_new_from_fd(fd_click, &dev_click);
    if (rc < 0) {
            fprintf(stderr, "Failed to init libevdev (%s)\n", strerror(-rc));
            exit(1);
    }

    printf("Input device name: \"%s\"\n", libevdev_get_name(dev_rotate));
    printf("Input device ID: bus %#x vendor %#x product %#x\n",
        libevdev_get_id_bustype(dev_rotate),
        libevdev_get_id_vendor(dev_rotate),
        libevdev_get_id_product(dev_rotate));
    printf("Input device name: \"%s\"\n", libevdev_get_name(dev_click));
    printf("Input device ID: bus %#x vendor %#x product %#x\n",
        libevdev_get_id_bustype(dev_click),
        libevdev_get_id_vendor(dev_click),
        libevdev_get_id_product(dev_click));
    
    do {
            struct input_event ev_rotate, ev_click;
            rc = libevdev_next_event(dev_rotate, LIBEVDEV_READ_FLAG_NORMAL, &ev_rotate);
            if (rc == 0)
            {
                printf("Event: %s %s %d\n",
                    libevdev_event_type_get_name(ev_rotate.type),
                    libevdev_event_code_get_name(ev_rotate.type, ev_rotate.code),
                    ev_rotate.value);

                if (ev_rotate.type == EV_REL)
                {
                    menu_index += ev_rotate.value;

                    if (menu_index > MAX_INDEX)
                    {
                        menu_index = MAX_INDEX;
                    }
                    else if (menu_index < MIN_INDEX)
                    {
                        menu_index = MIN_INDEX;
                    }
                }
                
                redraw();
            }

            rc = libevdev_next_event(dev_click, LIBEVDEV_READ_FLAG_NORMAL, &ev_click);
            if (rc == 0)
            {
                std::string event_code_name{ libevdev_event_code_get_name(ev_click.type, ev_click.code) };


                printf("Event: %s %s %d\n",
                        libevdev_event_type_get_name(ev_click.type),
                        event_code_name.c_str(),
                        ev_click.value);

                if (event_code_name == "KEY_ENTER" && ev_click.value == 0)
                {
                    if (menu_index == 3)
                    {
                        oled->displayOff();
                        exit(0);
                    }

                    if (displayToggle)
                    {
                        oled->displayOff();
                    }
                    else
                    {
                        oled->displayOn();
                    }
                    displayToggle = !displayToggle;
                }
            }

            usleep(100);
    } while (rc == 1 || rc == 0 || rc == -EAGAIN);

}