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
    , m_max_tasks_expected(settings.max_tasks_expected)
{
    Slice<std::byte> tracer_buff;
    std::tie(m_system_state, tracer_buff) = settings.buff.cut<TaskStatus_t>(m_max_tasks_expected);

    const auto data_ready_handler = [this](EventRegistry &registry, data_done_cb_t done_cb) {
        produce_message(Message{.registry = &registry, .done_cb = std::move(done_cb)});
    };

    SingleEventTracer::emplace(EventTracer::Settings{.buff = tracer_buff,
                                                     .max_tasks_expected = settings.max_tasks_expected,
                                                     .message_pool_capacity = settings.message_pool_capacity,
                                                     .data_ready_cb = data_ready_handler,
                                                     .get_time_cb = settings.get_timestamp_cb});

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
    if (task_num > m_max_tasks_expected) {
        ET_ERROR("Insufficient system state buffer");
        return;
    }

    const auto status = uxTaskGetSystemState(m_system_state.data(), m_system_state.size(), nullptr);
    if (status == pdFAIL) {
        ET_ERROR("Failed to get system state");
        return;
    }

    {
        std::scoped_lock lock(INTERRUPTS);
        for (const auto &task_status : m_system_state) {
            if (task_status.pcTaskName) {
                SingleEventTracer::instance()->register_user_event(UserEventId::DUMP_SYSTEM_STATE,
                                                                   task_status.pcTaskName, &task_status);
            }
        }
    }
}

// clang-format off

namespace
{

using ctx_info_id_t = uint8_t;

template <typename T> struct ContextInfoId      { static constexpr ctx_info_id_t value = 0; }; // undefined
template <> struct ContextInfoId<task_prio_t>   { static constexpr ctx_info_id_t value = 1; }; // task prio
template <> struct ContextInfoId<message_t>     { static constexpr ctx_info_id_t value = 2; }; // message
template <> struct ContextInfoId<ContextMarker> { static constexpr ctx_info_id_t value = 3; }; // context mark

}  // namespace

std::string_view format(const Event &e, bool newline)
{
    // event format: [ts|event_id|task|info_id:info]

    static char EVENT_STR[] = "[%" PRIu64 "|%" PRIu8 "|%" PRIu64 "|%s]%s";
    static char MSG_CTX_STR[] = "%" PRIu8 ":%s";
    static char NUM_CTX_STR[] = "%" PRIu8 ":%" PRIu64;

    static constexpr auto CTX_STR_SIZE = std::max<size_t>({
        /* task prio */ std::numeric_limits<task_prio_t>::digits10,
        /* message   */ MAX_EVENT_MESSAGE_LEN,
        /* marker    */ std::numeric_limits<ContextMarker>::digits10}) +
        /* info id   */ std::numeric_limits<ctx_info_id_t>::digits10 +
                        std::max({
                            sizeof(MSG_CTX_STR),
                            sizeof(NUM_CTX_STR)});

    static constexpr auto EVENT_STR_SIZE = std::numeric_limits<uint64_t>::digits10 +
                                           std::numeric_limits<uint8_t>::digits10 +
                                           std::numeric_limits<uint64_t>::digits10 +
                                           std::numeric_limits<uint8_t>::digits10 +
                                           CTX_STR_SIZE +
                                           sizeof(EVENT_STR) +
                                           sizeof("\n");

    static char event_str[EVENT_STR_SIZE];
    static char ctx_str[CTX_STR_SIZE];

    std::visit(Alternatives{
        [&](task_prio_t prio) {
            std::snprintf(ctx_str, CTX_STR_SIZE, NUM_CTX_STR, ContextInfoId<task_prio_t>::value, prio);
        },
        [&](message_t msg) {
            char* message = nullptr;
            {
                std::scoped_lock lock(INTERRUPTS);

                EventTracerPtr tracer = SingleEventTracer::instance();
                ET_ASSERT(tracer);

                message = msg.get(tracer->access_message_alloc());
            }
            std::snprintf(ctx_str, CTX_STR_SIZE, MSG_CTX_STR, ContextInfoId<message_t>::value, message);
        },
        [&](ContextMarker mark) {
            std::snprintf(ctx_str, CTX_STR_SIZE, NUM_CTX_STR, ContextInfoId<ContextMarker>::value, mark);
        }}, e.ctx.info);

    std::snprintf(event_str, EVENT_STR_SIZE, EVENT_STR, e.ts, e.id, e.ctx.task_id, ctx_str, newline ? "\n" : "");
    return event_str;
}

// clang-format on

}  // namespace event_tracer::freertos
