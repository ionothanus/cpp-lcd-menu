// Copyright (c) 2020 Jonathan Moscardini
// 
// Licensed under the MIT License
// http://www.opensource.org/licenses/MIT

#ifndef SRC_DISPLAY_TEXTMENURENDERER_H
#define SRC_DISPLAY_TEXTMENURENDERER_H

#include <memory>
#include <shared_mutex>
#include <chrono>

#include "menuview/IMenuView.h"
#include "BasicTask.h"
#include "display/IMenuRenderer.h"

namespace textmenu
{
    /// @brief Handles rendering a menu on a character LCD,
    ///        including selection highlighting and long-line scrolling.
    class TextMenuView : public IMenuView, public BasicTask
    {
    public:
        /// @brief Construct a new TextMenuView object
        /// 
        /// @param menu_renderer The device used to render this menu.
        TextMenuView(std::unique_ptr<display::IMenuRenderer> menu_renderer);
        ~TextMenuView() override = default;

        // IMenuView overrides
        // These are described in IMenuView.h.
        void LoadNewMenu(const menu::MenuEntryList& entries) override;
        void RequestTimedOverlay(const std::string& message,
                                 std::chrono::system_clock::duration duration) override;
        void RequestIndexChange(int rel_offset) override;
        int GetCurrentIndex() override;

    private:
        using clock = std::chrono::system_clock;

        void ThreadFunction() override;
        void UpdateIndex(int this_requested_index_change);
        void ResetScrollState();
        bool ScrollSelectedLine(const menu::MenuEntryList& list);
        void RenderOverlay(const std::string& message,
                           std::chrono::system_clock::duration duration);

        std::unique_ptr<display::IMenuRenderer> m_menu_renderer;

        // input variables from consumers - must be thread-safe
        menu::MenuEntryList m_local_list;
        std::atomic<bool> m_update_required;
        std::atomic<int> m_requested_index_change;
        std::atomic<bool> m_overlay_requested;
        std::chrono::system_clock::duration m_overlay_duration;
        std::string m_overlay_message;

        // output variables for consumers - must be thread-safe
        std::atomic<int> m_selected_index;

        // internal display state variables
        int m_display_start_index;
        int m_selected_line_start_index;
        int m_scroll_hold_counter;
        bool m_full_scroll_completed;
        ScrollDirection m_scroll_direction;
        clock::time_point m_last_input_time;

        static constexpr int MIN_LIST_INDEX{ 0 };
        static constexpr int SCROLL_DELAY_MS{ 250 };
        static constexpr int MAX_SCROLL_HOLD{ 4 };
        static constexpr clock::duration MIN_SCREEN_ON_TIME{ std::chrono::seconds(5) };
    };
}

#endif //SRC_DISPLAY_TEXTMENURENDERER_H