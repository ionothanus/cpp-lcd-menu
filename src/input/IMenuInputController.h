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

            virtual void RegisterRelativeVerticalHandler(RelativeVerticalHandler callback) = 0;
            virtual void RegisterSelectButtonHandler(PushButtonHandler callback) = 0;
        };
    }
}

#endif //IMENUINPUTCONTROLLER_H