add_library(freertos-tracer-common INTERFACE)
target_include_directories(freertos-tracer-common INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(freertos-tracer-common INTERFACE event-tracer-core)
