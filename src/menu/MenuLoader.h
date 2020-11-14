// Copyright (c) 2020 Jonathan Moscardini
// 
// Licensed under the MIT License
// http://www.opensource.org/licenses/MIT

#ifndef MENU_MENULOADER_HPP
#define MENU_MENULOADER_HPP

#include <unordered_map>

#include "menu/MenuTypes.h"

namespace textmenu
{
    namespace menu
    {
        /// @brief Load a menu structure from a YAML file, including
        ///        the menu tree and the device configurations.
        /// @param path: the path to the YAML file
        /// @return a MenuDefinition container
        MenuDefinition LoadYamlMenu(const std::string& path);
    }
}

#endif //MENU_MENULOADER_HPP