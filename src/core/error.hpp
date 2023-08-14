#pragma once

#ifdef tracerUSE_ERROR_HOOK
extern "C" void vTracerError(const char* const pcErrorMsg);
#define ET_ERROR(msg) vTracerError(msg)
#else
#include <assert.hpp>
#include <string_view>
#include <type_traits>
#define ET_ERROR(msg)                                                      \
    static_assert(std::is_convertible_v<decltype(msg), std::string_view>); \
    ET_ASSERT(!msg)
#endif
