// Copyright (c) 2020 Jonathan Moscardini
// 
// Licensed under the MIT License
// http://www.opensource.org/licenses/MIT

#ifndef ADAFRUIT16X2DEVICEWRAPPER_H
#define ADAFRUIT16X2DEVICEWRAPPER_H

#include <memory>

#include "CharLCD/src/CharLCD.h"

#include "menu/MenuTypes.h"
#include "ThreadSafe.hpp"

namespace textmenu
{
    namespace driver
    {
        /// @brief Wraps an Adafruit 16x2 LCD display + buttons
        ///        so that the button inputs & the LCD output
        ///        can be handled by different objects. Thread-safe.
        class Adafruit16x2DeviceWrapper
        {
        public:
            /// @brief Construct a new Adafruit16x2DeviceWrapper object
            /// 
            /// @param i2c_bus The I2C bus on which to find the device.
            /// @param i2c_address The I2C address of the device.
            Adafruit16x2DeviceWrapper(int i2c_bus, int i2c_address);

            /// @brief Construct a new Adafruit16x2DeviceWrapper based on a configuration map
            ///        (e.g. from a YAML menu.)
            /// 
            /// @param config The SettingsMap containing the appropriate device settings.
            /// @return std::unique_ptr<Adafruit16x2DeviceWrapper> 
            static std::unique_ptr<Adafruit16x2DeviceWrapper> ConstructFromConfigMap(const menu::SettingsMap& config);

            /// @brief Get the LcdHandle object to access the device.
            /// 
            /// @return ThreadSafe<CharLCD>& 
            ThreadSafe<CharLCD>& GetLcdHandle();

            /// @brief The maximum number of rows that can be displayed by this device.
            static constexpr int MAX_ROWS{ 2 };
            /// @brief The maximum number of characters in a row on this device.
            static constexpr int MAX_LINE_LENGTH{ 16 };
        private:
            ThreadSafe<CharLCD> m_lcd_safe;
        };
    }
}

#endif //ADAFRUIT16X2DEVICEWRAPPER_H