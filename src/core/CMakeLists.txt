set(ET_ASSERT_DEFINITION "default" CACHE STRING "Use user defined target with assert hook for Event Tracer")
set(ET_ERROR_DEFINITION  "default" CACHE STRING "Use user defined target with error hook for Event Tracer")

message(STATUS "Event Tracer external definitions:")
message(STATUS "  ET_ASSERT_DEFINITION: ${ET_ASSERT_DEFINITION}")
message(STATUS "  ET_ERROR_DEFINITION:  ${ET_ERROR_DEFINITION}")

add_library(event-tracer-core INTERFACE)
target_include_directories(event-tracer-core INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

if(NOT ${ET_ASSERT_DEFINITION} STREQUAL "default")
  target_compile_definitions(event-tracer-core INTERFACE tracerUSE_ASSERT_HOOK)
  target_link_libraries(event-tracer-core INTERFACE ${ET_ASSERT_DEFINITION})
endif()

if(NOT ${ET_ERROR_DEFINITION} STREQUAL "default")
  target_compile_definitions(event-tracer-core INTERFACE tracerUSE_ERROR_HOOK)
  target_link_libraries(event-tracer-core INTERFACE ${ET_ERROR_DEFINITION})
endif()
