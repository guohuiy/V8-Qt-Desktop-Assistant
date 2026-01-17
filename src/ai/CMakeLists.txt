cmake_minimum_required(VERSION 3.16)

project(AI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network)

qt_standard_project_setup()

add_library(AI SHARED
    ai.h
    ai.cpp
)

target_link_libraries(AI PRIVATE Qt6::Core Qt6::Network)

target_compile_definitions(AI PRIVATE AI_LIBRARY)
