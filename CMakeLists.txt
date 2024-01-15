cmake_minimum_required(VERSION 3.5)

project(ra2ob)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(ra2ob Ra2ob/example.cpp)

set_target_properties(ra2ob PROPERTIES LINK_FLAGS "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\" ")
