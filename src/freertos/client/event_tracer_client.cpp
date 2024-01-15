#include <assert.hpp>
#include <critical_section.hpp>
#include <error.hpp>
#include <event_tracer.hpp>
#include <event_tracer_client.hpp>
#include <variant_helpers.hpp>

#include <FreeRTOS.h>
#include <portmacro.h>
#include <queue.h>
#include <task.h>

#include <mutex>
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
    : m_consumer(std::move(consumer))
    , m_polling_interval_ms(settings.polling_interval_ms)
    , m_max_task_num_expected(settings.max_task_num_expected)
{
    m_system_state =
        Span<TaskStatus_t>(reinterpret_cast<TaskStatus_t*>(settings.buff.data), m_max_task_num_expected);

    ET_ASSERT(m_system_state.size_bytes() < settings.buff.size_bytes());

    const auto reserved_size = m_system_state.size_bytes();
    const Span<std::byte> traces_buff(settings.buff.data + reserved_size, settings.buff.size - reserved_size);

    const auto data_ready_handler = [this](EventRegistry &registry, data_done_cb_t done_cb) {
        produce_message(Message{.registry = &registry, .done_cb = std::move(done_cb)});
    };

    SingleEventTracer::emplace(traces_buff, data_ready_handler, settings.get_timestamp_cb);

    m_queue_hdl = xQueueCreate(2 /* 2 items for alternated registries */, sizeof(Message));
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

void Client::emit(UserEventId event, std::optional<std::string_view> message)
{
    if (event == UserEventId::DUMP_SYSTEM_STATE) {
        dump_system_state();
        return;
    }

    {
        std::scoped_lock lock(INTERRUPTS);
        SingleEventTracer::instance()->register_user_event(event, message);
    }

    // dump system state after capturing start, just to update the info
    if (event == UserEventId::START_CAPTURING) {
        dump_system_state();
    }
}

void Client::client_task()
{
    Message msg;

    while (true) {
        ET_ASSERT(m_queue_hdl);
        while (xQueueReceive(m_queue_hdl, &msg, 0 /* don't block */) == pdPASS) {
            ET_ASSERT(msg.registry);
            ET_ASSERT(msg.done_cb);
            m_consumer(*msg.registry, msg.done_cb);
        }
        vTaskDelay(m_polling_interval_ms / portTICK_PERIOD_MS);
    }
}

void Client::produce_message(Message &&msg)
{
    ET_ASSERT(m_queue_hdl);

    const bool is_inside_isr = xPortIsInsideInterrupt();
    auto higher_prio_task_woken = pdFALSE;

    const auto status = is_inside_isr ? xQueueSendFromISR(m_queue_hdl, &msg, &higher_prio_task_woken)
                                      : xQueueSend(m_queue_hdl, &msg, 0 /* don't block */);

    if (status != pdPASS) {
        ET_ERROR("Failed to send tracing data");
        return;
    }

    if (is_inside_isr) {
        portYIELD_FROM_ISR(higher_prio_task_woken);
    }
}

void Client::dump_system_state()
{
    const auto task_num = uxTaskGetNumberOfTasks();
    if (task_num > m_max_task_num_expected) {
        ET_ERROR("Insufficient system state buffer");
        return;
    }

    const auto status = uxTaskGetSystemState(m_system_state.data, m_system_state.size, nullptr);
    if (status == pdFAIL) {
        ET_ERROR("Failed to get system state");
        return;
    }

    {
        std::scoped_lock lock(INTERRUPTS);
        for (const auto& task_status : m_system_state) {
            if (task_status.pcTaskName) {
                SingleEventTracer::instance()->register_user_event(
                    UserEventId::DUMP_SYSTEM_STATE, task_status.pcTaskName, &task_status);
            }
        }
    }
}

std::string_view format(const Event &e, bool newline)
{
    static char EVENT_STR[] =
        "{\"ts\":%" PRIu64 ",\"event\":%" PRIu8 ",\"ctx\":{\"task\":%" PRIu64 ",\"info\":{%s}}}%s";
    static char CTX_PRIO_STR[] = "\"prio\":%" PRIu64;
    static char CTX_MSG_STR[] = "\"msg\":\"%s\"";
    static char CTX_MRK_STR[] = "\"mark\":%" PRIu8;

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
