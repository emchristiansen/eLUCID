cmake_minimum_required(VERSION 2.8)
project(lucid)

find_package(ecto QUIET)
if (${ecto_FOUND})
  add_subdirectory(ecto)
endif()
add_subdirectory(cpp)
