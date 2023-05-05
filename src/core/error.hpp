#pragma once

#ifdef ERROR_HOOK
    #define error(desc) ERROR_HOOK(desc)
#else
    #include <assert.hpp>
    #define error(desc) assert(false)
#endif
