#ifndef ADAFRUIT16X2BUTTONCONTROLLER_H
#define ADAFRUIT16X2BUTTONCONTROLLER_H

#include "BasicTask.h"
#include "input/IMenuInputController.h"
#include "driver/Adafruit16x2DeviceWrapper.h"

namespace textmenu
{
    namespace input
    {
        class Adafruit16x2ButtonController : public IMenuInputController, public BasicTask
        {
        public:
            Adafruit16x2ButtonController(std::shared_ptr<driver::Adafruit16x2DeviceWrapper> lcd,
                                         int polling_interval = 100);
            ~Adafruit16x2ButtonController() override = default;

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
