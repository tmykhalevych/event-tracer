add_library(freertos-event-tracer STATIC event_tracer.cpp)
target_include_directories(freertos-event-tracer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(freertos-event-tracer PRIVATE event-tracer-core)
