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

        struct MenuEntry
        {
            std::string displayValue;
            std::string actionLabel;
            MenuEntryList submenu;
        };

        using SettingsMap = std::unordered_map<std::string, std::string>;

        struct MenuConfiguration
        {
            SettingsMap m_display_settings;
            SettingsMap m_controller_settings;
        };

        using MenuDefinition = std::pair<MenuEntry, MenuConfiguration>;
    }
}
#endif //MENU_MENUTYPES_HPP
