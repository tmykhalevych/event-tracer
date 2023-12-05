add_library(event-tracer-common INTERFACE)
target_include_directories(event-tracer-common INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(event-tracer-common INTERFACE event-tracer-core)
