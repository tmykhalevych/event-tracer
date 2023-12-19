set(TRACER_ASSERT_DEFINITION "" CACHE STRING "Use user defined assert hook for Event Tracer")
set(TRACER_ERROR_DEFINITION "" CACHE STRING "Use user defined error hook for Event Tracer")

message("Event Tracer options:")
message("- TRACER_ASSERT_DEFINITION: \"${TRACER_ASSERT_DEFINITION}\"")
message("- TRACER_ERROR_DEFINITION: \"${TRACER_ERROR_DEFINITION}\"")

add_library(event-tracer-core INTERFACE)
target_include_directories(event-tracer-core INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

if(NOT ${TRACER_ASSERT_DEFINITION} STREQUAL "")
  target_compile_definitions(event-tracer-core INTERFACE tracerUSE_ASSERT_HOOK)
  target_link_libraries(event-tracer-core INTERFACE ${TRACER_ASSERT_DEFINITION})
endif()

if(NOT ${TRACER_ERROR_DEFINITION} STREQUAL "")
  target_compile_definitions(event-tracer-core INTERFACE tracerUSE_ERROR_HOOK)
  target_link_libraries(event-tracer-core INTERFACE ${TRACER_ERROR_DEFINITION})
endif()
