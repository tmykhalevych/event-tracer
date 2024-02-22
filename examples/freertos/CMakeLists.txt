list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(FREERTOS_CONFIG_FILE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "")
set(FREERTOS_HEAP "3" CACHE STRING "") # rely on compiler provided malloc/free
set(FREERTOS_PORT "GCC_POSIX" CACHE STRING "")

set(ET_TASK_TRACES ON CACHE BOOL "")

set(ET_FREERTOS_TARGET "freertos_kernel" CACHE STRING "")
set(ET_TASK_ID_TYPE "uint16_t" CACHE STRING "")
set(ET_TASK_PRIO_TYPE "uint8_t" CACHE STRING "")
set(ET_TIMESTAMP_RESOLUTION_BITS "40" CACHE STRING "")
set(ET_MAX_MESSAGE_LEN "12" CACHE STRING "")

find_package(FreeRTOS REQUIRED)
find_package(Threads REQUIRED)

target_link_libraries(${ET_FREERTOS_TARGET} freertos-tracer-config)

add_example(freertos main.cpp)
target_link_libraries(freertos PRIVATE ${ET_FREERTOS_TARGET} freertos-tracer-client ${CMAKE_THREAD_LIBS_INIT})
