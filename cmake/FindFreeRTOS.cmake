if (NOT FREERTOS_PATH)
  message(STATUS "FREERTOS_PATH not specified, fetching FreeRTOS from github.com/FreeRTOS")

  include(FetchContent)
  FetchContent_Declare(FreeRTOS
    GIT_REPOSITORY https://github.com/FreeRTOS/FreeRTOS.git
    GIT_TAG 202212.01
    GIT_PROGRESS TRUE)

  # check if populated already, just to do it once
  FetchContent_GetProperties(FreeRTOS)
  if (NOT FreeRTOS_POPULATED)
    set(FETCHCONTENT_QUIET FALSE)
    FetchContent_Populate(FreeRTOS)
  endif()

  set(FREERTOS_PATH ${freertos_SOURCE_DIR})
  message(STATUS "FREERTOS_PATH set to '${FREERTOS_PATH}'")
endif()

add_subdirectory(${FREERTOS_PATH}/FreeRTOS/Source FreeRTOS)
