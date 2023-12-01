add_library(event-tracer-client STATIC traces.cpp)
target_include_directories(event-tracer-client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(event-tracer-client PRIVATE event-tracer-backend)
