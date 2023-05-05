add_library(freertos-traces SHARED traces.cpp)
target_include_directories(freertos-traces PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
