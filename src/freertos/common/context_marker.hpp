#pragma once

#include <event.hpp>

namespace event_tracer::freertos
{

/// @brief Marker for the events to indicate unusual cases
enum class ContextMarker : event_tracer::Event<>::id_t
{
    UNDEFINED = 0,
    GLOBAL_SCOPE,
    MESSAGE_LOST
};

}  // namespace event_tracer::freertos
