# cpp-lcd-menu

Create and implement menus on LCD character devices, with long-line scrolling, selection, paging, navigation and actions. Manage LCD devices as well as user input devices (buttons, rotary controllers, etc.). Define menus and device parameters in YAML for easy modification.

This project is copyright © 2020 Jonathan Moscardini, published under the MIT License.

## Current device support

Currently, building this project requires all dependencies for all devices. Work is planned to support conditional compilation, so that users can only compile support for the devices they need (and therefore skip the dependencies they don't). Further device support will be welcome after that is implemented.

### Displays

- Adafruit 16x2 I2C LCD + button plates
- Adafruit SSD1306 128x64 OLED LCD (I2C only)

### Input devices

- Adafruit 16x2 I2C LCD + button plates
- Raspberry Pi libevdev rotary controllers

## Device usage

There are four main components to a cpp-lcd-menu:

- `IMenuRenderer` - an output display device (e.g. a 16x2 character LCD)
- `IMenuInputController` - a user input navigation device (e.g. a rotary controller with a select button)
- `IMenuView` - an object to render a `MenuEntryList` on an `IMenuRenderer`
- `MenuController` - an object to combine an `IMenuInputController` with an `IMenuView` to navigate through a menu & take action

Put together, an `IMenuView` has an `IMenuRenderer`, and a `MenuController` has a `MenuEntryList`, an `IMenuInputController`, and an `IMenuView`.

Additionally, an `ITask` interface represents the thread controls for the above objects, where applicable.

Roughly speaking, this is intended to resemble a Model-View-Controller (MVC) layout, with the `MenuEntryList` providing the Model, the `IMenuView` providing the View, and the `MenuController` providing the Controller.

Some hardware requires additional components to manage its resources and share them amongst the above components.

See main.cpp for a complete example implementation demonstrating the use and composition of these components.

## YAML menu usage
_See example_menu.yaml for the available options for defining a YAML menu._

You can load a YAML menu as follows:

```c++
std::string path_to_yaml_file{ argv[1] };

textmenu::menu::MenuDefinition menu_definition{ textmenu::menu::LoadYamlMenu(path_to_yaml_file) };
```

`textmenu::menu::MenuDefinition` is an `std::pair`, with the first member being the device configuration and the second member being the menu definition.

### Creating a menu

With a YAML menu loaded, you can create the menu list as follows:

```c++
    textmenu::menu::MenuEntry menu_list{ menu_definition.first };
```

### Creating display objects

With a YAML menu loaded, for devices with the appropriate factory function, you can also create the appropriate device directly from those parameters:

```c++
std::unique_ptr<IMenuRenderer> renderer{ SSD1306MenuRenderer::ConstructFromConfigMap(menu_definition.second.m_display_settings) };
```

## Library dependencies

- libyaml-cpp
- libevdev (for evdev rotary controller component)
- libi2c (for Adafruit 16x2 LCD + button plate component)

