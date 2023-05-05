#pragma once

#include <cstdint>

namespace event_tracer
{

/// @brief Basic event descriptor
/// @tparam CT Context concrete type
template<typename CT = int16_t>
struct EventDesc
{
    static_assert(sizeof(CT) <= 2, "Context should be packed into 2 bytes");

    using context_type = CT;
    using id_type = uint8_t;

    uint64_t ts : 40;
    id_type id;
    context_type ctx;
};

} // namespace event_tracer
