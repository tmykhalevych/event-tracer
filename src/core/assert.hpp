#pragma once

#ifdef tracerUSE_ASSERT_HOOK
void event_tracer_assert(const char* const file, unsigned long line);
#define ET_ASSERT(expr) \
    if (!(expr)) event_tracer_assert(__FILE__, __LINE__)
#else
#include <cassert>
#define ET_ASSERT(expr) assert(expr)
#endif
