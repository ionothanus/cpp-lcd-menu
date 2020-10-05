#ifndef MENU_MENULOADER_HPP
#define MENU_MENULOADER_HPP

#include <unordered_map>

#include "menu/MenuTypes.h"

namespace textmenu
{
    namespace menu
    {
        /// Load a menu structure from a YAML file, including
        /// the menu tree and the device configurations.
        /// @param path: the path to the YAML file
        /// @return a MenuDefinition container
        MenuDefinition LoadYamlMenu(const std::string& path);
    }
}

#endif //MENU_MENULOADER_HPP