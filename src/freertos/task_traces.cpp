#include <FreeRTOS.h>
#include <event.hpp>
#include <event_tracer.hpp>
#include <task_traces.h>

using namespace event_tracer::freertos;

extern "C" void vTraceTaskCreate(void *xTask)
{
    // TODO: implement
}

extern "C" void vTraceTaskDelete(void *xTask)
{
    // TODO: implement
}

extern "C" void vTraceTaskSwitchedIn(void *pxCurrentTCB)
{
    const auto timestamp = tracer().now();
    static void *pxPreviousTCB = nullptr;
    if (pxPreviousTCB != pxCurrentTCB) {
        tracer().register_event(Event::TASK_SWITCHED_IN, timestamp);
        pxPreviousTCB = pxCurrentTCB;
    }
}
