#pragma once

#include <array>
#include <cinttypes>
#include <cstdio>
#include <limits>
#include <optional>
#include <string_view>

#include <event_id.hpp>
#include <event_registry.hpp>
#include <inplace_function.hpp>
#include <prohibit_copy_move.hpp>
#include <singleton.hpp>

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

/// @brief FreeRTOS event tracer implementation
class EventTracer : public ProhibitCopyMove
{
public:
    using data_done_cb_t = InplaceFunction<void()>;
    using data_ready_cb_t = InplaceFunction<void(EventRegistry &, data_done_cb_t)>;
    using get_time_cb_t = InplaceFunction<Event::timestamp_t()>;

    EventTracer(std::byte *buff, size_t capacity, data_ready_cb_t data_ready_cb, get_time_cb_t get_time_cb);

    void register_event(EventId id, std::optional<TaskHandle_t> task = std::nullopt,
                        std::optional<Event::timestamp_t> timestamp = std::nullopt);

    void register_user_event(const message_t &message);

    [[nodiscard]] Event::timestamp_t now() const { return m_get_time_cb(); }

private:
    void on_registry_ready(EventRegistry &registry);
    void notify_done(EventRegistry &registry);

    constexpr bool needs_message(EventId id);

    EventRegistry *m_active_registry;
    EventRegistry *m_pending_registry;

    std::array<std::optional<EventRegistry>, 2> m_registries;

    data_ready_cb_t m_data_ready_cb;
    get_time_cb_t m_get_time_cb;

    uint64_t m_first_ts;
};

using SingleEventTracer = Singleton<EventTracer>;
using EventTracerPtr = SingleEventTracer::Ptr;

/// @brief Event string formatter
/// @param event Event to format
/// @param newline Indicator for adding newline character at the end of string
/// @return Formatted event as a string_view
[[nodiscard]] std::string_view format(const Event &e, bool newline = true);

}  // namespace event_tracer::freertos
