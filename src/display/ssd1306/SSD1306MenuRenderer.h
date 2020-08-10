#ifndef SRC_DISPLAY_SSD1306MENURENDERER_H
#define SRC_DISPLAY_SSD1306MENURENDERER_H

#include <thread>
#include <condition_variable>
#include <mutex>
#include <atomic>

#include "libSSD1306/lib/OledI2C.h"
#include "libSSD1306/lib/OledFont8x16.h"

#include "IMenuRenderer.h"

namespace textmenu
{
    namespace display
    {
        class SSD1306MenuRenderer : IMenuRenderer
        {
        public:
            SSD1306MenuRenderer();
            ~SSD1306MenuRenderer();

            void UpdateEntries(const MenuList& entries) override;
            void UpdateIndex(int rel_offset) override;
            int GetCurrentIndex() override;
            void Sleep() override;
            void Wake() override;

            void Run();
            void Stop();

        private:

            enum class ScrollDirection
            {
                None,
                Forward,
                Reverse,
                Hold
            };

            enum class ScreenState
            {
                On,
                Off
            };

            void RenderThread();
            void RedrawMenu();
            bool ScrollSelectedLine();

            std::thread m_thread;
            std::mutex m_cv_mutex;
            std::condition_variable m_thread_cv;
            MenuList m_local_list;
            SSD1306::OledI2C m_oled;
            std::atomic<bool> m_terminate;
            std::atomic<bool> m_update_required;

            std::atomic<int> m_selected_index;
            int m_display_start_index;
            int m_selected_line_start_index;
            int m_scroll_hold_counter;
            bool m_full_scroll_completed;
            ScrollDirection m_scroll_direction;
            int m_sleep_counter;
            ScreenState m_screen_state;

            static constexpr int MAX_ENTRIES{ SSD1306::OledI2C::Height / SSD1306::sc_fontHeight8x16 };
            static constexpr int MAX_LINE_LENGTH{ SSD1306::OledI2C::Width / SSD1306::sc_fontWidth8x16 };
            static constexpr int MIN_LIST_INDEX{ 0 };
            static constexpr int SCROLL_DELAY_MS{ 250 };
            static constexpr int MAX_SCROLL_HOLD{ 4 };
            static constexpr int MAX_SLEEP_COUNT{ 4 };
        };
    }
}

#endif //SRC_DISPLAY_SSD1306MENURENDERER_H