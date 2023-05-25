#pragma once

#include <assert.hpp>
#include <error.hpp>

static bool g_assert_detected = false;
static bool g_error_detected = false;

extern "C" inline void vTracerAssert(const char* const pcFileName, unsigned long ulLine)
{
    g_assert_detected = true;
}

extern "C" inline void vTracerError(const char* const pcErrorMsg)
{
    g_error_detected = true;
}

constexpr void reset_error_assert()
{
    g_assert_detected = false;
    g_error_detected = false;
}
