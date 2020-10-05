
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

        class MenuController
        {
        public:
            MenuController(MenuEntryList menu,
                           std::shared_ptr<IMenuView> menuview,
                           std::shared_ptr<input::IMenuInputController> input);

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
