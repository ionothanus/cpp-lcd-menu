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

        /// @brief Update the current list of entries.
        /// @param entries: the list of MenuEntries to display
        virtual void UpdateEntries(const MenuList& entries) = 0;

        /// @brief Request the selected/highlighted index change by a given offset.
        ///        Will clamp requests to the list limits automatically.
        ///        May also ignore requests if changing the index would have an undesired effect
        ///        (e.g. to avoid changing state while the display is asleep)
        /// @param rel_offset: the relative offset to change the index by
        virtual void RequestIndexChange(int rel_offset) = 0;

        /// @brief Gets the index of the currently-selected menu entry.
        /// @return the index of the currently-selected entry
        virtual int GetCurrentIndex() = 0;
    };
}


#endif //SRC_IMENURENDERER_H