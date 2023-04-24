#pragma once

#include <cstdint>

namespace event_tracer
{

/// @brief Basic event descriptor
/// @tparam CT Context concrete type
template<typename CT>
struct EventDesc
{
    static_assert(sizeof(CT) <= 2, "Context should be packed into 2 bytes");

    using context_t = CT;
    uint64_t ts : 40;
    uint8_t id;
    context_t ctx;
};

/// @brief Basic event descriptor
/// @note Uint16 is used as an event context
using RawEventDesc = EventDesc<int16_t>;

} // namespace event_tracer
