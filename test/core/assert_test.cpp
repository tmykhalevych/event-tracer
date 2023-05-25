#include <gtest/gtest.h>
#include <error_assert_impl.hpp>

TEST(EventTracerAssert, AssertDetected) {
    reset_error_assert();
    EXPECT_FALSE(g_assert_detected);
    ASSERT(false);
    EXPECT_TRUE(g_assert_detected);
}
