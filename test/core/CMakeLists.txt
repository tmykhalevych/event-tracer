file(GLOB tests *_test.cpp)
add_executable(core-test ${tests})
target_link_libraries(core-test test-infra event-tracer-core)
mark_as_gtest(core-test)
