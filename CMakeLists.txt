cmake_minimum_required(VERSION 3.5)

project(ra2ob)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ra2ob ra2ob.cpp)

target_include_directories(ra2ob PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
