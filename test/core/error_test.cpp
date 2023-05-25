#include <gtest/gtest.h>
#include <error_assert_impl.hpp>

TEST(EventTracerError, ErrorDetected) {
    reset_error_assert();
    EXPECT_FALSE(g_error_detected);
    ERROR("test error");
    EXPECT_TRUE(g_error_detected);
}
