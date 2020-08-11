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
            :

              m_oled{"/dev/i2c-1", 0x3D},

              m_screen_state{ScreenState::On}
        {
            m_oled.clear();
            m_oled.displayUpdate();
        }

        SSD1306MenuRenderer::~SSD1306MenuRenderer()
        {
            m_oled.displayOff();
            m_oled.clear();
            m_oled.displayUpdate();
        }

        /// Public API

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

        int SSD1306MenuRenderer::GetMaxRows()
        {
            return MAX_ROWS;
        }

        int SSD1306MenuRenderer::GetMaxLineLength()
        {
            return MAX_LINE_LENGTH;
        }

        ScreenState SSD1306MenuRenderer::GetScreenState()
        {
            return m_screen_state;
        }

        void SSD1306MenuRenderer::DrawMenuList(const MenuList& menu, int list_start_index, int selected_index, int selected_line_start_index)
        {
            SSD1306::OledBitmap<SSD1306::OledI2C::Width, SSD1306::sc_fontHeight8x16> menu_selected{};
            SSD1306::OledBitmap<SSD1306::OledI2C::Width, SSD1306::OledI2C::Height> buffer{};
            menu_selected.fill();
            buffer.clear();

            int rowCount{ 0 };

            for (int i = list_start_index; i < menu.size(); i++)
            {
                MenuEntry entry = menu[i];
                std::string string_to_render{ entry.displayValue };

                if (i == selected_index)
                {
                    buffer.setFrom(menu_selected, SSD1306::OledPoint{0, rowCount * SSD1306::sc_fontHeight8x16});
                    string_to_render = string_to_render.substr(selected_line_start_index, string_to_render.length() - selected_line_start_index);
                }

                SSD1306::drawString8x16(SSD1306::OledPoint{0, rowCount * SSD1306::sc_fontHeight8x16},
                                        string_to_render,
                                        ((i == selected_index) ? SSD1306::PixelStyle::Unset 
                                                                        : SSD1306::PixelStyle::Set),
                                        buffer);

                if (rowCount == 0 && list_start_index != 0)
                {
                    buffer.setFrom(UP_ARROW, SSD1306::OledPoint{SSD1306::OledI2C::Width - SSD1306::sc_fontWidth8x16, 0});
                }
                else if (rowCount == (MAX_ROWS - 1) && list_start_index == 0)
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