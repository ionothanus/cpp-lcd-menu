// Copyright (c) 2020 Jonathan Moscardini
// 
// Licensed under the MIT License
// http://www.opensource.org/licenses/MIT

#ifndef SRC_DISPLAY_ADAFRUIT16X2MENURENDERER_H
#define SRC_DISPLAY_ADAFRUIT16X2MENURENDERER_H

#include <memory>

#include "display/IMenuRenderer.h"
#include "driver/Adafruit16x2DeviceWrapper.h"

namespace textmenu
{
    namespace display
    {
        /// @brief Supports the Adafruit 16x2 I2C LCD display.
        class Adafruit16x2MenuRenderer : public IMenuRenderer
        {
        public:
            /// @brief Construct a new Adafruit16x2MenuRenderer.
            /// 
            /// @param lcd The Adafruit16x2DeviceWrapper which provides access to
            ///            the underlying hardware.
            Adafruit16x2MenuRenderer(std::shared_ptr<driver::Adafruit16x2DeviceWrapper>& lcd);
            ~Adafruit16x2MenuRenderer() override;

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
                return true;
            }

        private:
            /// @brief The LCD hardware interface.
            std::shared_ptr<driver::Adafruit16x2DeviceWrapper> m_lcd;

            /// @brief The state of this screen (on or off).
            ScreenState m_screen_state;

            /// @brief The maximum number of rows displayable on this screen.
            static constexpr size_t MAX_LINE_LENGTH{ driver::Adafruit16x2DeviceWrapper::MAX_LINE_LENGTH };
            /// @brief The maximum number of characters displayable in a row.
            static constexpr size_t MAX_ROWS{ driver::Adafruit16x2DeviceWrapper::MAX_ROWS };
        };
    }
}

#endif //SRC_DISPLAY_ADAFRUIT16X2MENURENDERER_H