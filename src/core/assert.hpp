#pragma once

#ifdef tracerASSERT_HOOK
    #define assert(expr) tracerASSERT_HOOK(expr)
#else
    #include <cassert>
#endif
