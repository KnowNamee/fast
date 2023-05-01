cmake_minimum_required(VERSION 3.25)
project(fast LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

include(cmake/Config.cmake)
include(cmake/Dependencies.cmake)
include(cmake/Utils.cmake)

add_subdirectory(fast)
add_subdirectory(playground)
