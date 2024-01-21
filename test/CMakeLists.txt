enable_testing()
find_package(GTest REQUIRED)

function(mark_as_gtest TARGET)
  target_link_libraries(${TARGET} GTest::gtest GTest::gtest_main)
  gtest_discover_tests(${TARGET})
  add_test(NAME ${TARGET} COMMAND ${TARGET} WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endfunction()

add_subdirectory(infra)
add_subdirectory(core)
