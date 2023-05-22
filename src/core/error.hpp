#pragma once

#ifdef tracerERROR_HOOK
    #define error(desc) tracerERROR_HOOK(desc)
#else
    #include <assert.hpp>
    #include <type_traits>
    #include <string_view>
    #define error(desc) \
        static_assert(std::is_convertible_v<decltype(desc), std::string_view>); \
        assert(!desc)
#endif
