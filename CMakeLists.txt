cmake_minimum_required(VERSION 3.5)

project(ra2ob)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(ra2ob main.cpp ra2ob.cpp)
