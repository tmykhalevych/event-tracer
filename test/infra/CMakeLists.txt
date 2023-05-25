add_library(test-infra error_assert_impl.cpp)
target_include_directories(test-infra PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(test-infra PUBLIC tracerUSE_ERROR_HOOK tracerUSE_ASSERT_HOOK)
