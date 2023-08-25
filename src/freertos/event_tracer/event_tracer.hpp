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

namespace event_tracer::freertos
{

/// @brief FreeRTOS event context
struct EventContext
{
    uint8_t id;
    uint8_t prio;
};

/// @brief Global context constant
static constexpr EventContext GLOBAL_CONTEXT{.id = 0, .prio = 0};

using EventDesc = event_tracer::EventDesc<EventContext>;
using EventRegistry = event_tracer::EventRegistry<EventDesc>;

static_assert(sizeof(EventDesc) == 8, "EventDesk should be packed into 8 bytes");

/// @brief FreeRTOS event tracer implementation
class EventTracer
{
public:
    using data_done_cb_t = StaticFunction<void()>;
    using data_ready_cb_t = StaticFunction<void(EventRegistry &, data_done_cb_t)>;
    using get_time_cb_t = StaticFunction<EventDesc::timestamp_t()>;

    EventTracer(std::byte *buff, size_t capacity, data_ready_cb_t data_ready_cb);

    static void set_single_instance(EventTracer *tracer);
    [[nodiscard]] static EventTracer &get_single_instance();

    void set_time_getter(get_time_cb_t cb) { m_get_time_cb = cb; }
    [[nodiscard]] EventDesc::timestamp_t now() const;

    void register_event(Event event, std::optional<TaskHandle_t> task = std::nullopt,
                        std::optional<EventDesc::timestamp_t> timestamp = std::nullopt);

private:
    void on_registry_ready(EventRegistry &registry);
    void notify_done(EventRegistry &registry);

    void on_task_delete(const TaskStatus_t &info);

    EventRegistry *m_active_registry;
    EventRegistry *m_pending_registry;

    std::array<std::optional<EventRegistry>, 2> m_registries;

    data_ready_cb_t m_data_ready_cb;
    get_time_cb_t m_get_time_cb;

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

}  // namespace event_tracer::freertos
