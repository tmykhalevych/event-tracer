option(DEFINE_TRACER_ASSERT "Use user defined assert hook for Event Tracer")
option(DEFINE_TRACER_ERROR  "Use user defined error hook for Event Tracer")

message("Event Tracer options:")
message("- DEFINE_TRACER_ASSERT ${DEFINE_TRACER_ASSERT}")
message("- DEFINE_TRACER_ERROR  ${DEFINE_TRACER_ERROR}")

add_library(event-tracer-core INTERFACE)
target_include_directories(event-tracer-core INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

if(DEFINE_TRACER_ASSERT)
  target_compile_definitions(event-tracer-core INTERFACE tracerUSE_ASSERT_HOOK)
endif()

if(DEFINE_TRACER_ERROR)
  target_compile_definitions(event-tracer-core INTERFACE tracerUSE_ERROR_HOOK)
endif()
