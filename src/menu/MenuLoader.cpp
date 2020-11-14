// Copyright (c) 2020 Jonathan Moscardini
// 
// Licensed under the MIT License
// http://www.opensource.org/licenses/MIT

#include "menu/MenuLoader.h"

#include <yaml-cpp/yaml.h>


namespace textmenu
{
    namespace menu
    {
        /// Recursively build the MenuEntry tree
        /// @param node: the current node of the YAML menu definition
        /// @return the assembled MenuEntry tree for the current level of recursion
        MenuEntry GetEntries(const YAML::Node& node)
        {
            MenuEntryList this_menu{};

            YAML::Node these_entries{node["entries"]};

            for (auto this_menu_it = these_entries.begin();
                 this_menu_it != these_entries.end();
                 this_menu_it++)
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

        /// Extract configuration parameters from the YAML file
        /// @param node: the top level YAML node
        /// @return: all device entries from the YAML file, in maps of strings
        MenuConfiguration GetConfiguration(const YAML::Node& yaml_top_level)
        {
            MenuConfiguration configuration{};

            for (auto it = yaml_top_level["devices"]["display"].begin(); 
                 it != yaml_top_level["devices"]["display"].end();
                 it++)
            {
                 configuration.m_display_settings
                              .emplace(it->first.as<std::string>(),
                                       it->second.as<std::string>());

            }

            for (auto it = yaml_top_level["devices"]["controller"].begin(); 
                 it != yaml_top_level["devices"]["controller"].end();
                 it++)
            {
                configuration.m_controller_settings
                                .emplace(it->first.as<std::string>(),
                                         it->second.as<std::string>());
            }

            return configuration;
        }

        MenuDefinition LoadYamlMenu(const std::string& path)
        {
            YAML::Node menu{ YAML::LoadFile(path) };

            MenuEntryList top_level{};

            for (auto top_level_it = menu["menu"].begin();
                 top_level_it != menu["menu"].end();
                 top_level_it++)
            {
                MenuEntry this_menu{GetEntries((*top_level_it)["submenu"])};

                top_level.emplace_back(this_menu);
            }

            MenuEntry head{ "Main menu", "", top_level };

            MenuConfiguration configuration_settings{ GetConfiguration(menu) };

            return MenuDefinition{head, configuration_settings};
        }
    }
}