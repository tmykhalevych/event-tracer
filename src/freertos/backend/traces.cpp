#include <critical_section.hpp>
#include <event_tracer.hpp>

#include <mutex>

/// @file The declarations of this functions are inside freertos/config/FreeRTOSTraces.h
///       This is done to not bring any external dependencies to freertos-tracer-config library explicitly

using namespace event_tracer::freertos;

#ifdef __cplusplus
extern "C"
{
#endif

void trace_task_create(void* task)
{
    std::scoped_lock lock(INTERRUPTS);

    EventTracerPtr tracer = SingleEventTracer::instance();
    if (!tracer) {
        return;
    }

    tracer->register_event(EventId::TASK_CREATE, static_cast<TaskHandle_t>(task));
}

void trace_task_delete(void* task)
{
    std::scoped_lock lock(INTERRUPTS);

    EventTracerPtr tracer = SingleEventTracer::instance();
    if (!tracer) {
        return;
    }

    tracer->register_event(EventId::TASK_DELETE, static_cast<TaskHandle_t>(task));
}

void trace_malloc([[maybe_unused]] void* addr, [[maybe_unused]] size_t size)
{
    std::scoped_lock lock(INTERRUPTS);

    EventTracerPtr tracer = SingleEventTracer::instance();
    if (!tracer) {
        return;
    }

    tracer->register_event(EventId::MALLOC);
}

void trace_free([[maybe_unused]] void* addr, [[maybe_unused]] size_t size)
{
    std::scoped_lock lock(INTERRUPTS);

    EventTracerPtr tracer = SingleEventTracer::instance();
    if (!tracer) {
        return;
    }

    tracer->register_event(EventId::FREE);
}

void ISR_trace_task_switched_in(void* current_tcb)
{
    std::scoped_lock lock(ISR_PREEMPTION);

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

void ISR_trace_system_tick(size_t tick_count)
{
    std::scoped_lock lock(ISR_PREEMPTION);

    // TODO: process profiling logic here
}

#ifdef __cplusplus
}  // extern "C"
#endif
