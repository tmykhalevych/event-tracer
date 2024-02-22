if(NOT ET_FREERTOS_TARGET)
  message(FATAL_ERROR "ET_FREERTOS_TARGET is not set")
endif()

add_subdirectory(common)
add_subdirectory(backend)
add_subdirectory(client)
add_subdirectory(config)
