// Copyright (c) 2020 Jonathan Moscardini
// 
// Licensed under the MIT License
// http://www.opensource.org/licenses/MIT

#ifndef ADAFRUIT16X2BUTTONCONTROLLER_H
#define ADAFRUIT16X2BUTTONCONTROLLER_H

#include "BasicTask.h"
#include "input/IMenuInputController.h"
#include "driver/Adafruit16x2DeviceWrapper.h"

namespace textmenu
{
    namespace input
    {
        /// @brief Polls the buttons on an Adafruit I2C 16x2 LCD + button plate.
        class Adafruit16x2ButtonController : public IMenuInputController, public BasicTask
        {
        public:
            /// @brief Construct a new Adafruit16x2ButtonController object
            /// 
            /// @param lcd The Adafruit16x2DeviceWrapper object wrapping the hardware device.
            /// @param polling_interval The rate to poll the hardware for button presses.
            Adafruit16x2ButtonController(std::shared_ptr<driver::Adafruit16x2DeviceWrapper> lcd,
                                         int polling_interval = 100);
            ~Adafruit16x2ButtonController() override = default;

            // IMenuInputController overrides
            // These are commented in IMenuInputController.h.
            void RegisterRelativeVerticalHandler(RelativeVerticalHandler callback) override;
            void RegisterSelectButtonHandler(PushButtonHandler callback) override;

        private:
            void ThreadFunction();

            std::shared_ptr<driver::Adafruit16x2DeviceWrapper> m_lcd;

            int m_polling_interval;

            RelativeVerticalHandler m_updown_handler;
            PushButtonHandler m_select_handler;
        };
    }
}

#endif //ADAFRUIT16X2BUTTONCONTROLLER_H
