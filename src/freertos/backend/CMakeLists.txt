add_library(freertos-tracer-backend STATIC event_tracer.cpp traces.cpp)
target_include_directories(freertos-tracer-backend PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(freertos-tracer-backend PUBLIC freertos-tracer-common ${FREERTOS_TARGET})
