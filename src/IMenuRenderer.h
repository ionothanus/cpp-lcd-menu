#ifndef SRC_IMENURENDERER_H
#define SRC_IMENURENDERER_H

#include <vector>

#include "MenuEntry.h"

namespace textmenu
{
    class IMenuRenderer
    {
    public:
        virtual ~IMenuRenderer() noexcept = default;

        virtual void UpdateEntries(const MenuList& entries) = 0;
        virtual void UpdateIndex(int rel_offset) = 0;
        virtual int GetCurrentIndex() = 0;
        virtual void Sleep() = 0;
        virtual void Wake() = 0;
    };
}


#endif //SRC_IMENURENDERER_H