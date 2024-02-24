#pragma once

#include <context_marker.hpp>
#include <event_id.hpp>
#include <event_registry.hpp>
#include <inplace_function.hpp>
#include <message.hpp>
#include <slice.hpp>
#include <user_event_id.hpp>

#include <FreeRTOS.h>
#include <task.h>

#include <array>
#include <cstdint>
#include <type_traits>
#include <variant>

#ifndef tracerMAX_EVENT_MESSAGE_LEN
#define tracerMAX_EVENT_MESSAGE_LEN configMAX_TASK_NAME_LEN
#else
static_assert(std::is_integral_v<decltype(tracerMAX_EVENT_MESSAGE_LEN)> &&
              tracerMAX_EVENT_MESSAGE_LEN >= configMAX_TASK_NAME_LEN);
#endif

#ifndef tracerTASK_ID_TYPE
#define tracerTASK_ID_TYPE decltype(TaskStatus_t::xTaskNumber)
#else
static_assert(std::is_integral_v<tracerTASK_ID_TYPE>);
#endif

#ifndef tracerTASK_PRIO_TYPE
#define tracerTASK_PRIO_TYPE decltype(TaskStatus_t::uxCurrentPriority)
#else
static_assert(std::is_integral_v<tracerTASK_PRIO_TYPE>);
#endif

#ifndef tracerTIMESTAMP_RESOLUTION_BITS
#define tracerTIMESTAMP_RESOLUTION_BITS DEFAULT_TIMESTAMP_RESOLUTION_BITS
#else
static_assert(std::is_integral_v<decltype(tracerTIMESTAMP_RESOLUTION_BITS)>);
#endif

namespace event_tracer::freertos
{

/// @brief Max event message length (including terminal zero)
static constexpr size_t MAX_EVENT_MESSAGE_LEN = (tracerMAX_EVENT_MESSAGE_LEN);

using task_id_t = tracerTASK_ID_TYPE;
using task_prio_t = tracerTASK_PRIO_TYPE;
using message_t = Message<MAX_EVENT_MESSAGE_LEN>;

/// @brief Context of FreeRTOS event
struct EventContext
{
    task_id_t task_id;
    std::variant<task_prio_t, message_t, ContextMarker> info;
};

using Event = event_tracer::Event<EventContext, (tracerTIMESTAMP_RESOLUTION_BITS)>;
using EventRegistry = event_tracer::EventRegistry<Event>;

using data_done_cb_t = InplaceFunction<void()>;
using data_ready_cb_t = InplaceFunction<void(EventRegistry &, data_done_cb_t)>;

using event_tracer::as_slice;
using event_tracer::Slice;

}  // namespace event_tracer::freertos

namespace freertos_tracer = event_tracer::freertos;
