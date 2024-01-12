#pragma once

#include <event.hpp>

namespace event_tracer::freertos
{

/// @brief User events enumeration
enum class UserEventId : event_tracer::Event<>::id_t
{
    UNDEFINED = 0,
    START_CAPTURING,
    STOP_CAPTURING,
    MESSAGE,

    /// @warning Not for public usage
    NEXT
};

}  // namespace event_tracer::freertos
