cmake_minimum_required(VERSION 3.16)

find_package(Qt6 COMPONENTS Core Widgets Network QUIET)
if(NOT Qt6_FOUND)
    find_package(Qt5 5.12 REQUIRED COMPONENTS Core Widgets Network)
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

# HTTP文件服务端
add_executable(HttpFileServer
    HttpFileServer.cpp
    HttpFileServer.h
    main_server.cpp
)

if(QT_VERSION_MAJOR EQUAL 6)
    target_link_libraries(HttpFileServer PRIVATE Qt6::Core Qt6::Widgets Qt6::Network)
else()
    target_link_libraries(HttpFileServer PRIVATE Qt5::Core Qt5::Widgets Qt5::Network)
endif()

# HTTP文件客户端
add_executable(HttpFileClient
    HttpFileClient.cpp
    HttpFileClient.h
    main_client.cpp
)

if(QT_VERSION_MAJOR EQUAL 6)
    target_link_libraries(HttpFileClient PRIVATE Qt6::Core Qt6::Widgets Qt6::Network)
else()
    target_link_libraries(HttpFileClient PRIVATE Qt5::Core Qt5::Widgets Qt5::Network)
endif()

if(WIN32 AND MSVC AND QT_BIN_DIR)
    set_target_properties(HttpFileServer PROPERTIES
        VS_DEBUGGER_ENVIRONMENT "PATH=${QT_BIN_DIR}$<SEMICOLON>%PATH%"
    )
    set_target_properties(HttpFileClient PROPERTIES
        VS_DEBUGGER_ENVIRONMENT "PATH=${QT_BIN_DIR}$<SEMICOLON>%PATH%"
    )
endif()
