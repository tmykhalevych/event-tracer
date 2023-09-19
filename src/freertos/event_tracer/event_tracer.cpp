#include <assert.hpp>
#include <error.hpp>
#include <event_tracer.hpp>

namespace event_tracer::freertos
{

EventTracer *EventTracer::m_single_instance = nullptr;
static constexpr size_t MIN_REGISTRY_CAPACITY = 20;

EventTracer::EventTracer(std::byte *buff, size_t capacity, data_ready_cb_t data_ready_cb)
    : m_data_ready_cb(data_ready_cb)
{
    ET_ASSERT(buff);

    const size_t registry_capacity = capacity / sizeof(EventDesc) / 2;
    EventDesc *registry_ptr = reinterpret_cast<EventDesc *>(buff);

    ET_ASSERT(registry_capacity > 1);
    if (registry_capacity < MIN_REGISTRY_CAPACITY) {
        ET_ERROR("Buffer size could be insufficient");
    }

    m_active_registry = &m_registries[0].emplace(Span(registry_ptr, registry_capacity));
    m_pending_registry = &m_registries[1].emplace(Span(registry_ptr + registry_capacity, registry_capacity));

    const auto ready_cb = [this](EventRegistry &registry) { on_registry_ready(registry); };
    m_active_registry->set_ready_cb(ready_cb);
    m_pending_registry->set_ready_cb(ready_cb);
}

void EventTracer::set_single_instance(EventTracer *tracer)
{
    ET_ASSERT(!m_single_instance);
    m_single_instance = tracer;
}

EventTracer &EventTracer::get_single_instance()
{
    ET_ASSERT(m_single_instance);
    return *m_single_instance;
}

EventDesc::timestamp_t EventTracer::now() const
{
    ET_ASSERT(m_get_time_cb);
    return m_get_time_cb();
}

void EventTracer::register_event(Event event, std::optional<TaskHandle_t> task,
                                 std::optional<EventDesc::timestamp_t> timestamp)
{
    const auto ts = timestamp.value_or(now());
    const auto tcb = task.value_or(xTaskGetCurrentTaskHandle());
    EventContext ctx = GLOBAL_CONTEXT;
    TaskStatus_t info;

    if (tcb) {
        vTaskGetInfo(tcb, &info, pdFALSE, eInvalid);
        ctx = {.id = static_cast<uint8_t>(info.xTaskNumber), .prio = static_cast<uint8_t>(info.uxCurrentPriority)};
    }

    // handle specific events differently
    if (handle_specific(event, info)) {
        return;
    }

    m_active_registry->add({.ts = ts, .id = to_underlying(event), .ctx = std::move(ctx)});
}

void EventTracer::notify_done(EventRegistry &registry)
{
    ET_ASSERT(&registry != m_active_registry);
    registry.reset();
}

bool EventTracer::handle_specific(Event event, const TaskStatus_t &info)
{
    switch (event) {
        case Event::TASK_CREATE: on_task_create(info); return true;
        case Event::TASK_DELETE: on_task_delete(info); return true;

        default: return false;
    }
}

void EventTracer::on_task_create(const TaskStatus_t &info)
{
    // TODO: implement
}

void EventTracer::on_task_delete(const TaskStatus_t &info)
{
    // TODO: implement
}

void EventTracer::on_registry_ready(EventRegistry &registry)
{
    if (!m_pending_registry->empty()) {
        ET_ERROR("Pending registry is not empty, dropping active registry");
        m_active_registry->reset();
        return;
    }

    std::swap(m_active_registry, m_pending_registry);
    m_data_ready_cb(*m_pending_registry, [this, &registry = *m_pending_registry] { notify_done(registry); });
}

std::string_view format(const EventDesc &event, bool newline)
{
    static constexpr auto EVENT_STR_SIZE =
        std::numeric_limits<decltype(event.ts)>::digits10 + std::numeric_limits<decltype(event.id)>::digits10 +
        std::numeric_limits<decltype(event.ctx.id)>::digits10 +
        std::numeric_limits<decltype(event.ctx.prio)>::digits10 +
        30 /* message body (braces, commas, etc) + null terminator */ + 5 /* just in case */;

    static char event_str[EVENT_STR_SIZE];

    std::snprintf(event_str, EVENT_STR_SIZE, "{ts:%" PRIu64 ",event:%" PRIu8 ",task:%" PRIu16 ",prio:%" PRIu16 "}%s",
                  event.ts, event.id, event.ctx.id, event.ctx.prio, newline ? "\n" : "");

    return event_str;
}

}  // namespace event_tracer::freertos
