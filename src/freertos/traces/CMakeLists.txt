add_library(freertos-traces STATIC traces.cpp alloc_traces.cpp)
target_include_directories(freertos-traces PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(freertos-traces PRIVATE freertos-event-tracer)
