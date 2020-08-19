#ifndef EVDEVROTARYCONTROLLER_H
#define EVDEVROTARYCONTROLLER_H


#include "libevdev-1.0/libevdev/libevdev.h"

#include "BasicTask.h"
#include "input/IMenuInputController.h"

namespace textmenu
{
    namespace input
    {
        class EvdevRotaryController : public BasicTask, public IMenuInputController
        {
        public:
            EvdevRotaryController(const std::string& rotary_path, const std::string& button_path, int polling_interval = 100);
            ~EvdevRotaryController() override;

            void RegisterRelativeVerticalHandler(RelativeVerticalHandler callback) override;
            void RegisterSelectButtonHandler(PushButtonHandler callback) override;

        private:
            void ThreadFunction() override;

            const int m_polling_interval;

            int m_fd_rotary;
            int m_fd_button;

            // eww. try to hide with smart pointers?
            libevdev* m_dev_rotary;
            libevdev* m_dev_button;

            RelativeVerticalHandler m_rotation_handler;
            PushButtonHandler m_button_handler;
        };
    }
}

#endif //EVDEVROTARYCONTROLLER_H