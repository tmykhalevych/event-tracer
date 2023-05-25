#include <gtest/gtest.h>
#include <assert.hpp>
#include <error_assert_impl.hpp>

TEST(Assert, Detected) {
    CLEAR_ERROR_ASSERT();
    ASSERT(false);
    EXPECT_ASSERT();
}
