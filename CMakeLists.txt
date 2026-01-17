cmake_minimum_required(VERSION 3.16)

project(V8 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set compiler flags for consistent ABI
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -static-libgcc -static-libstdc++")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

# Ensure consistent linking
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-libgcc -static-libstdc++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Multimedia MultimediaWidgets)

qt_standard_project_setup()

# Force static linking of Qt to avoid ABI issues
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")

# Add subdirectories for modules
add_subdirectory(src/speech)
add_subdirectory(src/overlay)
add_subdirectory(src/ai)
add_subdirectory(src/ocr)
add_subdirectory(src/camera)
add_subdirectory(src/ui)

# Main executable
qt_add_executable(V8
    src/main.cpp
    src/ui/mainwindow/mainwindow.h
    src/ui/mainwindow/mainwindow.cpp
)

target_link_libraries(V8 PRIVATE Qt6::Core Qt6::Widgets Qt6::Multimedia Qt6::MultimediaWidgets Speech Overlay AI OCR Camera AnimationLib)

target_include_directories(V8 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/speech
    ${CMAKE_CURRENT_SOURCE_DIR}/src/overlay
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ocr
    ${CMAKE_CURRENT_SOURCE_DIR}/src/camera
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/mainwindow
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/vosk/include
)

# Packaging
include(CPack)
set(CPACK_PROJECT_NAME "V8")
set(CPACK_PACKAGE_NAME "V8")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "V8 Application")
set(CPACK_PACKAGE_VENDOR "Your Company")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "V8")
set(CPACK_NSIS_DISPLAY_NAME "V8")
set(CPACK_NSIS_PACKAGE_NAME "V8")
