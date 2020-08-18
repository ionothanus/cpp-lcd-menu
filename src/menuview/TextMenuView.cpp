
#include "menuview/TextMenuView.h"

#include <algorithm>
#include <mutex>


namespace textmenu
{
    TextMenuView::TextMenuView(std::unique_ptr<display::IMenuRenderer> menu_renderer)
        : m_menu_renderer{std::move(menu_renderer)},
          m_local_list{},
          m_update_required{false},
          m_requested_index_change{0},
          m_selected_index{0},
          m_display_start_index{0},
          m_selected_line_start_index{0},
          m_scroll_hold_counter{0},
          m_full_scroll_completed{false},
          m_scroll_direction{ScrollDirection::None},
          m_sleep_counter{0}
    {}

    void TextMenuView::UpdateEntries(const MenuList& entries)
    {
        {
            std::lock_guard<std::mutex> lock {m_cv_mutex};
            m_local_list = entries;
            m_update_required = true;
        }
        
        m_thread_cv.notify_one();
    }

    void TextMenuView::RequestIndexChange(int rel_offset)
    {
        {
            std::lock_guard<std::mutex> lock {m_cv_mutex};
            m_requested_index_change = rel_offset;
            m_update_required = true;
        }

        m_thread_cv.notify_one();
    }

    int TextMenuView::GetCurrentIndex()
    {
        return m_selected_index;
    }

    // private thread functions

    void TextMenuView::ThreadFunction()
    {
        MenuList this_list;
        bool this_update_required;
        bool this_terminate;
        int this_requested_index_change;

        while (!m_terminate)
        {
            {
                std::unique_lock<std::mutex> lock{m_cv_mutex};
                m_thread_cv.wait_for(lock,
                                    std::chrono::milliseconds(SCROLL_DELAY_MS),
                                    [this] () { return ((m_terminate == true) ||
                                                        (m_update_required == true)); });

                // make local copies so we can release the lock
                this_list = m_local_list;
                this_update_required = m_update_required;
                this_terminate = m_terminate;
                this_requested_index_change = m_requested_index_change;

                m_update_required = false;
                m_requested_index_change = 0;
            }
            
            if (this_terminate)
            {
                break;
            }

            UpdateIndex(this_requested_index_change);

            bool draw = false;

            if (this_update_required)
            {
                draw = true;

                if (m_menu_renderer->GetScreenState() == display::ScreenState::Off)
                {
                    m_menu_renderer->Wake();
                }

                ResetScrollState();
            }
            else if (m_menu_renderer->GetScreenState() == display::ScreenState::On)
            {
                if (ScrollSelectedLine(this_list))
                {
                    draw = true;
                }

                if (m_full_scroll_completed)
                {
                    if (m_sleep_counter > MAX_SLEEP_COUNT)
                    {
                        m_menu_renderer->Sleep();
                    }
                    else
                    {
                        m_sleep_counter++;
                    }
                }
            }

            if (draw)
            {
                m_menu_renderer->DrawMenuList(this_list,
                                              m_display_start_index,
                                              m_selected_index,
                                              m_selected_line_start_index);
            }
        }
    }

    void TextMenuView::UpdateIndex(int this_requested_index_change)
    {
        if (m_menu_renderer->GetScreenState() == display::ScreenState::On)
        {
            m_selected_index = std::clamp(m_selected_index += this_requested_index_change,
                                            MIN_LIST_INDEX,
                                            static_cast<int>(m_local_list.size() - 1));

            if (this_requested_index_change > 0 && m_selected_index - m_display_start_index > (m_menu_renderer->GetMaxRows() - 1))
            {
                m_display_start_index = std::clamp(++m_display_start_index,
                                                    0,
                                                    static_cast<int>(m_local_list.size() - m_menu_renderer->GetMaxRows()));
            }
            else if (this_requested_index_change < 0 && m_selected_index < m_display_start_index)
            {
                m_display_start_index = std::clamp(--m_display_start_index,
                                                    0,
                                                    static_cast<int>(m_local_list.size() - m_menu_renderer->GetMaxRows()));
            }
        }
    }

    void TextMenuView::ResetScrollState()
    {
        m_scroll_direction = ScrollDirection::None;
        m_scroll_hold_counter = 0;
        m_selected_line_start_index = 0;
        m_full_scroll_completed = false;
        m_sleep_counter = 0;
    }

    bool TextMenuView::ScrollSelectedLine(const MenuList& list)
    {
        if (list.size() > 0)
        {
            // these two conditions will account for the one-character arrows drawn
            // at the end of the first or last rows, if there are off-screen entries
            // above or below the current window
            bool selectedFirstRenderedLine{ (list.size() > m_menu_renderer->GetMaxRows() 
                                            && (m_selected_index - m_display_start_index) == 0
                                            && m_display_start_index != 0) };
            bool selectedLastRenderedLine{ (list.size() > m_menu_renderer->GetMaxRows() 
                                            && (m_selected_index - m_display_start_index == m_menu_renderer->GetMaxRows() - 1)
                                            && m_display_start_index < (list.size() - m_menu_renderer->GetMaxRows())) };

            int maxWidth =  (selectedFirstRenderedLine || selectedLastRenderedLine)
                            ? m_menu_renderer->GetMaxRows() - 1
                            : m_menu_renderer->GetMaxRows();
            int lineLength = list[m_selected_index].displayValue.length();

            if (lineLength > maxWidth)
            {
                if (m_scroll_direction == ScrollDirection::None)
                {
                    m_scroll_hold_counter++;

                    if (m_scroll_hold_counter > MAX_SCROLL_HOLD)
                    {
                        m_scroll_hold_counter = 0;
                        m_scroll_direction = ScrollDirection::Forward;
                    }
                }
                else if (m_scroll_direction == ScrollDirection::Forward)
                {
                    m_selected_line_start_index = std::clamp(++m_selected_line_start_index,
                                                                0,
                                                                lineLength - maxWidth);

                    if (m_selected_line_start_index == lineLength - maxWidth)
                    {
                        m_scroll_direction = ScrollDirection::Hold;
                    }

                    return true;
                }
                else if (m_scroll_direction == ScrollDirection::Reverse)
                {
                    m_selected_line_start_index = std::clamp(--m_selected_line_start_index,
                                                                0,
                                                                lineLength - maxWidth);

                    if (m_selected_line_start_index == 0)
                    {
                        m_scroll_direction = ScrollDirection::Hold;
                    }

                    return true;
                }
                else if (m_scroll_direction == ScrollDirection::Hold)
                {
                    m_scroll_hold_counter++;

                    if (m_scroll_hold_counter > MAX_SCROLL_HOLD)
                    {
                        m_scroll_hold_counter = 0;

                        if (m_selected_line_start_index == 0)
                        {
                            m_scroll_direction = ScrollDirection::Forward;
                            m_full_scroll_completed = true;
                        }
                        else
                        {
                            m_scroll_direction = ScrollDirection::Reverse;
                        }
                    }

                    return false;
                }
            }
        }

        return false;
    }
}
