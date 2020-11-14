// Copyright (c) 2020 Jonathan Moscardini
// 
// Licensed under the MIT License
// http://www.opensource.org/licenses/MIT

#ifndef MENU_MENUCONTROLLER_H
#define MENU_MENUCONTROLLER_H

#include <functional>
#include <memory>
#include <stack>
#include <unordered_map>

#include "menu/MenuTypes.h"
#include "menuview/IMenuView.h"
#include "input/IMenuInputController.h"

namespace textmenu
{
    namespace menu
    {
        struct MenuAction
        {
            std::function<bool (void)> m_action;
            bool show_result;
        };

        /// @brief Connects an input controller to a menu, 
        ///        handling user input for menu navigation,
        ///        accepting registration of actions for specific menu entries,
        ///        and executing those actions on user activation of the menu entry.
        class MenuController
        {
        public:
            /// @brief Construct a new MenuController object
            /// 
            /// @param menu The menu list to manage.
            /// @param menuview The IMenuView source of selected menu entries and pages.
            /// @param input The user input device, used to trigger events.
            MenuController(MenuEntryList menu,
                           std::shared_ptr<IMenuView> menuview,
                           std::shared_ptr<input::IMenuInputController> input);

            /// @brief Registers an action to be executed when a user activates
            ///        a selected menu entry.
            /// 
            /// @param actionLabel The name of the menu entry.
            /// @param action The action to execute.
            /// @return true if the action is successfully registered.
            /// @return false if the action was not successfully registered.
            bool RegisterAction(const std::string& actionLabel, MenuAction action);

        private:
            void RegisterDefaultActions();
            void ConfigureInputs();
            void ActionHandler(bool selected);
            bool BackNavHandler();
            void DisplayResult(const std::string& result);

            std::unordered_map<std::string, MenuAction> m_action_map;
            std::stack<menu::MenuEntryList> m_nav_history;

            std::shared_ptr<IMenuView> m_menuview;
            std::shared_ptr<input::IMenuInputController> m_input;
        };
    }
}

#endif //MENU_MENUCONTROLLER_H
