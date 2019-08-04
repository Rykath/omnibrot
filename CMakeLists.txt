cmake_minimum_required(VERSION 3.14)
project(omnibrot)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-O3 -Wall")

include_directories(.)
add_subdirectory(deepixel)
add_subdirectory(recipes)
add_subdirectory(common)