cmake_minimum_required(VERSION 3.16)

project(event-tracer LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

if(BUILD_EVENT_TRACER_AUXILIARY_TARGETS)
    add_subdirectory(examples)
    add_subdirectory(test)
endif()

add_subdirectory(src)
