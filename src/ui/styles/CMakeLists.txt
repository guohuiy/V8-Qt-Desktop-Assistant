cmake_minimum_required(VERSION 3.16)

project(AnimationLib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

add_library(AnimationLib SHARED
    animationlib.h
    animationlib.cpp
)

target_link_libraries(AnimationLib PRIVATE Qt6::Core Qt6::Widgets)

target_compile_definitions(AnimationLib PRIVATE ANIMATIONLIB_LIBRARY)
