#pragma once

#ifdef tracerUSE_ERROR_HOOK
    extern "C" void vTracerError(const char* const pcErrorMsg);
    #define error(msg) vTracerError(msg)
#else
    #include <assert.hpp>
    #include <type_traits>
    #include <string_view>
    #define error(msg) \
        static_assert(std::is_convertible_v<decltype(msg), std::string_view>); \
        assert(!msg)
#endif
