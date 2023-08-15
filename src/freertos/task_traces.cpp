#include <FreeRTOS.h>
#include <event.hpp>
#include <event_tracer.hpp>
#include <task_traces.h>

using namespace event_tracer::freertos;

extern "C" void vTraceTaskCreate(void *xTask)
{
    tracer().register_event(Event::TASK_CREATE, static_cast<TaskHandle_t>(xTask));
}

extern "C" void vTraceTaskDelete(void *xTask)
{
    tracer().register_event(Event::TASK_DELETE, static_cast<TaskHandle_t>(xTask));
}

extern "C" void vTraceTaskSwitchedIn(void *pxCurrentTCB)
{
    const auto timestamp = tracer().now();
    static void *pxPreviousTCB = nullptr;
    if (pxPreviousTCB != pxCurrentTCB) {
        tracer().register_event(Event::TASK_SWITCHED_IN, static_cast<TaskHandle_t>(pxCurrentTCB), timestamp);
        pxPreviousTCB = pxCurrentTCB;
    }
}
