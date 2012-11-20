cmake_minimum_required(VERSION 2.8)
project(lucid_cpp)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# deal with Octave
find_package(Octave QUIET)
include_directories(SYSTEM ${OCTAVE_INCLUDE_DIRS})

# add compile instructions
add_definitions("-msse4.2")
include_directories(include)

set(BIN_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(src)
