file(GLOB SRC "*.cpp")

add_gtest(core-test ${SRC})
target_link_libraries(core-test event-tracer-core)
