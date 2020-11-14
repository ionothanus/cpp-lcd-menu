// Copyright (c) 2020 Jonathan Moscardini
// 
// Licensed under the MIT License
// http://www.opensource.org/licenses/MIT

#ifndef IMENUINPUTCONTROLLER_H
#define IMENUINPUTCONTROLLER_H

#include <functional>

namespace textmenu
{
    namespace input
    {
        class IMenuInputController
        {
        public:
            using RelativeVerticalHandler = std::function<void(int)>;
            using PushButtonHandler = std::function<void(bool)>;

            virtual ~IMenuInputController() = default;

            /// @brief Register a callback function for user inputs requesting
            ///        vertical menu navigation (e.g. rotating a rotary controller,
            ///        pressing up/down)
            /// 
            /// @param callback The function to be called on vertical user inputs
            ///                 (e.g. rotating a rotary controller, pressing up/down)
            virtual void RegisterRelativeVerticalHandler(RelativeVerticalHandler callback) = 0;

            /// @brief Register a callback function for a select button.
            /// 
            /// @param callback The function to be called when a user presses the select button.
            virtual void RegisterSelectButtonHandler(PushButtonHandler callback) = 0;
        };
    }
}

#endif //IMENUINPUTCONTROLLER_H