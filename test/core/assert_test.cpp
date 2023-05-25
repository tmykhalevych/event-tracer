#include <gtest/gtest.h>

#define tracerUSE_ASSERT_HOOK
#include <assert.hpp>

static bool assert_detected = false;
extern "C" void vTracerAssert(const char* const pcFileName, unsigned long ulLine)
{
    assert_detected = true;
}

TEST(EventTracerAssert, AssertDetected) {
    EXPECT_FALSE(assert_detected);
    ASSERT(false);
    EXPECT_TRUE(assert_detected);
}
