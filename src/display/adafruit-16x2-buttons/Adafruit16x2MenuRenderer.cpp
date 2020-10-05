#include <algorithm>

#include "Adafruit16x2MenuRenderer.h"

namespace textmenu
{
    namespace display
    {
        const uint8_t DOWN_ARROW_INDEX{ 1 };
        const uint8_t UP_ARROW_INDEX{ 2 };

        //should be const but library requires non-const
        uint8_t DOWN_ARROW[] {
            0b00000000,
            0b00000100,
            0b00000100,
            0b00000100,
            0b00011111,
            0b00001110,
            0b00000100,
            0b00000000
        };

        uint8_t UP_ARROW[] {
            0b00000000,
            0b00000100,
            0b00001110,
            0b00011111,
            0b00000100,
            0b00000100,
            0b00000100,
            0b00000000
        };

        Adafruit16x2MenuRenderer::Adafruit16x2MenuRenderer(std::shared_ptr<driver::Adafruit16x2DeviceWrapper>& lcd)
            : m_lcd{lcd},
              m_screen_state{ScreenState::On}
        {
            m_lcd->GetLcdHandle()->createChar(DOWN_ARROW_INDEX, DOWN_ARROW);
            m_lcd->GetLcdHandle()->createChar(UP_ARROW_INDEX, UP_ARROW);
            m_lcd->GetLcdHandle()->noBlink();
            m_lcd->GetLcdHandle()->noCursor();
        }

        Adafruit16x2MenuRenderer::~Adafruit16x2MenuRenderer()
        {
            Sleep();
        }

        /// Public API

        void Adafruit16x2MenuRenderer::Sleep()
        {
            m_lcd->GetLcdHandle()->noDisplay();
            m_lcd->GetLcdHandle()->setBacklight(0);
            m_screen_state = ScreenState::Off;
        }

        void Adafruit16x2MenuRenderer::Wake()
        {
            m_lcd->GetLcdHandle()->display();
            m_lcd->GetLcdHandle()->setBacklight(255);
            m_screen_state = ScreenState::On;
        }

        int Adafruit16x2MenuRenderer::GetMaxRows()
        {
            return MAX_ROWS;
        }

        int Adafruit16x2MenuRenderer::GetMaxLineLength()
        {
            return MAX_LINE_LENGTH;
        }

        ScreenState Adafruit16x2MenuRenderer::GetScreenState()
        {
            return m_screen_state;
        }

        void Adafruit16x2MenuRenderer::DrawOverlay(const std::string& message)
        {
            m_lcd->GetLcdHandle()->home();

            std::string blank_line = std::string(MAX_LINE_LENGTH, ' ');

            std::string line{blank_line};
            size_t size_to_render{ std::clamp(message.size(), 0U, MAX_LINE_LENGTH) };
            std::string string_to_render{ line.replace(0, size_to_render, message) };

            m_lcd->GetLcdHandle()->setCursor(0, 0);

            m_lcd->GetLcdHandle()->print(string_to_render.substr(0, MAX_LINE_LENGTH));
        }

        void Adafruit16x2MenuRenderer::DrawMenuList(const menu::MenuEntryList& menu, int list_start_index, int selected_index, int selected_line_start_index)
        {
            int rowCount{ 0 };

            m_lcd->GetLcdHandle()->home();

            std::string blank_line = std::string(MAX_LINE_LENGTH, ' ');

            for (int i = list_start_index; i < menu.size(); i++)
            {
                std::string line{blank_line};
                menu::MenuEntry entry = menu[i];
                size_t size_to_render{ std::clamp(entry.displayValue.size(), 0U, MAX_LINE_LENGTH) };
                std::string string_to_render{ line.replace(0, size_to_render, entry.displayValue) };

                //scrolling
                if (i == selected_index)
                {
                    //buffer.setFrom(menu_selected, SSD1306::OledPoint{0, rowCount * SSD1306::sc_fontHeight8x16});
                    string_to_render = "*" + string_to_render.substr(selected_line_start_index, string_to_render.length() - selected_line_start_index);
                }

                if (rowCount == 0 && list_start_index > 0)
                {
                    //draw up arrow
                    string_to_render[MAX_LINE_LENGTH - 1] = UP_ARROW_INDEX;
                }
                else if (rowCount == (MAX_ROWS - 1) && (menu.size() - list_start_index - MAX_ROWS) > 0)
                {
                    //draw down arrow
                    string_to_render[MAX_LINE_LENGTH - 1] = DOWN_ARROW_INDEX;
                }
                
                m_lcd->GetLcdHandle()->setCursor(0, rowCount);

                m_lcd->GetLcdHandle()->print(string_to_render.substr(0, MAX_LINE_LENGTH));

                rowCount++;

                if (rowCount >= MAX_ROWS)
                {
                    break;
                }
            }

            while (rowCount < MAX_ROWS)
            {
                m_lcd->GetLcdHandle()->print(blank_line);
                rowCount++;
            }
        }
    }
}