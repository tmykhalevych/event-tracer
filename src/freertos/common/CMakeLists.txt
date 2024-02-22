set(ET_TASK_ID_TYPE              "default" CACHE STRING "FreeRTOS task id type")
set(ET_TASK_PRIO_TYPE            "default" CACHE STRING "FreeRTOS task priority type")
set(ET_TIMESTAMP_RESOLUTION_BITS "default" CACHE STRING "Event timestamp resolution (in bits)")
set(ET_MAX_MESSAGE_LEN           "default" CACHE STRING "Event message max length")

message(STATUS "FreeRTOS Event Tracer configuration:")
message(STATUS "  ET_FREERTOS_TARGET:           ${ET_FREERTOS_TARGET}")
message(STATUS "  ET_TASK_ID_TYPE:              ${ET_TASK_ID_TYPE}")
message(STATUS "  ET_TASK_PRIO_TYPE:            ${ET_TASK_PRIO_TYPE}")
message(STATUS "  ET_TIMESTAMP_RESOLUTION_BITS: ${ET_TIMESTAMP_RESOLUTION_BITS}")
message(STATUS "  ET_MAX_MESSAGE_LEN:           ${ET_MAX_MESSAGE_LEN}")

set(ET_COMPILE_DEFINITIONS "")

if(NOT ${ET_TASK_ID_TYPE} STREQUAL "default")
  list(APPEND ET_COMPILE_DEFINITIONS tracerTASK_ID_TYPE=${ET_TASK_ID_TYPE})
endif()

if(NOT ${ET_TASK_PRIO_TYPE} STREQUAL "default")
  list(APPEND ET_COMPILE_DEFINITIONS tracerTASK_PRIO_TYPE=${ET_TASK_PRIO_TYPE})
endif()

if(NOT ${ET_TIMESTAMP_RESOLUTION_BITS} STREQUAL "default")
  list(APPEND ET_COMPILE_DEFINITIONS tracerTIMESTAMP_RESOLUTION_BITS=${ET_TIMESTAMP_RESOLUTION_BITS})
endif()

if(NOT ${ET_MAX_MESSAGE_LEN} STREQUAL "default")
  list(APPEND ET_COMPILE_DEFINITIONS tracerMAX_EVENT_MESSAGE_LEN=${ET_MAX_MESSAGE_LEN})
endif()

add_library(freertos-tracer-common INTERFACE)
target_compile_definitions(freertos-tracer-common INTERFACE ${ET_COMPILE_DEFINITIONS})
target_include_directories(freertos-tracer-common INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(freertos-tracer-common INTERFACE event-tracer-core ${ET_FREERTOS_TARGET})
