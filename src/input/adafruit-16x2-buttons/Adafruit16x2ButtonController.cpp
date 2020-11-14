// Copyright (c) 2020 Jonathan Moscardini
// 
// Licensed under the MIT License
// http://www.opensource.org/licenses/MIT

#include "input/adafruit-16x2-buttons/Adafruit16x2ButtonController.h"

namespace textmenu
{
    namespace input
    {

        Adafruit16x2ButtonController::Adafruit16x2ButtonController(std::shared_ptr<driver::Adafruit16x2DeviceWrapper> lcd,
                                                                   int polling_interval)
            : m_lcd{lcd},
              m_polling_interval{polling_interval}
        {}

        void Adafruit16x2ButtonController::RegisterRelativeVerticalHandler(RelativeVerticalHandler callback)
        {
            m_updown_handler = callback;
        }
        void Adafruit16x2ButtonController::RegisterSelectButtonHandler(PushButtonHandler callback)
        {
            m_select_handler = callback;
        }

        void toggle(bool& bit)
        {
            bit = !bit;
        }

        void Adafruit16x2ButtonController::ThreadFunction()
        {
            uint8_t last_buttons;

            while (!m_terminate)
            {
                uint8_t buttons;
                
                {
                    std::unique_lock<std::mutex> lock{m_cv_mutex};
                    m_thread_cv.wait_for(lock,
                                        std::chrono::milliseconds(m_polling_interval),
                                        [&] () 
                                        { 
                                            buttons = m_lcd->GetLcdHandle()->readButtons();
                                            bool change{ buttons != last_buttons };

                                            return ((m_terminate == true) ||
                                                    change); 
                                        });
                }

                if (buttons != 0)
                {
                    int rel_offset = 0;

                    if ((buttons & BUTTON_UP) && !(last_buttons & BUTTON_UP))
                    {
                        rel_offset += -1;
                    }

                    if ((buttons & BUTTON_RIGHT) && !(last_buttons & BUTTON_RIGHT))
                    {
                        rel_offset += 1;
                    }

                    if ((buttons & BUTTON_DOWN) && !(last_buttons & BUTTON_DOWN))
                    {
                        rel_offset += 1;
                    }

                    if ((buttons & BUTTON_LEFT) && !(last_buttons & BUTTON_LEFT))
                    {
                        rel_offset += -1;
                    }

                    if (rel_offset != 0)
                    {
                        m_updown_handler(rel_offset);
                    }
                }

                if ((buttons & BUTTON_SELECT) && !(last_buttons & BUTTON_SELECT))
                {
                    m_select_handler(true);
                }
                else if (!(buttons & BUTTON_SELECT) && (last_buttons & BUTTON_SELECT))
                {
                    m_select_handler(false);
                }

                last_buttons = buttons;
            }
        }
    }
}
