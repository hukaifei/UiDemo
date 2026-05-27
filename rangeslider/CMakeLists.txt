cmake_minimum_required(VERSION 3.16)
project(RangeSliderTest VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(MSVC)
    add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:/utf-8>")
endif()

find_package(Qt6 COMPONENTS Widgets Core Gui QUIET)
if(NOT Qt6_FOUND)
    find_package(Qt5 5.12 REQUIRED COMPONENTS Widgets Core Gui)
    set(QT_VERSION_MAJOR 5)
else()
    set(QT_VERSION_MAJOR 6)
endif()

if(WIN32 AND MSVC)
    if(QT_VERSION_MAJOR EQUAL 6)
        get_target_property(_qt_qmake Qt6::qmake IMPORTED_LOCATION)
    else()
        get_target_property(_qt_qmake Qt5::qmake IMPORTED_LOCATION)
    endif()
    if(_qt_qmake)
        get_filename_component(QT_BIN_DIR "${_qt_qmake}" DIRECTORY)
        message(STATUS "Qt binary dir: ${QT_BIN_DIR}")
    endif()
endif()

set(SOURCES
    main.cpp
    RangeSlider.cpp
    RangeSlider.h
)

add_executable(${PROJECT_NAME} ${SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

if(QT_VERSION_MAJOR EQUAL 6)
    target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Core Qt6::Gui)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::Core Qt5::Gui)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(WIN32 AND MSVC AND QT_BIN_DIR)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        VS_DEBUGGER_ENVIRONMENT "PATH=${QT_BIN_DIR}$<SEMICOLON>%PATH%"
    )
    message(STATUS "VS debugger PATH: ${QT_BIN_DIR}")
endif()
