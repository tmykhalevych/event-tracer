find_package(GTest REQUIRED)

function(mark_as_gtest TARGET)
  target_link_libraries(${TARGET} GTest::gtest GTest::gtest_main)
  gtest_discover_tests(${TARGET})
  add_test(NAME ${TARGET} COMMAND ${TARGET})
endfunction()

add_subdirectory(infra)
add_subdirectory(core)
