#include <assert.hpp>
#include <error.hpp>
#include <event_tracer.hpp>
#include <scope_guard.hpp>
#include <variant_helpers.hpp>

#include <cstring>

namespace event_tracer::freertos
{

static constexpr size_t MIN_REGISTRY_CAPACITY = 20;

EventTracer::EventTracer(std::byte *buff, size_t capacity, data_ready_cb_t data_ready_cb, get_time_cb_t get_time_cb)
    : m_data_ready_cb(data_ready_cb), m_get_time_cb(get_time_cb)
{
    ET_ASSERT(buff);
    ET_ASSERT(data_ready_cb);
    ET_ASSERT(get_time_cb);

    const size_t registry_capacity = capacity / sizeof(Event) / 2;
    Event *registry_ptr = reinterpret_cast<Event *>(buff);

    ET_ASSERT(registry_capacity > 1);
    if (registry_capacity < MIN_REGISTRY_CAPACITY) {
        ET_ERROR("Buffer size could be insufficient");
    }

    m_active_registry = &m_registries[0].emplace(Span(registry_ptr, registry_capacity));
    m_pending_registry = &m_registries[1].emplace(Span(registry_ptr + registry_capacity, registry_capacity));
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
        event.ctx.info = ContextMarker::GLOBAL;
        return;
    }

    TaskStatus_t task_info;
    vTaskGetInfo(tcb, &task_info, pdFALSE, eInvalid);

    event.ctx.task_id = task_info.xTaskNumber;

    // add task name to task lifetime events
    if (needs_message(id)) {
        message_t msg;
        std::strncpy(msg.data(), task_info.pcTaskName, msg.max_size());
        event.ctx.info = msg;
        return;
    }

    // for all other events, add dedicated task priority to events
    event.ctx.info = task_prio_t{task_info.uxCurrentPriority};
}

void EventTracer::register_user_event(const message_t &message)
{
    const auto ts = now();
    const auto tcb = xTaskGetCurrentTaskHandle();

    TaskStatus_t task_info;
    vTaskGetInfo(tcb, &task_info, pdFALSE, eInvalid);

    Event event{
        .ts = ts, .id = to_underlying(EventId::EVENT_USER), .ctx = {.task_id = task_info.xTaskNumber, .info = message}};

    m_active_registry->add(std::move(event));
}

void EventTracer::notify_done(EventRegistry &registry)
{
    ET_ASSERT(&registry != m_active_registry);
    registry.reset();
}

constexpr bool EventTracer::needs_message(EventId id)
{
    switch (id) {
        case EventId::TASK_CREATE:
        case EventId::TASK_DELETE: return true;
        default: return false;
    }
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

std::string_view format(const Event &e, bool newline)
{
    static char EVENT_STR[] = "{ts:%" PRIu64 ",event:%" PRIu8 ",ctx:{task:%" PRIu64 ",info:{%s}}}%s";
    static char CTX_PRIO_STR[] = "prio:%" PRIu64;
    static char CTX_MSG_STR[] = "msg:\"%s\"";
    static char CTX_MRK_STR[] = "mark:%" PRIu8;

    static constexpr auto CTX_INFO_STR_SIZE = std::max<size_t>(
        {std::numeric_limits<task_prio_t>::digits10 + sizeof(CTX_PRIO_STR), MAX_EVENT_MESSAGE_LEN + sizeof(CTX_MSG_STR),
         std::numeric_limits<ContextMarker>::digits10 + sizeof(CTX_MRK_STR)});

    static constexpr auto EVENT_CONTENT_STR_SIZE = std::numeric_limits<decltype(Event::ts)>::digits10 +
                                                   std::numeric_limits<decltype(Event::id)>::digits10 +
                                                   CTX_INFO_STR_SIZE;

    static constexpr auto EVENT_STR_SIZE = EVENT_CONTENT_STR_SIZE + sizeof(EVENT_STR);

    static char event_str[EVENT_STR_SIZE];
    static char ctx_str[CTX_INFO_STR_SIZE];

    std::visit(
        Alternatives{[&](task_prio_t prio) { std::snprintf(ctx_str, CTX_INFO_STR_SIZE, CTX_PRIO_STR, prio); },
                     [&](const message_t &msg) { std::snprintf(ctx_str, CTX_INFO_STR_SIZE, CTX_MSG_STR, msg.data()); },
                     [&](ContextMarker m) { std::snprintf(ctx_str, CTX_INFO_STR_SIZE, CTX_MRK_STR, m); }},
        e.ctx.info);

    std::snprintf(event_str, EVENT_STR_SIZE, EVENT_STR, e.ts, e.id, e.ctx.task_id, ctx_str, newline ? "\n" : "");

    return event_str;
}

}  // namespace event_tracer::freertos
