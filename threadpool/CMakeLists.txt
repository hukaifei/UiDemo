cmake_minimum_required(VERSION 3.16)

find_package(Qt6 COMPONENTS Core Widgets QUIET)
if(NOT Qt6_FOUND)
    find_package(Qt5 5.12 REQUIRED COMPONENTS Core Widgets)
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

add_executable(threadpool_demo
    main.cpp
    mainwindow.cpp
    mainwindow.h
    complextask.cpp
    complextask.h
)

if(QT_VERSION_MAJOR EQUAL 6)
    target_link_libraries(threadpool_demo PRIVATE Qt6::Core Qt6::Widgets)
else()
    target_link_libraries(threadpool_demo PRIVATE Qt5::Core Qt5::Widgets)
endif()

if(WIN32 AND MSVC AND QT_BIN_DIR)
    set_target_properties(threadpool_demo PROPERTIES
        VS_DEBUGGER_ENVIRONMENT "PATH=${QT_BIN_DIR}$<SEMICOLON>%PATH%"
    )
endif()
