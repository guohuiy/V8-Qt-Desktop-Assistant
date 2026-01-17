cmake_minimum_required(VERSION 3.16)

project(Camera LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Multimedia MultimediaWidgets)

qt_standard_project_setup()

add_library(Camera SHARED
    camera.h
    camera.cpp
)

target_link_libraries(Camera PRIVATE Qt6::Core Qt6::Multimedia Qt6::MultimediaWidgets)

target_compile_definitions(Camera PRIVATE CAMERA_LIBRARY)
