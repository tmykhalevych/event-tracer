add_executable(core-test
    assert_test.cpp
    error_test.cpp
    event_registry_test.cpp)

target_link_libraries(core-test test-infra event-tracer-core)

mark_as_gtest(core-test)
