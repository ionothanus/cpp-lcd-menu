cmake_minimum_required(VERSION 3.1.0)
project(text-menu VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)
enable_testing()

add_subdirectory(thirdparty/libSSD1306 EXCLUDE_FROM_ALL)
add_executable(text-menu src/main.cpp)

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/libSSD1306)

add_library(menuview STATIC src/BasicTask.cpp
                            src/menuview/TextMenuView.cpp)

add_library(displaySSD1306 STATIC src/display/ssd1306/SSD1306MenuRenderer.cpp)


target_include_directories(text-menu PUBLIC .)
target_include_directories(text-menu PUBLIC ./src)
target_include_directories(text-menu PUBLIC thirdparty)
target_link_libraries(text-menu PUBLIC evdev)
target_link_libraries(displaySSD1306 PUBLIC SSD1306)
target_link_libraries(text-menu PUBLIC displaySSD1306)
target_link_libraries(text-menu PUBLIC menuview)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(text-menu PRIVATE Threads::Threads)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
