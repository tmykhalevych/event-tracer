#include <assert.hpp>
#include <error.hpp>
#include <event_tracer.hpp>
#include <traces.h>

#include <FreeRTOS.h>
#include <queue.h>

#include <string_view>

using namespace event_tracer::freertos;

#ifdef __cplusplus
extern "C"
{
#endif

    void trace_task_create(void* task) { tracer().register_event(Event::TASK_CREATE, static_cast<TaskHandle_t>(task)); }

    void trace_task_delete(void* task) { tracer().register_event(Event::TASK_DELETE, static_cast<TaskHandle_t>(task)); }

    void trace_task_switched_in(void* current_tcb)
    {
        const auto timestamp = tracer().now();
        static void* previous_tcb = nullptr;
        if (previous_tcb != current_tcb) {
            tracer().register_event(Event::TASK_SWITCHED_IN, static_cast<TaskHandle_t>(current_tcb), timestamp);
            previous_tcb = current_tcb;
        }
    }

    void trace_system_tick(size_t tick_count)
    {
        // TODO: implement
    }

    void trace_malloc([[maybe_unused]] void* addr, [[maybe_unused]] size_t size)
    {
        tracer().register_event(Event::MALLOC);
    }

    void trace_free([[maybe_unused]] void* addr, [[maybe_unused]] size_t size) { tracer().register_event(Event::FREE); }

    void traces_init(TracesSettings settings)
    {
        struct EventTracerContext
        {
            print_traces_cb_t* data_cb;
        };

        struct DataReadyMessage
        {
            EventRegistry* registry = nullptr;
            EventTracer::data_done_cb_t done_cb;
        };

        // the queue is instanciated after event tracer to prevent traces when tracer is not ready
        static QueueHandle_t data_ready_queue = nullptr;

        const auto tracer_task = [](void* context) {
            ET_ASSERT(context);
            ET_ASSERT(data_ready_queue);

            auto& callbacks = *reinterpret_cast<EventTracerContext*>(context);
            DataReadyMessage msg;

            while (true) {
                if (xQueueReceive(data_ready_queue, &msg, portMAX_DELAY)) {
                    ET_ASSERT(msg.registry);
                    for (const auto& event : *msg.registry) {
                        callbacks.data_cb(format(event).data());
                    }
                    // notify event tracer that we're done handling the data
                    msg.done_cb();
                }
            }
        };

        const auto data_ready_handler = [](EventRegistry& registry, EventTracer::data_done_cb_t done_cb) {
            ET_ASSERT(data_ready_queue);

            DataReadyMessage msg{.registry = &registry, .done_cb = done_cb};

            if (xQueueSend(data_ready_queue, &msg, 0 /* don't wait */) != pdPASS) {
                ET_ERROR("Failed to send tracing data");
            }
        };

        static EventTracerContext context{.data_cb = settings.print_traces_cb};

        const auto msg_handler = [](const MessageEventDesk& msg_event) {
            ET_ASSERT(context.data_cb);
            context.data_cb(format(msg_event).data());
        };

        static EventTracer tracer(reinterpret_cast<std::byte*>(settings.buff), settings.capacity, data_ready_handler,
                                  msg_handler, settings.get_timestamp_cb);

        EventTracer::set_single_instance(&tracer);

        data_ready_queue = xQueueCreate(settings.data_queue_size, sizeof(DataReadyMessage));

        xTaskCreate(tracer_task, "event_tracer", configMINIMAL_STACK_SIZE, &context, (configTIMER_TASK_PRIORITY - 1),
                    nullptr);
    }

#ifdef __cplusplus
}  // extern "C"
#endif
