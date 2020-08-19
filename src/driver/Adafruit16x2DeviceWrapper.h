#ifndef ADAFRUIT16X2DEVICEWRAPPER_H
#define ADAFRUIT16X2DEVICEWRAPPER_H

#include <memory>

#include "CharLCD/src/CharLCD.h"

#include "ThreadSafe.hpp"

namespace textmenu
{
    namespace driver
    {
        class Adafruit16x2DeviceWrapper
        {
        public:
            Adafruit16x2DeviceWrapper(int i2c_bus, int i2c_address);

            ThreadSafe<CharLCD>& GetLcdHandle();

            static constexpr int MAX_ROWS{ 2 };
            static constexpr int MAX_LINE_LENGTH{ 16 };
        private:
            ThreadSafe<CharLCD> m_lcd_safe;
        };
    }
}

#endif //ADAFRUIT16X2DEVICEWRAPPER_H