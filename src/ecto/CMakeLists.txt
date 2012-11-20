cmake_minimum_required(VERSION 2.8)
project(lucid_ecto)

find_package(ecto)

find_package(OpenCV REQUIRED)
message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "OpenCV headers: ${OpenCV_INCLUDE_DIRS}")
include_directories(SYSTEM ../cpp/include
                           ${OpenCV_INCLUDE_DIRS}
)


if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
endif()


find_package(Boost COMPONENTS serialization filesystem system REQUIRED)

add_subdirectory(src)
