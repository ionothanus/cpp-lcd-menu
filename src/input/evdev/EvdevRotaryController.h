// Copyright (c) 2020 Jonathan Moscardini
// 
// Licensed under the MIT License
// http://www.opensource.org/licenses/MIT

#ifndef EVDEVROTARYCONTROLLER_H
#define EVDEVROTARYCONTROLLER_H


#include "libevdev-1.0/libevdev/libevdev.h"

#include "BasicTask.h"
#include "input/IMenuInputController.h"
#include "menu/MenuLoader.h"

namespace textmenu
{
    namespace input
    {
        /// @brief Polls an evdev-managed rotary controller for user inputs.
        class EvdevRotaryController : public BasicTask, public IMenuInputController
        {
        public:
            /// @brief Construct a new EvdevRotaryController object
            /// 
            /// @param rotary_path The path to the rotary controller evdev device 
            ///                    (e.g. "/dev/input/event0")
            /// @param button_path The path to the select button evdev device
            ///                    (e.g. "/dev/input/event1")
            /// @param polling_interval The rate at which to poll the evdev devices for events
            EvdevRotaryController(const std::string& rotary_path, const std::string& button_path, int polling_interval = 100);
            ~EvdevRotaryController() override;

            /// @brief Construct a new EvdevRotaryController based on a configuration map
            ///        (e.g. from a YAML menu.)
            /// 
            /// @param config The SettingsMap containing the appropriate device settings.
            /// @return std::unique_ptr<EvdevRotaryController> 
            static std::unique_ptr<EvdevRotaryController> ConstructFromConfigMap(const menu::SettingsMap& config);

            // IMenuInputController overrides
            // These are commented in IMenuInputController.h.
            void RegisterRelativeVerticalHandler(RelativeVerticalHandler callback) override;
            void RegisterSelectButtonHandler(PushButtonHandler callback) override;

        private:
            void ThreadFunction() override;

            const int m_polling_interval;

            int m_fd_rotary;
            int m_fd_button;

            //TODO: #3 eww. try to hide with smart pointers?
            libevdev* m_dev_rotary;
            libevdev* m_dev_button;

            RelativeVerticalHandler m_rotation_handler;
            PushButtonHandler m_button_handler;
        };
    }
}

#endif //EVDEVROTARYCONTROLLER_H