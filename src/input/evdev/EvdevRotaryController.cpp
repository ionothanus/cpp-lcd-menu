
#include <unistd.h>
#include <fcntl.h>
#include <exception>
#include <sstream>

#include "input/evdev/EvdevRotaryController.h"

namespace textmenu
{
    namespace input
    {
        EvdevRotaryController::EvdevRotaryController(const std::string& rotary_path, const std::string& button_path, int polling_interval)
            : m_polling_interval{polling_interval},
              m_fd_rotary{ -1 },
              m_fd_button{ -1 }
        {
            int rc = 1;
    
            m_fd_rotary = open(rotary_path.c_str(), O_RDONLY|O_NONBLOCK); //"/dev/input/event1"

            rc = libevdev_new_from_fd(m_fd_rotary, &m_dev_rotary);
            
            if (rc < 0)
            {
                throw std::runtime_error{"Couldn't open libevdev for rotary input"};
            }

            m_fd_button = open(button_path.c_str(), O_RDONLY|O_NONBLOCK); //"/dev/input/event0"
            rc = libevdev_new_from_fd(m_fd_button, &m_dev_button);

            if (rc < 0) 
            {
                throw std::runtime_error{"Couldn't open libevdev for button input"};
            }
        }

        EvdevRotaryController::~EvdevRotaryController()
        {
            if (m_dev_rotary != nullptr)
            {
                libevdev_free(m_dev_rotary);
            }

            if (m_dev_button != nullptr)
            {
                libevdev_free(m_dev_button);
            }

            if (m_fd_rotary > -1)
            {
                close(m_fd_rotary);
            }

            if (m_fd_button > -1)
            {
                close(m_fd_button);
            }
        }

        std::unique_ptr<EvdevRotaryController> EvdevRotaryController::ConstructFromConfigMap(const menu::SettingsMap& config)
        {
            //const std::string& rotary_path, const std::string& button_path, int polling_interval)
            const std::string ROTARY_PATH_KEY{"rotary_device_path"};
            const std::string BUTTON_PATH_KEY{"button_device_path"};
            const std::string OPTIONAL_POLLING_INTERVAL_KEY{"polling_interval_ms"};

            std::string rotary_path{};
            std::string button_path{};

            try
            {
                rotary_path = config.at(ROTARY_PATH_KEY);
                button_path = config.at(BUTTON_PATH_KEY);
            }
            catch (std::out_of_range& out_of_range)
            {
                std::stringstream sstream{};
                sstream << "Mandatory parameter for EvdevRotaryController missing from YAML configuration: " << out_of_range.what();
                throw new std::runtime_error{ sstream.str() };
            }

            auto polling_interval_iter{ config.find(OPTIONAL_POLLING_INTERVAL_KEY) };

            if (polling_interval_iter != config.end())
            {
                try
                {
                    int polling_interval{ std::stoi(polling_interval_iter->second, 0, 0) };

                    return std::make_unique<EvdevRotaryController>(rotary_path, button_path, polling_interval);
                }
                catch (std::invalid_argument& invalid_argument)
                {
                    return std::make_unique<EvdevRotaryController>(rotary_path, button_path);
                }
            }

            return std::make_unique<EvdevRotaryController>(rotary_path, button_path);
        }

        void EvdevRotaryController::RegisterSelectButtonHandler(PushButtonHandler handler)
        {
            m_button_handler = handler;
        }

        void EvdevRotaryController::RegisterRelativeVerticalHandler(RelativeVerticalHandler handler)
        {
            m_rotation_handler = handler;
        }

        void EvdevRotaryController::ThreadFunction()
        {
            while (!m_terminate)
            {
                input_event ev_rotary{}, ev_button{};
                int ev_rotary_rc, ev_button_rc;

                {
                    std::unique_lock<std::mutex> lock{m_cv_mutex};
                    m_thread_cv.wait_for(lock,
                                        std::chrono::milliseconds(m_polling_interval),
                                        [&] () 
                                        { 
                                            ev_rotary_rc = libevdev_next_event(m_dev_rotary, LIBEVDEV_READ_FLAG_NORMAL, &ev_rotary);
                                            ev_button_rc = libevdev_next_event(m_dev_button, LIBEVDEV_READ_FLAG_NORMAL, &ev_button);

                                            return ((m_terminate == true) ||
                                                    (ev_rotary_rc == 0) ||
                                                    (ev_button_rc == 0)); 
                                        });            
                }

                if (ev_rotary_rc == 0)
                {
                    if (ev_rotary.type == EV_REL)
                    {
                        m_rotation_handler(ev_rotary.value);
                    }
                }

                if (ev_button_rc == 0)
                {
                    std::string event_code_name{ libevdev_event_code_get_name(ev_button.type, ev_button.code) };

                    if (event_code_name == "KEY_ENTER")
                    {
                        m_button_handler(ev_button.value);
                    }
                }
            }
        }
    }
}