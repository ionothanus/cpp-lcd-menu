#ifndef SRC_DISPLAY_SSD1306MENURENDERER_H
#define SRC_DISPLAY_SSD1306MENURENDERER_H

#include <memory>

#include "libSSD1306/lib/OledI2C.h"
#include "libSSD1306/lib/OledFont8x16.h"

#include "display/IMenuRenderer.h"

namespace textmenu
{
    namespace display
    {
        /// @brief Supports the Adafruit SSD1306 128x64 OLED display over i2c on Raspberry Pi.
        /// Requires GPIO export for reset pin before running the application.
        class SSD1306MenuRenderer : public IMenuRenderer
        {
        public:
            SSD1306MenuRenderer(const int reset_gpio, const int i2c_bus, const uint8_t address);
            ~SSD1306MenuRenderer() override;

            void DrawMenuList(const MenuEntryList& menu, 
                              int list_start_index,
                              int selected_index,
                              int selected_line_start_index) override;
            void DrawOverlay(const std::string& message) override;
            void Sleep() override;
            void Wake() override;
            int GetMaxRows() override;
            int GetMaxLineLength() override;
            ScreenState GetScreenState() override;
            bool SelectionIsCharacter() override
            {
                return false;
            }

        private:
            std::unique_ptr<SSD1306::OledI2C> m_oled;

            ScreenState m_screen_state;

            static std::unique_ptr<SSD1306::OledI2C> CreateOledI2C(int i2cbus, uint8_t address);

            static constexpr int MAX_ROWS{ SSD1306::OledI2C::Height / SSD1306::sc_fontHeight8x16 };
            static constexpr int MAX_LINE_LENGTH{ SSD1306::OledI2C::Width / SSD1306::sc_fontWidth8x16 };
        };
    }
}

#endif //SRC_DISPLAY_SSD1306MENURENDERER_H