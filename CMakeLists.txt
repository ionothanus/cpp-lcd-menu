cmake_minimum_required(VERSION 3.1.0)
project(text-menu VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)
enable_testing()

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/libSSD1306/lib)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/CharLCD/src)

#third-party libs
add_subdirectory(thirdparty/libSSD1306)# EXCLUDE_FROM_ALL)
add_library(adafruit-16x2 STATIC thirdparty/CharLCD/src/CharLCD.cpp
                                 thirdparty/CharLCD/src/MCP23017.cpp)

add_executable(text-menu src/main.cpp)

#testing
#add_executable(i2ctest thirdparty/CharLCD/example/lcd.cpp)
#target_link_libraries(i2ctest PUBLIC adafruit-16x2)
#target_link_libraries(i2ctest PUBLIC i2c)

add_library(text-menu-shared STATIC src/BasicTask.cpp)

add_library(menuview STATIC src/menuview/TextMenuView.cpp)

add_library(displaySSD1306 STATIC src/display/ssd1306/SSD1306MenuRenderer.cpp)
add_library(displayAdafruit16x2 STATIC src/display/adafruit-16x2-buttons/Adafruit16x2MenuRenderer.cpp
                                       src/input/adafruit-16x2-buttons/Adafruit16x2ButtonController.cpp
                                       src/driver/Adafruit16x2DeviceWrapper.cpp)
add_library(inputEvdevRotaryController STATIC src/input/evdev/EvdevRotaryController.cpp)
add_library(menu STATIC src/menu/MenuLoader.cpp
                        src/menu/MenuController.cpp)

target_include_directories(text-menu PUBLIC .)
target_include_directories(text-menu PUBLIC ./src)
target_include_directories(text-menu PUBLIC thirdparty)
target_link_libraries(text-menu PUBLIC i2c)
target_link_libraries(menuview PUBLIC text-menu-shared)
target_link_libraries(inputEvdevRotaryController PUBLIC evdev)
target_link_libraries(inputEvdevRotaryController PUBLIC text-menu-shared)
target_link_libraries(displaySSD1306 PUBLIC SSD1306)
target_link_libraries(displayAdafruit16x2 PUBLIC adafruit-16x2)
target_link_libraries(menu PUBLIC yaml-cpp)
target_link_libraries(text-menu PUBLIC displaySSD1306)
target_link_libraries(text-menu PUBLIC displayAdafruit16x2)
target_link_libraries(text-menu PUBLIC inputEvdevRotaryController)
target_link_libraries(text-menu PUBLIC menu)
target_link_libraries(text-menu PUBLIC menuview)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(text-menu PRIVATE Threads::Threads)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
