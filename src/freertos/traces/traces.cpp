#include <traces.h>
#include <event_tracer.hpp>
#include <assert.hpp>
#include <error.hpp>

#include <FreeRTOS.h>
#include <queue.h>

#include <string_view>
#include <sstream>

namespace
{

/// @brief The size for the data traces queue, 2 is minimal value, that should be enough
static constexpr auto DATA_REGISTRY_QUEUE_SIZE = 2;

} // namespace

extern "C" void vTracesInit(uint8_t *puBuff,
                            size_t uxCapasity,
                            uxGetTime *pfnGetSteadyTimestamp,
                            xPrintTraces *pfnDataOutputMethod,
                            xPrintTraces *pfnMetaOutputMethod)
{
    using namespace event_tracer::freertos;

    auto data_output_cb = [pfnDataOutputMethod](std::string_view data) { pfnDataOutputMethod(data.data()); };
    auto meta_output_cb = [pfnMetaOutputMethod](std::string_view meta) { pfnMetaOutputMethod(meta.data()); };

    struct EventTracerContext
    {
        using data_callback_type = decltype(data_output_cb);
        using meta_callback_type = decltype(meta_output_cb);

        data_callback_type data_cb;
        meta_callback_type meta_cb;
    };

    struct DataReadyMessage
    {
        EventRegistry* registry = nullptr;
        EventTracer::data_done_cb_t done_cb;
    };

    // the queue is instanciated after evebt tracer to prevent traces when tracer is not ready
    static QueueHandle_t data_ready_queue = nullptr;

    const auto tracer_task = [](void* context)
    {
        assert(context);
        assert(data_ready_queue);

        auto &callbacks = *reinterpret_cast<EventTracerContext*>(context);
        DataReadyMessage msg;

        while (true) {
            if (xQueueReceive(data_ready_queue, &msg, portMAX_DELAY)) {
                assert(msg.registry);
                for (const auto& event : *msg.registry) {
                    std::ostringstream oss;
                    oss << "Event [ts:" << event.ts << ", id:" << event.id << ", task(id:" << event.ctx.id << ", prio: " << event.ctx.prio << ")]";
                    callbacks.data_cb(oss.str().c_str());
                }
                // notify event tracer that we're done handling the data
                msg.done_cb();
            }
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        }
    };

    const auto data_ready_handler = [](EventRegistry& registry, EventTracer::data_done_cb_t done_cb)
    {
        assert(data_ready_queue);

        DataReadyMessage msg {
            .registry = &registry,
            .done_cb = done_cb
        };

        if (xQueueSend(data_ready_queue, &msg, 0 /* don't wait */) == pdPASS) {
            error("Failed to send tracing data");
        }
    };

    static EventTracer tracer(reinterpret_cast<std::byte*>(puBuff), uxCapasity, data_ready_handler);
    EventTracer::set_single_instance(&tracer);

    tracer.set_time_getter(pfnGetSteadyTimestamp);
    data_ready_queue = xQueueCreate(DATA_REGISTRY_QUEUE_SIZE, sizeof(DataReadyMessage));

    static EventTracerContext context {
        .data_cb = std::move(data_output_cb),
        .meta_cb = std::move(meta_output_cb)
    };

    xTaskCreate(tracer_task, "event_tracer", configMINIMAL_STACK_SIZE, &context, (configTIMER_TASK_PRIORITY - 1), nullptr);
}
