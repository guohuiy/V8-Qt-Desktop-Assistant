cmake_minimum_required(VERSION 3.16)

project(V8 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Multimedia MultimediaWidgets)

qt_standard_project_setup()

add_subdirectory(speech)
add_subdirectory(overlay)
add_subdirectory(ai)
add_subdirectory(ocr)
add_subdirectory(camera)
add_subdirectory(ui)

qt_add_executable(V8
    main.cpp
    ui/mainwindow.h
    ui/mainwindow.cpp
)

target_link_libraries(V8 PRIVATE Qt6::Core Qt6::Widgets Qt6::Multimedia Qt6::MultimediaWidgets Speech Overlay AI OCR Camera AnimationLib)

target_include_directories(V8 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/speech ${CMAKE_CURRENT_SOURCE_DIR}/overlay ${CMAKE_CURRENT_SOURCE_DIR}/ai ${CMAKE_CURRENT_SOURCE_DIR}/ocr ${CMAKE_CURRENT_SOURCE_DIR}/camera ${CMAKE_CURRENT_SOURCE_DIR}/ui ${CMAKE_CURRENT_SOURCE_DIR}/libs/vosk/include)

# Copy vosk dll to build directory
add_custom_command(TARGET V8 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/vosk/lib/libvosk.dll
        $<TARGET_FILE_DIR:V8>
)
