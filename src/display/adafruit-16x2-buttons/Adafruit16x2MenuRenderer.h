#ifndef SRC_DISPLAY_ADAFRUIT16X2MENURENDERER_H
#define SRC_DISPLAY_ADAFRUIT16X2MENURENDERER_H

#include "CharLCD/src/CharLCD.h"

#include "display/IMenuRenderer.h"

namespace textmenu
{
    namespace display
    {
        class Adafruit16x2MenuRenderer : public IMenuRenderer
        {
        public:
            Adafruit16x2MenuRenderer();
            ~Adafruit16x2MenuRenderer();

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
            CharLCD m_lcd;

            ScreenState m_screen_state;

            static constexpr int MAX_ROWS{ 2 };
            static constexpr int MAX_LINE_LENGTH{ 16 };
        };
    }
}

#endif //SRC_DISPLAY_ADAFRUIT16X2MENURENDERER_H