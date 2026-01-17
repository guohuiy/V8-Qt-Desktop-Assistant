cmake_minimum_required(VERSION 3.16)

project(Speech LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network Multimedia)

qt_standard_project_setup()

add_library(Speech SHARED
    speech.h
    speech.cpp
    miclib.h
    miclib.cpp
)

target_include_directories(Speech PRIVATE ${CMAKE_SOURCE_DIR}/libs/vosk/include)

target_link_libraries(Speech PRIVATE Qt6::Core Qt6::Network Qt6::Multimedia ${CMAKE_SOURCE_DIR}/libs/vosk/lib/libvosk.dll)

target_compile_definitions(Speech PRIVATE SPEECH_LIBRARY)
