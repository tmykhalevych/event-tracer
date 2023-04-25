cmake_minimum_required(VERSION 3.18)

project(event-tracer LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

include(cmake/example.cmake)

add_subdirectory(src)
add_subdirectory(examples)
