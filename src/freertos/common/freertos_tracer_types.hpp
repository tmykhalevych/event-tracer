#pragma once

#include <array>

#include <event_id.hpp>
#include <event_registry.hpp>
#include <inplace_function.hpp>

#include <FreeRTOS.h>
#include <task.h>

#include <array>
#include <variant>

#ifndef tracerMAX_EVENT_MESSAGE_LEN
#define tracerMAX_EVENT_MESSAGE_LEN configMAX_TASK_NAME_LEN
#else
static_assert(tracerMAX_EVENT_MESSAGE_LEN >= configMAX_TASK_NAME_LEN);
#endif

#ifndef tracerTASK_ID_TYPE
#define tracerTASK_ID_TYPE decltype(TaskStatus_t::xTaskNumber)
#endif

#ifndef tracerTASK_PRIO_TYPE
#define tracerTASK_PRIO_TYPE decltype(TaskStatus_t::uxCurrentPriority)
#endif

namespace event_tracer::freertos
{

/// @brief Max event message length (including terminal zero)
static constexpr size_t MAX_EVENT_MESSAGE_LEN = tracerMAX_EVENT_MESSAGE_LEN;

using task_id_t = tracerTASK_ID_TYPE;
using task_prio_t = tracerTASK_PRIO_TYPE;
using message_t = std::array<char, MAX_EVENT_MESSAGE_LEN>;

/// @brief Marker for the events came from non-FreeRTOS context
enum class ContextMarker
{
    GLOBAL
};

/// @brief Context of FreeRTOS event
struct EventContext
{
    task_id_t task_id;
    std::variant<task_prio_t, message_t, ContextMarker> info;
};

using Event = event_tracer::Event<EventContext>;
using EventRegistry = event_tracer::EventRegistry<Event>;

using data_done_cb_t = InplaceFunction<void()>;
using data_ready_cb_t = InplaceFunction<void(EventRegistry &, data_done_cb_t)>;

}  // namespace event_tracer::freertos
