#include <event_tracer.hpp>

using namespace event_tracer::freertos;

#ifdef __cplusplus
extern "C"
#endif
{
// the declarations of this functions are inside freertos/config/FreeRTOSTraces.h
// this is done to not bring any external dependencies to event-tracer-config library explicitly

void trace_task_create(void* task)
{
    EventTracerPtr tracer = SingleEventTracer::instance();
    if (!tracer) {
        return;
    }

    tracer->register_event(EventId::TASK_CREATE, static_cast<TaskHandle_t>(task));
}

void trace_task_delete(void* task)
{
    EventTracerPtr tracer = SingleEventTracer::instance();
    if (!tracer) {
        return;
    }

    tracer->register_event(EventId::TASK_DELETE, static_cast<TaskHandle_t>(task));
}

void trace_task_switched_in(void* current_tcb)
{
    EventTracerPtr tracer = SingleEventTracer::instance();
    if (!tracer) {
        return;
    }

    const auto timestamp = tracer->now();
    static void* previous_tcb = nullptr;
    if (previous_tcb != current_tcb) {
        tracer->register_event(EventId::TASK_SWITCHED_IN, static_cast<TaskHandle_t>(current_tcb), timestamp);
        previous_tcb = current_tcb;
    }
}

void trace_system_tick(size_t tick_count)
{
    // TODO: process profiling logic here
}

void trace_malloc([[maybe_unused]] void* addr, [[maybe_unused]] size_t size)
{
    EventTracerPtr tracer = SingleEventTracer::instance();
    if (!tracer) {
        return;
    }

    tracer->register_event(EventId::MALLOC);
}

void trace_free([[maybe_unused]] void* addr, [[maybe_unused]] size_t size)
{
    EventTracerPtr tracer = SingleEventTracer::instance();
    if (!tracer) {
        return;
    }

    tracer->register_event(EventId::FREE);
}

#ifdef __cplusplus
}  // extern "C"
#endif
