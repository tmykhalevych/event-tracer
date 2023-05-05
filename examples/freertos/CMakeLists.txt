set(FREERTOS_CONFIG_FILE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(FREERTOS_HEAP 3) # compiler provided malloc/free
set(FREERTOS_PORT GCC_POSIX)

find_package(FreeRTOS REQUIRED)
find_package(Threads REQUIRED)

add_example(freertos main.cpp)

target_compile_definitions(freertos PUBLIC USE_ALLOCATOR_TRACES)
target_link_libraries(freertos PRIVATE freertos-traces freertos_kernel ${CMAKE_THREAD_LIBS_INIT})
