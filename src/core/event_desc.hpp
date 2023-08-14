#pragma once

#include <cstdint>
#include <type_traits>

namespace event_tracer
{

/// @brief Basic event descriptor
/// @tparam CT Context concrete type
template <typename CT = int16_t>
struct EventDesc
{
    using timestamp_type = uint64_t;
    using context_type = CT;
    using id_type = uint8_t;

    timestamp_type ts : 40;
    id_type id;
    context_type ctx;
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
