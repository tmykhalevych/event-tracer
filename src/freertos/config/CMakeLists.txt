option(TASK_TRACES   "Use task traces"             OFF)
option(TASK_PERF     "Use task profiling utility"  OFF)
option(ALLOC_TRACES  "Use allocation traces"       OFF)

message("FreeRTOS Tracer options:")
message("- TASK_TRACES   ${TASK_TRACES}")
message("- TASK_PERF     ${TASK_PERF}")
message("- ALLOC_TRACES  ${ALLOC_TRACES}")

set(FREERTOS_TRACES_COMPILE_DEFINITIONS "")

if(TASK_TRACES)
  list(APPEND FREERTOS_TRACES_COMPILE_DEFINITIONS tracerUSE_TASK_TRACES)
endif()

if(TASK_PERF)
  list(APPEND FREERTOS_TRACES_COMPILE_DEFINITIONS tracerUSE_TASK_PERF)
endif()

if(ALLOC_TRACES)
  list(APPEND FREERTOS_TRACES_COMPILE_DEFINITIONS tracerUSE_ALLOC_TRACES)
endif()

add_library(freertos-tracer-config INTERFACE)
target_compile_definitions(freertos-tracer-config INTERFACE ${FREERTOS_TRACES_COMPILE_DEFINITIONS})
target_include_directories(freertos-tracer-config INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
