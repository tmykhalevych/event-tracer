#pragma once

#ifdef tracerUSE_ASSERT_HOOK
extern "C" void vTracerAssert(const char* const pcFileName, unsigned long ulLine);
#define ET_ASSERT(expr) \
    if ((expr) == 0) vTracerAssert(__FILE__, __LINE__)
#else
#include <cassert>
#define ET_ASSERT(expr) assert(expr)
#endif
