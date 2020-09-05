
#include "menu/MenuLoader.h"

#include <yaml-cpp/yaml.h>

namespace textmenu
{
    namespace menu
    {
        MenuEntry GetEntries(const YAML::Node& node)
        {
            MenuEntryList this_menu{};

            YAML::Node these_entries{node["entries"]};

            for (auto this_menu_it = these_entries.begin(); this_menu_it != these_entries.end(); this_menu_it++)
            {
                if ((*this_menu_it)["submenu"])
                {
                    this_menu.emplace_back(GetEntries((*this_menu_it)["submenu"]));
                }
                else
                {
                    MenuEntry this_entry{ (*this_menu_it)["display_text"].as<std::string>(),
                        (*this_menu_it)["action"].as<std::string>(), MenuEntryList{} };
                    this_menu.emplace_back(this_entry);
                }
            }

            //always add a "Back" entry for nested menus
            this_menu.emplace_back(MenuEntry{"Back", "Back", MenuEntryList{}});

            return MenuEntry{node["display_text"].as<std::string>(), "", this_menu};
        }

        MenuEntry LoadYamlMenu(const std::string& path)
        {
            YAML::Node menu{ YAML::LoadFile(path) };

            MenuEntryList top_level{};

            for (auto top_level_it = menu["menu"].begin(); top_level_it != menu["menu"].end(); top_level_it++)
            {
                MenuEntry this_menu{GetEntries((*top_level_it)["submenu"])};

                top_level.emplace_back(this_menu);
            }

            return MenuEntry{"Main menu", "", top_level};
        }
    }
}