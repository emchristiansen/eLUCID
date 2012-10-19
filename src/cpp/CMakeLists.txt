cmake_minimum_required(VERSION 2.8)
project(lucid)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(Octave REQUIRED)
include_directories(${OCTAVE_INCLUDE_DIRS})

add_definitions("-msse4.2")

set(BIN_OUTPUT_PATH bin)
set(LIBRARY_OUTPUT_PATH lib)

add_subdirectory(src)
