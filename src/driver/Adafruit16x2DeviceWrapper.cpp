// Copyright (c) 2020 Jonathan Moscardini
// 
// Licensed under the MIT License
// http://www.opensource.org/licenses/MIT

#include <sstream>

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

        std::unique_ptr<Adafruit16x2DeviceWrapper> Adafruit16x2DeviceWrapper::ConstructFromConfigMap(const menu::SettingsMap& config)
        {
            const std::string I2C_BUS_KEY{"i2c_bus"};
            const std::string I2C_ADDRESS_KEY{"i2c_address"};

            int i2c_bus{};
            uint8_t i2c_address{};

            try
            {
                i2c_bus = std::stoi(config.at(I2C_BUS_KEY));
                // extra parameters automatically convert from the appropriate base
                i2c_address = std::stoi(config.at(I2C_ADDRESS_KEY), 0, 0);
            }
            catch (std::invalid_argument& invalid_argument)
            {
                std::stringstream sstream{};
                sstream << "Unable to convert parameter from YAML file for Adafruit 16x2 wrapper construction: "
                        << invalid_argument.what();
                throw new std::runtime_error{ sstream.str() };
            }
            catch (std::out_of_range& out_of_range)
            {
                std::stringstream sstream{};
                sstream << "Mandatory parameter for Adafruit 16x2 wrapper missing from YAML configuration: " << out_of_range.what();
                throw new std::runtime_error{ sstream.str() };
            }

            return std::make_unique<Adafruit16x2DeviceWrapper>(i2c_bus, i2c_address);
        }

        ThreadSafe<CharLCD>& Adafruit16x2DeviceWrapper::GetLcdHandle()
        {
            return m_lcd_safe;
        }
    }
}
