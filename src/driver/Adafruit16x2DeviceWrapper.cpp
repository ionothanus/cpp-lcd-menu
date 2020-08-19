
#include "driver/Adafruit16x2DeviceWrapper.h"

namespace textmenu
{
    namespace driver
    {
        Adafruit16x2DeviceWrapper::Adafruit16x2DeviceWrapper(int i2c_bus, int i2c_address)
            : m_lcd_safe{1, 0x20}
        {
            m_lcd_safe->start(MAX_LINE_LENGTH, MAX_ROWS);
        }

        ThreadSafe<CharLCD>& Adafruit16x2DeviceWrapper::GetLcdHandle()
        {
            return m_lcd_safe;
        }
    }
}
