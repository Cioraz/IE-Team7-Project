cmake_minimum_required(VERSION 3.10)

project(shell)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set (
    SOURCES
    src/main.cc
    src/shell.cc
    src/shell.h
)

add_executable(shell ${SOURCES})
