#include <error_assert_impl.hpp>

namespace
{

static bool g_assert = false;
static bool g_error = false;

} // namespace

extern "C" void vTracerAssert(const char* const pcFileName, unsigned long ulLine)
{
    g_assert = true;
}

extern "C" void vTracerError(const char* const pcErrorMsg)
{
    g_error = true;
}

namespace impl
{

bool assert_happened()
{
    return g_assert;
}

bool error_happened()
{
    return g_error;
}

void reset_error_assert()
{
    g_error = false;
    g_assert = false;
}

} // namespace impl
