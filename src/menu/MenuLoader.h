#ifndef MENU_MENULOADER_HPP
#define MENU_MENULOADER_HPP

#include "MenuEntry.h"

namespace textmenu
{
    namespace menu
    {
        MenuEntry LoadYamlMenu(const std::string& path);
    }
}

#endif //MENU_MENULOADER_HPP