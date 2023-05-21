#pragma once

#include <array>
#include <optional>
#include <functional>

#include <event_registry.hpp>
#include <event.hpp>

namespace event_tracer::freertos
{

/// @brief FreeRTOS event context
struct EventContext
{
    uint8_t id;
    uint8_t prio;
};

using EventDesc = event_tracer::EventDesc<EventContext>;
using EventRegistry = event_tracer::EventRegistry<EventDesc>;

/// @brief FreeRTOS event tracer implementation
class EventTracer
{
public:
    using data_done_cb_t = std::function<void()>;
    using data_ready_cb_t = std::function<void(EventRegistry&, data_done_cb_t)>;
    using get_time_cb_t = std::function<EventDesc::timestamp_type()>;

    EventTracer(std::byte *buff, size_t capacity, data_ready_cb_t data_ready_cb);

    static void set_single_instance(EventTracer *tracer);
    [[nodiscard]] static EventTracer& get_single_instance();

    void set_time_getter(get_time_cb_t cb) { m_get_time_cb = cb; }

    void register_event(EventDesc desc);
    void register_event(Event event);

private:
    void on_registry_ready(EventRegistry &registry);
    void notify_done(EventRegistry &registry);

    EventRegistry *m_active_registry;
    EventRegistry *m_pending_registry;

    std::array<std::optional<EventRegistry>, 2> m_registries;
    data_ready_cb_t m_data_ready_cb;
    get_time_cb_t m_get_time_cb;

    static EventTracer* m_single_instance;
};

/// @brief Short alias to access global event tracer
/// @return Reference to event tracer single instance
[[nodiscard]] inline EventTracer& tracer()
{
    return EventTracer::get_single_instance();
}

} // namespace event_tracer::freertos
