cmake_minimum_required(VERSION 3.16)

project(OCR LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

add_library(OCR SHARED
    ocr.cpp
    ocr.h
)

target_link_libraries(OCR PRIVATE Qt6::Core Qt6::Widgets)

# Define for export
target_compile_definitions(OCR PRIVATE OCR_LIBRARY)
