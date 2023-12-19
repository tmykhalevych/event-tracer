#pragma once

#ifdef tracerUSE_ERROR_HOOK
void event_tracer_error(const char* const message);
#define ET_ERROR(msg) event_tracer_error(msg)
#else
#include <assert.hpp>
#include <string_view>
#include <type_traits>
#define ET_ERROR(msg)                                                      \
    static_assert(std::is_convertible_v<decltype(msg), std::string_view>); \
    ET_ASSERT(!msg)
#endif
