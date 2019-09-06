cmake_minimum_required(VERSION 2.6)

project(cpplib)

set(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "project dir")

set(CMAKE_CXX_STANDARD 17)

if (UNIX)
    set (CMAKE_CXX_FLAGS "-pthread")
endif (UNIX)

add_subdirectory(example)