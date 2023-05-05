#pragma once

#include <array>
#include <optional>
#include <functional>

#include <event_registry.hpp>

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

    EventTracer(std::byte *buff, size_t capacity, data_ready_cb_t data_ready_cb);

    static void set_single_instance(EventTracer &tracer);
    [[nodiscard]] static EventTracer& get_single_instance();

    void register_event(EventDesc &&event);

private:
    void on_registry_ready(EventRegistry &registry);
    void notify_done(EventRegistry &registry);

    EventRegistry *m_active_registry;
    EventRegistry *m_pending_registry;

    std::array<std::optional<EventRegistry>, 2> m_registries;
    data_ready_cb_t m_data_ready_cb;

    static EventTracer* m_single_instance;
};

/// @brief Short alias to access global event tracer
/// @return Reference to event tracer single instance
inline EventTracer& global_tracer()
{
    return EventTracer::get_single_instance();
}

} // namespace event_tracer::freertos
