#include <algorithm>

#include "libSSD1306/lib/OledGraphics.h"
#include "SSD1306MenuRenderer.h"

namespace textmenu
{
    namespace display
    {
        const SSD1306::OledBitmap<SSD1306::sc_fontWidth8x16, SSD1306::sc_fontHeight8x16> DOWN_ARROW {
            0b00000000,
            0b00000000,
            0b00111100,
            0b00111100,
            0b00111100,
            0b00111100,
            0b00111100,
            0b00111100,
            0b00111100,
            0b00111100,
            0b00111100,
            0b00111100,
            0b11111111,
            0b01111110,
            0b00111100,
            0b00011000
        };

        const SSD1306::OledBitmap<SSD1306::sc_fontWidth8x16, SSD1306::sc_fontHeight8x16> UP_ARROW {
            0b00011000,
            0b00111100,
            0b01111110,
            0b11111111,
            0b00111100,
            0b00111100,
            0b00111100,
            0b00111100,
            0b00111100,
            0b00111100,
            0b00111100,
            0b00111100,
            0b00111100,
            0b00111100,
            0b00000000,
            0b00000000
        };

        SSD1306MenuRenderer::SSD1306MenuRenderer()
            : m_thread{},
              m_cv_mutex{},
              m_thread_cv{},
              m_local_list{},
              m_oled{"/dev/i2c-1", 0x3D},
              m_terminate{false},
              m_update_required{false},
              m_selected_index{0},
              m_display_start_index{0},
              m_selected_line_start_index{0},
              m_scroll_hold_counter{0},
              m_full_scroll_completed{false},
              m_scroll_direction{ScrollDirection::None},
              m_sleep_counter{0},
              m_screen_state{ScreenState::On}
        {
            m_oled.clear();
            m_oled.displayUpdate();
        }

        SSD1306MenuRenderer::~SSD1306MenuRenderer()
        {
            m_oled.clear();
            m_oled.displayUpdate();
            m_oled.displayOff();
        }

        /// Public API

        void SSD1306MenuRenderer::Run()
        {
            if (!m_thread.joinable())
            {
                m_terminate = false;
                m_thread = std::thread{[this](){ RenderThread(); } };
            }
        }

        void SSD1306MenuRenderer::Stop()
        {
            m_terminate = true;
            m_thread_cv.notify_all();

            if (m_thread.joinable())
            {
                m_thread.join();
            }
        }

        void SSD1306MenuRenderer::UpdateEntries(const MenuList& entries)
        {
            {
                std::lock_guard<std::mutex> lock {m_cv_mutex};
                m_local_list = entries;
                m_update_required = true;
            }
            
            m_thread_cv.notify_one();
        }

        void SSD1306MenuRenderer::UpdateIndex(int rel_offset)
        {
            {
                std::lock_guard<std::mutex> lock {m_cv_mutex};
                m_selected_index = std::clamp(m_selected_index += rel_offset,
                                              MIN_LIST_INDEX,
                                              static_cast<int>(m_local_list.size() - 1));

                if (rel_offset > 0 && m_selected_index - m_display_start_index > (MAX_ENTRIES - 1))
                {
                    m_display_start_index = std::clamp(++m_display_start_index,
                                                       0,
                                                       static_cast<int>(m_local_list.size() - MAX_ENTRIES));
                }
                else if (rel_offset < 0 && m_selected_index < m_display_start_index)
                {
                    m_display_start_index = std::clamp(--m_display_start_index,
                                    0,
                                    static_cast<int>(m_local_list.size() - MAX_ENTRIES));
                }

                m_update_required = true;
            }

            m_thread_cv.notify_one();
        }

        int SSD1306MenuRenderer::GetCurrentIndex()
        {
            return m_selected_index;
        }

        void SSD1306MenuRenderer::Sleep()
        {
            m_oled.displayOff();
            m_screen_state = ScreenState::Off;
        }

        void SSD1306MenuRenderer::Wake()
        {
            m_oled.displayOn();
            m_screen_state = ScreenState::On;
        }

        /// Worker thread implementations

        void SSD1306MenuRenderer::RenderThread()
        {
            while (!m_terminate)
            {
                std::unique_lock<std::mutex> lock{m_cv_mutex};
                m_thread_cv.wait_for(lock,
                                    std::chrono::milliseconds(SCROLL_DELAY_MS),
                                    [this] () { return ((m_terminate == true) ||
                                                        (m_update_required == true)); });
                
                if (m_terminate)
                {
                    break;
                }

                if (m_update_required)
                {
                    if (m_screen_state == ScreenState::Off)
                    {
                        Wake();
                    }

                    m_scroll_direction = ScrollDirection::None;
                    m_scroll_hold_counter = 0;
                    m_selected_line_start_index = 0;
                    m_full_scroll_completed = false;
                    m_sleep_counter = 0;

                    RedrawMenu();

                    m_update_required = false;
                }
                else if (m_screen_state == ScreenState::On)
                {
                    if (ScrollSelectedLine())
                    {
                        RedrawMenu();
                    }

                    if (m_full_scroll_completed)
                    {
                        if (m_sleep_counter > MAX_SLEEP_COUNT)
                        {
                            Sleep();
                        }
                        else
                        {
                            m_sleep_counter++;
                        }
                    }
                }
            }
        }

        bool SSD1306MenuRenderer::ScrollSelectedLine()
        {
            // these two conditions will account for the one-character arrows drawn
            // at the end of the first or last rows, if there are off-screen entries
            // above or below the current window
            bool selectedFirstRenderedLine{ (m_local_list.size() > MAX_ENTRIES 
                                            && (m_selected_index - m_display_start_index) == 0
                                            && m_display_start_index != 0) };
            bool selectedLastRenderedLine{ (m_local_list.size() > MAX_ENTRIES 
                                            && (m_selected_index - m_display_start_index == MAX_ENTRIES - 1)
                                            && m_display_start_index < (m_local_list.size() - MAX_ENTRIES)) };

            int maxWidth =  (selectedFirstRenderedLine || selectedLastRenderedLine)
                            ? MAX_LINE_LENGTH - 1
                            : MAX_LINE_LENGTH;
            int lineLength = m_local_list[m_selected_index].displayValue.length();

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

            return false;
        }

        void SSD1306MenuRenderer::RedrawMenu()
        {
            SSD1306::OledBitmap<SSD1306::OledI2C::Width, SSD1306::sc_fontHeight8x16> menu_selected{};
            SSD1306::OledBitmap<SSD1306::OledI2C::Width, SSD1306::OledI2C::Height> buffer{};
            menu_selected.fill();
            buffer.clear();

            int rowCount{ 0 };

            for (int i = m_display_start_index; i < m_local_list.size(); i++)
            {
                MenuEntry entry = m_local_list[i];
                std::string string_to_render{ entry.displayValue };

                if (i == m_selected_index)
                {
                    buffer.setFrom(menu_selected, SSD1306::OledPoint{0, rowCount * SSD1306::sc_fontHeight8x16});
                    string_to_render = string_to_render.substr(m_selected_line_start_index, string_to_render.length() - m_selected_line_start_index);
                }

                SSD1306::drawString8x16(SSD1306::OledPoint{0, rowCount * SSD1306::sc_fontHeight8x16},
                                        string_to_render,
                                        ((i == m_selected_index) ? SSD1306::PixelStyle::Unset 
                                                                        : SSD1306::PixelStyle::Set),
                                        buffer);

                if (rowCount == 0 && m_display_start_index != 0)
                {
                    buffer.setFrom(UP_ARROW, SSD1306::OledPoint{SSD1306::OledI2C::Width - SSD1306::sc_fontWidth8x16, 0});
                }
                else if (rowCount == (MAX_ENTRIES - 1) && m_display_start_index == 0)
                {
                    buffer.setFrom(DOWN_ARROW, SSD1306::OledPoint{SSD1306::OledI2C::Width - SSD1306::sc_fontWidth8x16, 
                                                                  rowCount * SSD1306::sc_fontHeight8x16});
                    break;
                }

                rowCount++;
            }

            m_oled.setFrom(buffer);

            m_oled.displayUpdate();
        }
    }
}