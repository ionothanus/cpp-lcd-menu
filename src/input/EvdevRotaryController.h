#ifndef EVDEVROTARYCONTROLLER_H
#define EVDEVROTARYCONTROLLER_H

#include <functional>
#include "libevdev-1.0/libevdev/libevdev.h"

#include "BasicTask.h"

namespace textmenu
{
    namespace input
    {
        class EvdevRotaryController : public BasicTask
        {
        public:
            using RelativeRotationHandler = std::function<void(int)>;
            using PushButtonHandler = std::function<void(bool)>;

            EvdevRotaryController(const std::string& rotary_path, const std::string& button_path, int polling_interval = 100);
            ~EvdevRotaryController() override;

            void RegisterRelativeRotationHandler(RelativeRotationHandler callback);
            void RegisterPushButtonHandler(PushButtonHandler callback);

        private:
            void ThreadFunction() override;

            const int m_polling_interval;

            int m_fd_rotary;
            int m_fd_button;

            // eww. try to hide with smart pointers?
            libevdev* m_dev_rotary;
            libevdev* m_dev_button;

            RelativeRotationHandler m_rotation_handler;
            PushButtonHandler m_button_handler;
        };
    }
}

#endif //EVDEVROTARYCONTROLLER_H