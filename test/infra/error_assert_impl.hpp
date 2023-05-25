#pragma once

namespace impl
{

bool assert_happened();
bool error_happened();
void reset_error_assert();

} // namespace impl

#define EXPECT_ASSERT() EXPECT_TRUE(impl::assert_happened())
#define EXPECT_ERROR() EXPECT_TRUE(impl::error_happened())
#define EXPECT_NO_ERROR_OR_ASSERT() EXPECT_FALSE(impl::error_happened() || impl::assert_happened())
#define CLEAR_ERROR_ASSERT() impl::reset_error_assert()
