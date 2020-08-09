cmake_minimum_required(VERSION 3.0.0)
project(text-menu VERSION 0.1.0)

include(CTest)
enable_testing()

add_subdirectory(thirdparty/libSSD1306)
add_executable(text-menu src/main.cpp)

target_include_directories(text-menu PUBLIC thirdparty)
target_link_libraries(text-menu PUBLIC evdev)
target_link_libraries(text-menu PUBLIC SSD1306)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
