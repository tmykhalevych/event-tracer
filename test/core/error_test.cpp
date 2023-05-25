#include <gtest/gtest.h>

#define tracerUSE_ERROR_HOOK
#include <error.hpp>

static bool error_detected = false;
extern "C" void vTracerError(const char* const pcErrorMsg)
{
    error_detected = true;
}

TEST(EventTracerError, ErrorDetected) {
    EXPECT_FALSE(error_detected);
    ERROR("test error");
    EXPECT_TRUE(error_detected);
}
