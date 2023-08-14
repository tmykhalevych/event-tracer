#include <error.hpp>
#include <error_assert_impl.hpp>
#include <gtest/gtest.h>

TEST(Error, Detected)
{
    CLEAR_ERROR_ASSERT();
    ET_ERROR("test error");
    EXPECT_ERROR();
}
