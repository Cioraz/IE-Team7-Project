cmake_minimum_required(VERSION 3.16)
project(gui_shell LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

add_executable(shell_ulti
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    shell.cc
    shell.h
)

target_link_libraries(shell_ulti PRIVATE Qt6::Widgets)