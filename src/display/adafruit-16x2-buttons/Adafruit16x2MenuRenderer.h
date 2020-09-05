#ifndef SRC_DISPLAY_ADAFRUIT16X2MENURENDERER_H
#define SRC_DISPLAY_ADAFRUIT16X2MENURENDERER_H

#include <memory>

#include "display/IMenuRenderer.h"
#include "driver/Adafruit16x2DeviceWrapper.h"

namespace textmenu
{
    namespace display
    {
        class Adafruit16x2MenuRenderer : public IMenuRenderer
        {
        public:
            Adafruit16x2MenuRenderer(std::shared_ptr<driver::Adafruit16x2DeviceWrapper>& lcd);
            ~Adafruit16x2MenuRenderer() override;

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
                return true;
            }

        private:
            std::shared_ptr<driver::Adafruit16x2DeviceWrapper> m_lcd;

            ScreenState m_screen_state;

            static constexpr size_t MAX_LINE_LENGTH{ driver::Adafruit16x2DeviceWrapper::MAX_LINE_LENGTH };
            static constexpr size_t MAX_ROWS{ driver::Adafruit16x2DeviceWrapper::MAX_ROWS };
        };
    }
}

#endif //SRC_DISPLAY_ADAFRUIT16X2MENURENDERER_H