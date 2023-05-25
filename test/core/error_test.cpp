#include <gtest/gtest.h>
#include <error.hpp>
#include <error_assert_impl.hpp>

TEST(Error, Detected) {
    CLEAR_ERROR_ASSERT();
    ERROR("test error");
    EXPECT_ERROR();
}
