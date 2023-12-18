cmake_minimum_required(VERSION 3.16)

project(event-tracer LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

add_subdirectory(examples)
add_subdirectory(test)
add_subdirectory(src)
