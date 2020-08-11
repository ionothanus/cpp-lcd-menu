#ifndef SRC_DISPLAY_SSD1306MENURENDERER_H
#define SRC_DISPLAY_SSD1306MENURENDERER_H

#include "libSSD1306/lib/OledI2C.h"
#include "libSSD1306/lib/OledFont8x16.h"

#include "display/IMenuRenderer.h"

namespace textmenu
{
    namespace display
    {
        class SSD1306MenuRenderer : public IMenuRenderer
        {
        public:
            SSD1306MenuRenderer();
            ~SSD1306MenuRenderer();

            void DrawMenuList(const MenuList& menu, 
                              int list_start_index,
                              int selected_index,
                              int selected_line_start_index) override;
            void Sleep() override;
            void Wake() override;
            int GetMaxRows() override;
            int GetMaxLineLength() override;
            ScreenState GetScreenState() override;

        private:
            SSD1306::OledI2C m_oled;

            ScreenState m_screen_state;

            static constexpr int MAX_ROWS{ SSD1306::OledI2C::Height / SSD1306::sc_fontHeight8x16 };
            static constexpr int MAX_LINE_LENGTH{ SSD1306::OledI2C::Width / SSD1306::sc_fontWidth8x16 };
        };
    }
}

#endif //SRC_DISPLAY_SSD1306MENURENDERER_H