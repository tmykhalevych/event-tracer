#include <event_tracer.hpp>
#include <assert.hpp>
#include <error.hpp>

#include <FreeRTOS.h>
#include <task.h>

namespace event_tracer::freertos
{

EventTracer* EventTracer::m_single_instance = nullptr;

EventTracer::EventTracer(std::byte *buff, size_t capacity, data_ready_cb_t data_ready_cb)
    : m_data_ready_cb(data_ready_cb)
{
    const size_t registry_capacity = capacity / sizeof(EventDesc) / 2;
    EventDesc *registry_ptr = reinterpret_cast<EventDesc*>(buff);

    /// @todo Add checks here

    m_active_registry = &m_registries[0].emplace(registry_ptr, registry_capacity);
    m_pending_registry = &m_registries[1].emplace(registry_ptr + registry_capacity, registry_capacity);

    const auto ready_cb = [this](EventRegistry& registry) { on_registry_ready(registry); };
    m_active_registry->set_ready_cb(ready_cb);
    m_pending_registry->set_ready_cb(ready_cb);
}

void EventTracer::set_single_instance(EventTracer *tracer)
{
    assert(!m_single_instance);
    m_single_instance = tracer;
}

EventTracer& EventTracer::get_single_instance()
{
    assert(m_single_instance);
    return *m_single_instance;
}

void EventTracer::register_event(EventDesc desc)
{
    m_active_registry->add(std::move(desc));
}

void EventTracer::register_event(Event event)
{
    assert(m_get_time_cb);
    const auto timestamp = m_get_time_cb();
    const auto tcb = xTaskGetCurrentTaskHandle();
    register_event({
        .ts = timestamp,
        .id = to_underlying(event),
        .ctx = {
            .id = static_cast<uint8_t>(uxTaskGetTaskNumber(tcb)),
            .prio = static_cast<uint8_t>(uxTaskPriorityGet(tcb))
        }
    });
}

void EventTracer::notify_done(EventRegistry &registry)
{
    assert(&registry != m_active_registry);
    registry.reset();
}

void EventTracer::on_registry_ready(EventRegistry &registry)
{
    if (!m_pending_registry->empty()) {
        error("Pending registry is not empty, dropping active registry");
        m_active_registry->reset();
        return;
    }

    std::swap(m_active_registry, m_pending_registry);
    m_data_ready_cb(*m_pending_registry, [this, &registry = *m_pending_registry]{ notify_done(registry); });
}

} // namespace event_tracer::freertos
