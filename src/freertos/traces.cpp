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

    void vTraceTaskCreate(void* xTask)
    {
        tracer().register_event(Event::TASK_CREATE, static_cast<TaskHandle_t>(xTask));
    }

    void vTraceTaskDelete(void* xTask)
    {
        tracer().register_event(Event::TASK_DELETE, static_cast<TaskHandle_t>(xTask));
    }

    void vTraceTaskSwitchedIn(void* pxCurrentTCB)
    {
        const auto timestamp = tracer().now();
        static void* pxPreviousTCB = nullptr;
        if (pxPreviousTCB != pxCurrentTCB) {
            tracer().register_event(Event::TASK_SWITCHED_IN, static_cast<TaskHandle_t>(pxCurrentTCB), timestamp);
            pxPreviousTCB = pxCurrentTCB;
        }
    }

    void vTraceSystemTick(size_t uiTickCount)
    {
        // TODO: implement
    }

    void vTraceMalloc([[maybe_unused]] void* pvAddress, [[maybe_unused]] size_t uiSize)
    {
        tracer().register_event(Event::MALLOC);
    }

    void vTraceFree([[maybe_unused]] void* pvAddress, [[maybe_unused]] size_t uiSize)
    {
        tracer().register_event(Event::FREE);
    }

    void vTracesInit(uint8_t* puBuff, size_t uxCapasity, uxGetTime* pfnGetSteadyTimestamp,
                     xPrintTraces* pfnOutputMethod)
    {
        struct EventTracerContext
        {
            xPrintTraces* data_cb;
        };

        struct DataReadyMessage
        {
            EventRegistry* registry = nullptr;
            EventTracer::data_done_cb_t done_cb;
        };

        /// the size for the data traces queue, 2 is minimal value, that should be enough
        static constexpr auto DATA_REGISTRY_QUEUE_SIZE = 2;
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

        static EventTracer tracer(reinterpret_cast<std::byte*>(puBuff), uxCapasity, data_ready_handler);
        EventTracer::set_single_instance(&tracer);

        tracer.set_time_getter(pfnGetSteadyTimestamp);
        data_ready_queue = xQueueCreate(DATA_REGISTRY_QUEUE_SIZE, sizeof(DataReadyMessage));

        static EventTracerContext context{.data_cb = pfnOutputMethod};
        xTaskCreate(tracer_task, "event_tracer", configMINIMAL_STACK_SIZE, &context, (configTIMER_TASK_PRIORITY - 1),
                    nullptr);
    }

#ifdef __cplusplus
}  // extern "C"
#endif
