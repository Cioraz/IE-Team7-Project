cmake_minimum_required(VERSION 3.10)

project(SmartShellTeam7)

# Setting and ensuring C++ standard used is C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Defines list of source and header files used in project
set (
    SOURCES
    src/main.cc   
    src/shell.cc  
    src/shell.h
)

# Defines executable from sources
add_executable(myShell ${SOURCES})
