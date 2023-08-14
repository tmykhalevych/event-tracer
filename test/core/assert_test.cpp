#include <assert.hpp>
#include <error_assert_impl.hpp>
#include <gtest/gtest.h>

TEST(Assert, Detected)
{
    CLEAR_ERROR_ASSERT();
    ET_ASSERT(false);
    EXPECT_ASSERT();
}
