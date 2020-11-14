// Copyright (c) 2020 Jonathan Moscardini
// 
// Licensed under the MIT License
// http://www.opensource.org/licenses/MIT

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
        ///        Requires GPIO export for reset pin before running the application.
        class SSD1306MenuRenderer : public IMenuRenderer
        {
        public:
            static std::unique_ptr<SSD1306MenuRenderer> ConstructFromConfigMap(const menu::SettingsMap& config);

            /// @brief Construct a new SSD1306MenuRenderer object.
            /// 
            /// @param reset_gpio The GPIO (GPIO number, not pin number) connected to the
            ///                   display's reset line.
            /// @param i2c_bus The I2C bus the display is connected to
            /// @param address The I2C device address
            SSD1306MenuRenderer(const int reset_gpio, const int i2c_bus, const uint8_t address);
            ~SSD1306MenuRenderer() override;

            // IMenuRenderer override functions
            // These functions are described in IMenuRenderer.h.
            void DrawMenuList(const menu::MenuEntryList& menu, 
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
            /// @brief The internal SSD1306 object used to communicate
            ///        with the display hardware.
            std::unique_ptr<SSD1306::OledI2C> m_oled;

            /// @brief The state of this display
            ScreenState m_screen_state;

            /// @brief Create a OledI2C object
            /// 
            /// @param i2cbus the I2C bus number
            /// @param address the device address
            /// @return std::unique_ptr<SSD1306::OledI2C> 
            static std::unique_ptr<SSD1306::OledI2C> CreateOledI2C(int i2cbus, uint8_t address);

            /// @brief The maximum number of rows displayable on this display
            static constexpr int MAX_ROWS{ SSD1306::OledI2C::Height / SSD1306::sc_fontHeight8x16 };

            /// @brief The maximum number of characters displayable on this display
            static constexpr int MAX_LINE_LENGTH{ SSD1306::OledI2C::Width / SSD1306::sc_fontWidth8x16 };
        };
    }
}

#endif //SRC_DISPLAY_SSD1306MENURENDERER_H