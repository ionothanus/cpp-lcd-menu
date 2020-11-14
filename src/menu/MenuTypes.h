// Copyright (c) 2020 Jonathan Moscardini
// 
// Licensed under the MIT License
// http://www.opensource.org/licenses/MIT

#ifndef MENU_MENUTYPES_HPP
#define MENU_MENUTYPES_HPP

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "IAction.h"

namespace textmenu
{
    namespace menu
    {
        struct MenuEntry;

        using MenuEntryList = std::vector<MenuEntry>;

        /// @brief A menu entry, including submenu definition.
        struct MenuEntry
        {
            /// @brief The text to be displayed for this entry.
            std::string displayValue;
            /// @brief The map key to be used to register an action for this label.
            std::string actionLabel;
            /// @brief The submenu to navigate to if this entry is selected, if applicable.
            MenuEntryList submenu;
        };

        using SettingsMap = std::unordered_map<std::string, std::string>;

        /// @brief A description of device configurations loaded from a menu definition.
        struct MenuConfiguration
        {
            SettingsMap m_display_settings;
            SettingsMap m_controller_settings;
        };

        using MenuDefinition = std::pair<MenuEntry, MenuConfiguration>;
    }
}
#endif //MENU_MENUTYPES_HPP
