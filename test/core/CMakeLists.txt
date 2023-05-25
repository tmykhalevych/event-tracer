add_executable(core-test
    assert_test.cpp
    error_test.cpp
    event_registry_test.cpp)

target_link_libraries(core-test test-infra event-tracer-core)
target_include_directories(core-test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

mark_as_gtest(core-test)
