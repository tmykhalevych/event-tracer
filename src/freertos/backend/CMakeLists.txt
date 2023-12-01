add_library(event-tracer-backend STATIC event_tracer.cpp traces.cpp)
target_include_directories(event-tracer-backend PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(event-tracer-backend PUBLIC event-tracer-core freertos_kernel)
