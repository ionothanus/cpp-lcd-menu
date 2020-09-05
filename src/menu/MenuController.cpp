
#include <iostream>

#include "MenuController.h"

namespace textmenu
{
    namespace menu
    {
        MenuController::MenuController(
            MenuEntryList menu,
            std::shared_ptr<IMenuView> menuview,
            std::shared_ptr<input::IMenuInputController> input)
            : m_action_map{},
              m_nav_history{},
              m_menuview{menuview},
              m_input{input}
        {
            m_nav_history.push(menu);

            RegisterDefaultActions();
            ConfigureInputs();

            m_menuview->LoadNewMenu(m_nav_history.top());
        }

        bool MenuController::RegisterAction(const std::string& actionLabel, MenuAction action)
        {
            m_action_map[actionLabel] = action;

            return true;
        }

        void MenuController::RegisterDefaultActions()
        {
            RegisterAction("Back", MenuAction{std::bind(&MenuController::BackNavHandler, this), false});
        }

        void MenuController::ConfigureInputs()
        {
            m_input->RegisterSelectButtonHandler([this](bool selected){
                ActionHandler(selected);
            });
            m_input->RegisterRelativeVerticalHandler([&](int rel_offset){
                m_menuview->RequestIndexChange(rel_offset);
            });
        }
        
        void MenuController::ActionHandler(bool selected)
        {
            textmenu::MenuEntry selected_entry{m_nav_history.top()[m_menuview->GetCurrentIndex()]};

            if (selected)
            {
                if (selected_entry.submenu.size() > 0)
                {
                    m_nav_history.push(selected_entry.submenu);
                    m_menuview->LoadNewMenu(m_nav_history.top());
                }
                
                if (selected_entry.actionLabel != "")
                {
                    if (m_action_map.find(selected_entry.actionLabel) != m_action_map.end())
                    {
                        MenuAction this_action{ m_action_map[selected_entry.actionLabel] };
                        bool result{ this_action.m_action() };
                        if (this_action.show_result)
                        {
                            result ? DisplayResult("Success") : DisplayResult("Error");
                        }
                    }
                    else
                    {
                        DisplayResult("Unknown action");
                    }
                }
            }
        }

        bool MenuController::BackNavHandler()
        {
            m_nav_history.pop();
            m_menuview->LoadNewMenu(m_nav_history.top());
            return true;
        }

        void MenuController::DisplayResult(const std::string& message)
        {
            m_menuview->RequestTimedOverlay(message, std::chrono::milliseconds(2000));
        }
    }
}
