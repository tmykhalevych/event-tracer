#pragma once

#include <cstdint>
#include <limits>
#include <type_traits>

namespace event_tracer
{

/// @brief Default event timestamp resolution in bits
static constexpr uint8_t DEFAULT_TIMESTAMP_RESOLUTION_BITS = 40;

/// @brief Event descriptor
/// @tparam C Context type
/// @tparam TR Timestamp resolution
template <typename C = int16_t, unsigned TR = DEFAULT_TIMESTAMP_RESOLUTION_BITS>
struct Event
{
    using timestamp_t = uint64_t;
    using id_t = uint8_t;
    using context_t = C;

    static_assert(TR <= std::numeric_limits<timestamp_t>::digits);

    timestamp_t ts : TR;
    id_t id;
    context_t ctx;
};

/// @brief Helper to cast event type to underlying type
/// @tparam E Event type, user defined
/// @param e Event
/// @return Event casted to underlying type
template <typename E>
constexpr auto to_underlying(E e) noexcept
{
    return static_cast<std::underlying_type_t<E>>(e);
}

}  // namespace event_tracer
