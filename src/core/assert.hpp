#pragma once

#ifdef ASSERT_HOOK
    #define assert(expr) ASSERT_HOOK(expr)
#else
    #include <cassert>
#endif
