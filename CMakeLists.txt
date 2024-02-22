cmake_minimum_required(VERSION 3.16)

project(event-tracer LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

option(ET_INCLUDE_EXAMPLES "Include examples to the build" OFF)
option(ET_INCLUDE_TESTS    "Include tests to the build"    OFF)

message(STATUS "Event Tracer auxiliary targets:")
message(STATUS "  ET_INCLUDE_EXAMPLES: ${ET_INCLUDE_EXAMPLES}")
message(STATUS "  ET_INCLUDE_TESTS:    ${ET_INCLUDE_TESTS}")

if(ET_INCLUDE_EXAMPLES)
    add_subdirectory(examples)
endif()

if(ET_INCLUDE_TESTS)
    add_subdirectory(test)
endif()

add_subdirectory(src)
