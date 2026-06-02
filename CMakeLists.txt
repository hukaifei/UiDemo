cmake_minimum_required(VERSION 3.16)
project(UITest VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(MSVC)
    add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:/utf-8>")
endif()

add_subdirectory(customwidget)
add_subdirectory(modelView)
add_subdirectory(rangeslider)
add_subdirectory(wizard)
# add_subdirectory(webengine)
