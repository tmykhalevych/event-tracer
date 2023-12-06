add_library(freertos-tracer-client STATIC event_tracer_client.cpp)
target_include_directories(freertos-tracer-client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(freertos-tracer-client PRIVATE freertos-tracer-backend PUBLIC freertos-tracer-common)
