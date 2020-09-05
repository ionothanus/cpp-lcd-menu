#ifndef SRC_MENUENTRY_H
#define SRC_MENUENTRY_H

#include <vector>
#include <memory>
#include <string>

#include "IAction.h"

namespace textmenu
{
    struct MenuEntry;

    using MenuEntryList = std::vector<MenuEntry>;

    struct MenuEntry
    {
        std::string displayValue;
        std::string actionLabel;
        MenuEntryList submenu;
    };
}

#endif //SRC_MENUENTRY_H