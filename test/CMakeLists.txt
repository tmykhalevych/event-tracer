find_package(GTest REQUIRED)

function(add_gtest TARGET SOURCES)
  add_executable(${TARGET} ${SOURCES})
  target_link_libraries(${TARGET} GTest::gtest GTest::gtest_main)
  gtest_discover_tests(${TARGET})
  add_test(NAME ${TARGET} COMMAND ${TARGET})
endfunction()

add_subdirectory(core)
