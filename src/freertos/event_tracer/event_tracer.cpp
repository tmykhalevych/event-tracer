#include <assert.hpp>
#include <error.hpp>
#include <event_tracer.hpp>

#include <cstring>

namespace event_tracer::freertos
{

EventTracer *EventTracer::m_single_instance = nullptr;
static constexpr size_t MIN_REGISTRY_CAPACITY = 20;

EventTracer::EventTracer(std::byte *buff, size_t capacity, data_ready_cb_t data_ready_cb, message_cb_t message_cb,
                         get_time_cb_t get_time_cb)
    : m_data_ready_cb(data_ready_cb), m_message_cb(message_cb), m_get_time_cb(get_time_cb)
{
    ET_ASSERT(buff);
    ET_ASSERT(get_time_cb);

    const size_t registry_capacity = capacity / sizeof(EventDesc) / 2;
    EventDesc *registry_ptr = reinterpret_cast<EventDesc *>(buff);

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

EventDesc::timestamp_t EventTracer::now() const { return m_get_time_cb(); }

void EventTracer::register_event(Event event, std::optional<TaskHandle_t> task,
                                 std::optional<EventDesc::timestamp_t> timestamp)
{
    const auto ts = timestamp.value_or(now());
    const auto tcb = task.value_or(xTaskGetCurrentTaskHandle());
    EventContext ctx = GLOBAL_CONTEXT;
    TaskStatus_t info;

    if (tcb) {
        vTaskGetInfo(tcb, &info, pdFALSE, eInvalid);
    }

    // add task name for task lifetime event
    if (event == Event::TASK_CREATE || event == Event::TASK_DELETE) {
        MessageEventDesk msg_event_desc{
            .ts = ts - m_first_ts, .id = to_underlying(event), .ctx = {.id = static_cast<uint8_t>(info.xTaskNumber)}};

        std::strncpy(msg_event_desc.ctx.msg.data(), info.pcTaskName, msg_event_desc.ctx.msg.max_size());
        m_message_cb(msg_event_desc);
    }
    else {
        EventDesc event_desc{.ts = ts,
                             .id = to_underlying(event),
                             .ctx = {.id = static_cast<uint8_t>(info.xTaskNumber),
                                     .prio = static_cast<uint8_t>(info.uxCurrentPriority)}};

        m_active_registry->add(std::move(event_desc));
    }
}

void EventTracer::notify_done(EventRegistry &registry)
{
    ET_ASSERT(&registry != m_active_registry);
    registry.reset();
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

    std::snprintf(event_str, EVENT_STR_SIZE, "{ts:%" PRIu64 ",event:%" PRIu8 ",task:%" PRIu32 ",prio:%" PRIu32 "}%s",
                  event.ts, event.id, static_cast<uint32_t>(event.ctx.id), static_cast<uint32_t>(event.ctx.prio),
                  newline ? "\n" : "");

    return event_str;
}

std::string_view format(const MessageEventDesk &event, bool newline)
{
    static constexpr auto MSG_EVENT_STR_SIZE =
        std::numeric_limits<decltype(event.ts)>::digits10 + std::numeric_limits<decltype(event.id)>::digits10 +
        std::numeric_limits<decltype(event.ctx.id)>::digits10 + sizeof(event.ctx.msg) +
        30 /* message body (braces, commas, etc) + null terminator */ + 5 /* just in case */;

    static char msg_event_str[MSG_EVENT_STR_SIZE];

    std::snprintf(msg_event_str, MSG_EVENT_STR_SIZE, "{ts:%" PRIu64 ",event:%" PRIu8 ",task:%" PRIu32 ",msg:\"%s\"}%s",
                  event.ts, event.id, static_cast<uint32_t>(event.ctx.id), event.ctx.msg.data(), newline ? "\n" : "");

    return msg_event_str;
}

}  // namespace event_tracer::freertos
