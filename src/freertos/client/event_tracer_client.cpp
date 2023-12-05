#include <assert.hpp>
#include <error.hpp>
#include <event_tracer.hpp>
#include <event_tracer_client.hpp>
#include <variant_helpers.hpp>

#include <FreeRTOS.h>
#include <queue.h>

#include <string_view>

namespace
{

using client_bound_fn_t = void (event_tracer::freertos::Client::*)();
template <client_bound_fn_t F>
void Bound(void *context)
{
    return std::invoke(F, static_cast<event_tracer::freertos::Client *>(context));
}

}  // namespace

namespace event_tracer::freertos
{

Client::Client(Settings settings, data_ready_cb_t consumer)
    : m_consumer(std::move(consumer)), m_polling_interval_ms(settings.polling_interval_ms)
{
    const auto data_ready_handler = [this](EventRegistry &registry, data_done_cb_t done_cb) {
        produce_message(Message{.registry = &registry, .done_cb = std::move(done_cb)});
    };

    SingleEventTracer::emplace(settings.buff, data_ready_handler, settings.get_timestamp_cb);

    m_queue_hdl = xQueueCreate(settings.data_queue_size, sizeof(Message));
    ET_ASSERT(m_queue_hdl);

    xTaskCreate(Bound<&Client::client_task>, settings.name, settings.stack_size, this, settings.prio, &m_task_hdl);
}

Client::~Client()
{
    if (m_task_hdl) {
        vTaskDelete(m_task_hdl);
    }

    SingleEventTracer::reset();

    if (m_queue_hdl) {
        vQueueDelete(m_queue_hdl);
    }
}

void Client::client_task()
{
    Message msg;

    while (true) {
        while (xQueueReceive(m_queue_hdl, &msg, 0 /* don't block */)) {
            ET_ASSERT(msg.registry);
            ET_ASSERT(msg.done_cb);
            m_consumer(*msg.registry, msg.done_cb);
        }
        vTaskDelay(m_polling_interval_ms / portTICK_PERIOD_MS);
    }
}

void Client::produce_message(Message &&msg)
{
    // TODO: handle pushing from ISR

    ET_ASSERT(m_queue_hdl);
    if (xQueueSend(m_queue_hdl, &msg, 0 /* don't block */) != pdPASS) {
        ET_ERROR("Failed to send tracing data");
    }
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
