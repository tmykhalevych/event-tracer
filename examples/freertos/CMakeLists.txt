set(FREERTOS_CONFIG_FILE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(FREERTOS_HEAP 3) # compiler provided malloc/free
set(FREERTOS_PORT GCC_POSIX)

find_package(FreeRTOS REQUIRED)
find_package(Threads REQUIRED)

target_link_libraries(freertos_kernel freertos-traces)
target_compile_definitions(freertos_kernel PUBLIC USE_ALLOCATOR_TRACES)

add_example(freertos main.cpp)
target_link_libraries(freertos PRIVATE freertos_kernel ${CMAKE_THREAD_LIBS_INIT})
