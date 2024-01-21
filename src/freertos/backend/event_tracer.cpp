#include <assert.hpp>
#include <error.hpp>
#include <event_tracer.hpp>
#include <scope_guard.hpp>
#include <variant_helpers.hpp>

#include <cstring>

namespace event_tracer::freertos
{

static constexpr size_t MIN_REGISTRY_CAPACITY = 20;

EventTracer::EventTracer(Settings settings)
    : m_data_ready_cb(settings.data_ready_cb), m_get_time_cb(settings.get_time_cb)
{
    ET_ASSERT(settings.buff);
    ET_ASSERT(m_data_ready_cb);
    ET_ASSERT(m_get_time_cb);

    auto [message_pool, event_storage] = settings.buff.cut(settings.message_pool_capacity * MAX_EVENT_MESSAGE_LEN);

    m_message_alloc.emplace(message_pool, MAX_EVENT_MESSAGE_LEN);

    auto [storage1, storage2] = event_storage.transform<Event>().bifurcate();

    // we can only check the storage1 because storage2 is identical
    ET_ASSERT(storage1);
    if (storage1.size() < MIN_REGISTRY_CAPACITY) {
        ET_ERROR("Buffer size could be insufficient");
    }

    m_active_registry = &m_registries[0].emplace(storage1);
    m_pending_registry = &m_registries[1].emplace(storage2);
    m_first_ts = now();

    const auto ready_cb = [this](EventRegistry &registry) { on_registry_ready(registry); };
    for (auto &registry : m_registries) {
        registry->set_ready_cb(ready_cb);
        registry->set_start_timestamp(m_first_ts);
    }
}

void EventTracer::register_event(EventId id, std::optional<TaskHandle_t> task,
                                 std::optional<Event::timestamp_t> timestamp)
{
    const auto ts = timestamp.value_or(now());
    const auto tcb = task.value_or(xTaskGetCurrentTaskHandle());

    Event event{.ts = ts, .id = to_underlying(id)};
    const auto event_registrator = ScopeGuard([&event, this] { m_active_registry->add(std::move(event)); });

    // if there is no current task control block, it means that it is a global context, and no task yet exists
    if (!tcb) {
        event.ctx.info = ContextMarker::GLOBAL_SCOPE;
        return;
    }

    TaskStatus_t task_info;
    vTaskGetInfo(tcb, &task_info, pdFALSE, eInvalid);

    event.ctx.task_id = task_info.xTaskNumber;

    // add task name to task lifetime events
    if (id == EventId::TASK_CREATE || id == EventId::TASK_DELETE) {
        add_message(event, task_info.pcTaskName);
        return;
    }

    // for all other events, add dedicated task priority to events
    event.ctx.info = task_prio_t{task_info.uxCurrentPriority};
}

void EventTracer::register_user_event(UserEventId id, std::optional<std::string_view> message,
                                      const TaskStatus_t *task_status)
{
    const auto ts = now();

    TaskStatus_t task_info;
    if (task_status) {
        task_info = *task_status;
    }
    else {
        const auto tcb = xTaskGetCurrentTaskHandle();
        vTaskGetInfo(tcb, &task_info, pdFALSE, eInvalid);
    }

    Event event{.ts = ts, .id = to_underlying(id), .ctx = {.task_id = task_info.xTaskNumber}};

    if (message) {
        add_message(event, message.value());
    }

    m_active_registry->add(std::move(event));
}

void EventTracer::notify_done(EventRegistry &registry)
{
    ET_ASSERT(&registry != m_active_registry);

    for (const auto &event : registry) {
        release_message_for(event);
    }

    registry.reset();
}

void EventTracer::add_message(Event &event, std::string_view src)
{
    auto msg = message_t::create(src, *m_message_alloc);
    if (msg) {
        event.ctx.info = msg.value();
    }
    else {
        event.ctx.info = ContextMarker::MESSAGE_LOST;
    }
}

void EventTracer::release_message_for(const Event &event)
{
    std::visit(Alternatives{[this](message_t msg) { message_t::destroy(msg, *m_message_alloc); }, Alternative::ignore},
               event.ctx.info);
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

}  // namespace event_tracer::freertos
