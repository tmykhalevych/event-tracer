#pragma once

#include <array>
#include <cinttypes>
#include <cstdio>
#include <limits>
#include <optional>
#include <string_view>

#include <event.hpp>
#include <event_registry.hpp>
#include <static_function.hpp>

#include <FreeRTOS.h>
#include <task.h>

#include <array>

namespace event_tracer::freertos
{

using task_id_t = tracerTASK_ID_TYPE;
using task_prio_t = tracerTASK_PRIO_TYPE;

/// @brief FreeRTOS event context
struct EventContext
{
    task_id_t id;
    task_prio_t prio;
};

/// @brief Global context constant
static constexpr EventContext GLOBAL_CONTEXT{.id = 0, .prio = 0};

using EventDesc = event_tracer::EventDesc<EventContext>;
using EventRegistry = event_tracer::EventRegistry<EventDesc>;

/// @brief Max event message length (including terminal zero)
static constexpr size_t MAX_EVENT_MESSAGE_LEN = tracerMAX_EVENT_MESSAGE_LEN;
static_assert(MAX_EVENT_MESSAGE_LEN >= configMAX_TASK_NAME_LEN);

using message_t = std::array<char, MAX_EVENT_MESSAGE_LEN>;

/// @brief FreeRTOS event context with additional message
struct MessageEventContext
{
    task_id_t id;
    message_t msg;
};

using MessageEventDesk = event_tracer::EventDesc<MessageEventContext>;

/// @brief FreeRTOS event tracer implementation
class EventTracer
{
public:
    using data_done_cb_t = StaticFunction<void()>;
    using data_ready_cb_t = StaticFunction<void(EventRegistry &, data_done_cb_t)>;
    using get_time_cb_t = StaticFunction<EventDesc::timestamp_t()>;
    using message_cb_t = StaticFunction<void(const MessageEventDesk &)>;

    EventTracer(std::byte *buff, size_t capacity, data_ready_cb_t data_ready_cb, message_cb_t message_cb,
                get_time_cb_t get_time_cb);

    static void set_single_instance(EventTracer *tracer);
    [[nodiscard]] static EventTracer &get_single_instance();

    [[nodiscard]] EventDesc::timestamp_t now() const;

    void register_event(FreertosEvent event, std::optional<TaskHandle_t> task = std::nullopt,
                        std::optional<EventDesc::timestamp_t> timestamp = std::nullopt);

private:
    void on_registry_ready(EventRegistry &registry);
    void notify_done(EventRegistry &registry);

    EventRegistry *m_active_registry;
    EventRegistry *m_pending_registry;

    std::array<std::optional<EventRegistry>, 2> m_registries;

    data_ready_cb_t m_data_ready_cb;
    get_time_cb_t m_get_time_cb;
    message_cb_t m_message_cb;

    uint64_t m_first_ts;

    static EventTracer *m_single_instance;
};

/// @brief Short alias to access global event tracer
/// @return Reference to event tracer single instance
[[nodiscard]] inline EventTracer &tracer() { return EventTracer::get_single_instance(); }

/// @brief Event string formatter
/// @param event Event to format
/// @param newline Indicator for adding newline character at the end of string
/// @return Formatted event as a string_view
[[nodiscard]] std::string_view format(const EventDesc &event, bool newline = true);

/// @brief Message event string formatter
/// @param event Event to format
/// @param newline Indicator for adding newline character at the end of string
/// @return Formatted event as a string_view
[[nodiscard]] std::string_view format(const MessageEventDesk &event, bool newline = true);

}  // namespace event_tracer::freertos
