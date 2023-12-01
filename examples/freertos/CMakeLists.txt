set(FREERTOS_CONFIG_FILE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "")
set(FREERTOS_HEAP "3" CACHE STRING "") # rely on compiler provided malloc/free
set(FREERTOS_PORT "GCC_POSIX" CACHE STRING "")

set(TASK_TRACES ON CACHE BOOL "")

find_package(FreeRTOS REQUIRED)
find_package(Threads REQUIRED)

target_link_libraries(freertos_kernel event-tracer-config)

add_example(freertos main.cpp)
target_link_libraries(freertos PRIVATE freertos_kernel event-tracer-client ${CMAKE_THREAD_LIBS_INIT})
