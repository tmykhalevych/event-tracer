add_subdirectory(event_tracer)

option(TASK_TRACES   "Use task traces"             OFF)
option(TASK_PERF     "Use task profiling utility"  OFF)
option(QUEUE_TRACES  "Use queue traces"            OFF)
option(SYNCH_TRACES  "Use synchronisation traces"  OFF)
option(SBUFF_TRACES  "Use stream buffer traces"    OFF)
option(TIMER_TRACES  "Use timer traces"            OFF)
option(POWER_TRACES  "Use power management traces" OFF)
option(ALLOC_TRACES  "Use allocation traces"       OFF)
option(EGROUP_TRACES "Use event group traces"      OFF)

message("FreeRTOS Tracer options:")
message("- TASK_TRACES   ${TASK_TRACES}")
message("- TASK_PERF     ${TASK_PERF}")
message("- QUEUE_TRACES  ${QUEUE_TRACES}")
message("- SYNCH_TRACES  ${SYNCH_TRACES}")
message("- SBUFF_TRACES  ${SBUFF_TRACES}")
message("- TIMER_TRACES  ${TIMER_TRACES}")
message("- POWER_TRACES  ${POWER_TRACES}")
message("- ALLOC_TRACES  ${ALLOC_TRACES}")
message("- EGROUP_TRACES ${EGROUP_TRACES}")

set(FREERTOS_TRACES_COMPILE_DEFINITIONS "")
set(FREERTOS_TRACES_SOURCES traces.cpp)

if(TASK_TRACES)
  list(APPEND FREERTOS_TRACES_COMPILE_DEFINITIONS tracerUSE_TASK_TRACES)
  list(APPEND FREERTOS_TRACES_SOURCES task_traces.cpp)
endif()

if(TASK_PERF)
  list(APPEND FREERTOS_TRACES_COMPILE_DEFINITIONS tracerUSE_TASK_PERF)
  list(APPEND FREERTOS_TRACES_SOURCES task_perf.cpp)
endif()

if(QUEUE_TRACES)
  list(APPEND FREERTOS_TRACES_COMPILE_DEFINITIONS tracerUSE_QUEUE_TRACES)
endif()

if(SYNCH_TRACES)
  list(APPEND FREERTOS_TRACES_COMPILE_DEFINITIONS tracerUSE_SYNCH_TRACES)
endif()

if(SBUFF_TRACES)
  list(APPEND FREERTOS_TRACES_COMPILE_DEFINITIONS tracerUSE_SBUFF_TRACES)
endif()

if(TIMER_TRACES)
  list(APPEND FREERTOS_TRACES_COMPILE_DEFINITIONS tracerUSE_TIMER_TRACES)
endif()

if(POWER_TRACES)
  list(APPEND FREERTOS_TRACES_COMPILE_DEFINITIONS tracerUSE_POWER_TRACES)
endif()

if(ALLOC_TRACES)
  list(APPEND FREERTOS_TRACES_COMPILE_DEFINITIONS tracerUSE_ALLOC_TRACES)
  list(APPEND FREERTOS_TRACES_SOURCES alloc_traces.cpp)
endif()

if(EGROUP_TRACES)
  list(APPEND FREERTOS_TRACES_COMPILE_DEFINITIONS tracerUSE_EGROUP_TRACES)
endif()

add_library(freertos-traces STATIC ${FREERTOS_TRACES_SOURCES})
target_include_directories(freertos-traces PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(freertos-traces INTERFACE ${FREERTOS_TRACES_COMPILE_DEFINITIONS})
target_link_libraries(freertos-traces PRIVATE freertos-event-tracer)
