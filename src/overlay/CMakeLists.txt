cmake_minimum_required(VERSION 3.16)

project(Overlay LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)

qt_standard_project_setup()

add_library(Overlay SHARED
    overlay.h
    overlay.cpp
)

target_link_libraries(Overlay PRIVATE Qt6::Core Qt6::Widgets Qt6::Gui)

target_compile_definitions(Overlay PRIVATE OVERLAY_LIBRARY)
