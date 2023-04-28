set(FREERTOS_CONFIG_FILE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(FREERTOS_PORT "GCC_POSIX")

find_package(FreeRTOS REQUIRED)

add_example(freertos main.cpp)
target_link_libraries(freertos PRIVATE event-registry freertos_kernel)
