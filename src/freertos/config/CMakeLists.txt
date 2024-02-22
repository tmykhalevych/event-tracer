option(ET_TASK_TRACES   "Enable task traces"             OFF)
option(ET_TASK_PROF     "Enable task profiling utility"  OFF)
option(ET_ALLOC_TRACES  "Enable allocation traces"       OFF)

message(STATUS "FreeRTOS Event Tracer features:")
message(STATUS "  ET_TASK_TRACES:   ${ET_TASK_TRACES}")
message(STATUS "  ET_TASK_PROF:     ${ET_TASK_PROF}")
message(STATUS "  ET_ALLOC_TRACES:  ${ET_ALLOC_TRACES}")

set(FREERTOS_TRACES_COMPILE_DEFINITIONS "")

if(ET_TASK_TRACES)
  list(APPEND FREERTOS_TRACES_COMPILE_DEFINITIONS tracerUSE_TASK_TRACES)
endif()

if(ET_TASK_PROF)
  list(APPEND FREERTOS_TRACES_COMPILE_DEFINITIONS tracerUSE_TASK_PROF)
endif()

if(ET_ALLOC_TRACES)
  list(APPEND FREERTOS_TRACES_COMPILE_DEFINITIONS tracerUSE_ALLOC_TRACES)
endif()

add_library(freertos-tracer-config INTERFACE)
target_compile_definitions(freertos-tracer-config INTERFACE ${FREERTOS_TRACES_COMPILE_DEFINITIONS})
target_include_directories(freertos-tracer-config INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
