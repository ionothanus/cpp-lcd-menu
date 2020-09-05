#include <algorithm>
#include <thread>
#include <fstream>
#include <sstream>

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

        void ResetDisplayGPIO(int gpio)
        {
            std::ostringstream gpio_file_sstream{};
            gpio_file_sstream << "/sys/class/gpio/gpio" << gpio << "/value";

            std::ostringstream gpio_direction_sstream{};
            gpio_direction_sstream << "/sys/class/gpio/gpio" << gpio << "/direction";

            {
                std::string direction_path{ gpio_direction_sstream.str() };
                std::ofstream gpio_direction{direction_path};

                if (!gpio_direction)
                {
                    throw std::runtime_error("Couldn't open file to set GPIO direction");
                }

                gpio_direction << "out";
            }

            {
                std::string file_path{ gpio_file_sstream.str() };
                std::ofstream gpio_file{ file_path};

                if (!gpio_file)
                {
                    throw std::runtime_error("Couldn't open file to set GPIO value");
                }

                gpio_file << "0";
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                gpio_file << "1";
            }

        }

        std::unique_ptr<SSD1306::OledI2C> SSD1306MenuRenderer::CreateOledI2C(int i2c_bus, uint8_t address)
        {
            std::ostringstream i2c_dev_path{};

            i2c_dev_path << "/dev/i2c-" << i2c_bus;
            return std::make_unique<SSD1306::OledI2C>(i2c_dev_path.str(), address);
        }

        SSD1306MenuRenderer::SSD1306MenuRenderer(const int reset_gpio, const int i2c_bus, const uint8_t address)
            : m_screen_state{ScreenState::On}
        {
            ResetDisplayGPIO(reset_gpio);
            m_oled = std::move(CreateOledI2C(i2c_bus, address));
            m_oled->clear();
            m_oled->displayUpdate();
        }

        SSD1306MenuRenderer::~SSD1306MenuRenderer()
        {
            m_oled->displayOff();
            m_oled->clear();
            m_oled->displayUpdate();
        }

        /// Public API

        void SSD1306MenuRenderer::Sleep()
        {
            m_oled->displayOff();
            m_screen_state = ScreenState::Off;
        }

        void SSD1306MenuRenderer::Wake()
        {
            m_oled->displayOn();
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

        void SSD1306MenuRenderer::DrawOverlay(const std::string& message)
        {
            SSD1306::OledBitmap<SSD1306::OledI2C::Width, SSD1306::sc_fontHeight8x16> menu_selected{};
            SSD1306::OledBitmap<SSD1306::OledI2C::Width, SSD1306::OledI2C::Height> buffer{};
            menu_selected.fill();
            buffer.clear();
            buffer.setFrom(menu_selected, SSD1306::OledPoint{0, 0});
            SSD1306::drawString8x16(SSD1306::OledPoint{0, 0},
                                    message,
                                    SSD1306::PixelStyle::Unset,
                                    buffer);
            
            m_oled->setFrom(buffer);

            m_oled->displayUpdate();
        }

        void SSD1306MenuRenderer::DrawMenuList(const MenuEntryList& menu, int list_start_index, int selected_index, int selected_line_start_index)
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
                    // draw up arrow
                    buffer.setFrom(UP_ARROW, SSD1306::OledPoint{SSD1306::OledI2C::Width - SSD1306::sc_fontWidth8x16, 0});
                }
                else if (rowCount == (MAX_ROWS - 1) && (menu.size() - list_start_index - MAX_ROWS) > 0)
                {
                    // draw down arrow
                    buffer.setFrom(DOWN_ARROW, SSD1306::OledPoint{SSD1306::OledI2C::Width - SSD1306::sc_fontWidth8x16, 
                                                                  rowCount * SSD1306::sc_fontHeight8x16});
                    break;
                }

                rowCount++;
            }

            m_oled->setFrom(buffer);

            m_oled->displayUpdate();
        }
    }
}