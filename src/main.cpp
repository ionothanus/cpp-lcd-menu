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

#include "src/display/ssd1306/SSD1306MenuRenderer.h"

textmenu::display::SSD1306MenuRenderer oled{};
textmenu::MenuList default_list{textmenu::MenuEntry{"Hello, not cruel world! I am so thrilled to be here today!", textmenu::MenuList{}},
                                textmenu::MenuEntry{"Goodbye, sort of cruel world!", textmenu::MenuList{}},
                                textmenu::MenuEntry{"Goodbye, crueller world!", textmenu::MenuList{}},
                                textmenu::MenuEntry{"Goodbye, cruelest world!", textmenu::MenuList{}},
                                textmenu::MenuEntry{"Goodbye, less-cruel world!", textmenu::MenuList{}}};

void signal_handler(int signum)
{
    oled.UpdateEntries(textmenu::MenuList{});
    oled.Sleep();
    oled.Stop();
    exit(signum);
}

const int MIN_INDEX = 0;
const int MAX_INDEX = 3;

int main()
{
    oled.Run();

    std::cout << "Hello worlds!" << std::endl;

    bool displayToggle = true;

    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);

    oled.UpdateEntries(default_list);

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
                oled.UpdateIndex(ev_rotate.value);
            }
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
                if (oled.GetCurrentIndex() == 3)
                {
                    oled.Sleep();
                    oled.Stop();
                    exit(0);
                }

                if (displayToggle)
                {
                    oled.Sleep();
                }
                else
                {
                    oled.Wake();
                }
                displayToggle = !displayToggle;
            }
        }

        usleep(100);
    } while (rc == 1 || rc == 0 || rc == -EAGAIN);
}
