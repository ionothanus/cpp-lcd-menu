// Copyright (c) 2020 Jonathan Moscardini
// 
// Licensed under the MIT License
// http://www.opensource.org/licenses/MIT

#ifndef DISPLAY_IMENURENDERER_H
#define DISPLAY_IMENURENDERER_H

#include "menu/MenuTypes.h"

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

            /// @brief Draws a MenuEntryList on the chosen display, windowed
            ///        based on the provided parameters to the display's
            ///        capabilities.
            /// 
            /// @param menu The menu to render.
            /// @param list_start_index The index in the list of options to start rendering at. The
            ///                         menu will automatically truncate to the available display rows
            ///                         if there are more entries than it is possible to render.
            /// @param selected_index The currently-selected row.
            /// @param selected_line_start_index The character on the selected row to begin rendering at
            ///                                  (for scrolling).
            virtual void DrawMenuList(const menu::MenuEntryList& menu, int list_start_index, int selected_index, int selected_line_start_index) = 0;

            /// @brief Clear the screen and print a single message.
            /// 
            /// @param message The message to print.
            virtual void DrawOverlay(const std::string& message) = 0;

            /// @brief Turn the display off.
            virtual void Sleep() = 0;

            /// @brief Turn the display on.
            virtual void Wake() = 0;

            /// @brief Get the maximum number of rows the display can render
            /// 
            /// @return int The maximum number of rows the display can render.
            virtual int GetMaxRows() = 0;

            /// @brief Get the maximum length of a line.
            /// 
            /// @return int The maximum number of characters this display can
            ///             render on a line.
            virtual int GetMaxLineLength() = 0;

            /// @brief Get the state of the screen (on or off).
            /// 
            /// @return ScreenState (on or off).
            virtual ScreenState GetScreenState() = 0;

            /// @brief Indicates if the display uses a character
            ///        to indicate the selected row, or if it
            ///        uses an alternate method that does not
            ///        consume a visible character (e.g. inverting
            ///        the foreground/background colour).
            /// 
            /// @return true if a character is consumed to mark the selected line
            ///         (a selected line is NOT the same length as an unselected line)
            /// @return false if a character is not consumed to mark the selected line
            ///         (a selected line IS the same length as an unselected line)
            virtual bool SelectionIsCharacter() = 0;
        };
    }
}

#endif //DISPLAY_IMENURENDERER_H