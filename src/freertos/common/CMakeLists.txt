set(FREERTOS_TRACER_TYPES "")

if(TASK_ID_TYPE)
  list(APPEND FREERTOS_TRACER_TYPES tracerTASK_ID_TYPE=${TASK_ID_TYPE})
endif()

if(TASK_PRIO_TYPE)
  list(APPEND FREERTOS_TRACER_TYPES tracerTASK_PRIO_TYPE=${TASK_PRIO_TYPE})
endif()

if(TIMESTAMP_RESOLUTION_BITS)
  list(APPEND FREERTOS_TRACER_TYPES tracerTIMESTAMP_RESOLUTION_BITS=${TIMESTAMP_RESOLUTION_BITS})
endif()

if(MAX_EVENT_MESSAGE_LEN)
  list(APPEND FREERTOS_TRACER_TYPES tracerMAX_EVENT_MESSAGE_LEN=${MAX_EVENT_MESSAGE_LEN})
endif()

add_library(freertos-tracer-common INTERFACE)
target_compile_definitions(freertos-tracer-common INTERFACE ${FREERTOS_TRACER_TYPES})
target_include_directories(freertos-tracer-common INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(freertos-tracer-common INTERFACE event-tracer-core ${FREERTOS_TARGET})
