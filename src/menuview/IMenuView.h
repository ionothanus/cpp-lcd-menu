// Copyright (c) 2020 Jonathan Moscardini
// 
// Licensed under the MIT License
// http://www.opensource.org/licenses/MIT

#ifndef SRC_IMENURENDERER_H
#define SRC_IMENURENDERER_H

#include <chrono>
#include <vector>

#include "menu/MenuTypes.h"

namespace textmenu
{
    class IMenuView
    {
    public:
        virtual ~IMenuView() noexcept = default;

        /// @brief Replace the current list of entries.
        /// @param entries: the list of MenuEntries to display
        virtual void LoadNewMenu(const menu::MenuEntryList& entries) = 0;

        /// @brief Draw a temporary message overlay (like a pop-up notification).
        /// @param text: message to display
        /// @param duration: duration to display before reverting to the menu
        virtual void RequestTimedOverlay(const std::string& message,
                                 std::chrono::system_clock::duration duration) = 0;

        /// @brief Request the selected/highlighted index change by a given offset.
        ///        Will clamp requests to the list limits automatically.
        ///        May also ignore requests if changing the index would have an undesired effect
        ///        (e.g. to avoid changing state while the display is asleep)
        /// @param rel_offset: the relative offset to change the index by
        virtual void RequestIndexChange(int rel_offset) = 0;

        /// @brief Gets the index of the currently-selected menu entry.
        /// @return the index of the currently-selected entry
        virtual int GetCurrentIndex() = 0;

    protected:
        enum class ScrollDirection
        {
            None,
            Forward,
            Reverse,
            Hold
        };
    };
}

#endif //SRC_IMENURENDERER_H