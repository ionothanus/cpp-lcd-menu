#ifndef DISPLAY_IMENURENDERER_H
#define DISPLAY_IMENURENDERER_H

#include "MenuEntry.h"

namespace textmenu
{
    namespace display
    {
        enum class ScreenState
        {
            On,
            Off
        };

        class IMenuRenderer
        {
        public:
            virtual ~IMenuRenderer() = default;

            virtual void DrawMenuList(const MenuList& menu, int list_start_index, int selected_index, int selected_line_start_index) = 0;
            virtual void Sleep() = 0;
            virtual void Wake() = 0;
            virtual int GetMaxRows() = 0;
            virtual int GetMaxLineLength() = 0;
            virtual ScreenState GetScreenState() = 0;
        };
    }
}

#endif //DISPLAY_IMENURENDERER_H