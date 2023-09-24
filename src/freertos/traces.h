#pragma once

#include "inttypes.h"
#include "stddef.h"

#define configUSE_TRACE_FACILITY 1
#define INCLUDE_xTaskGetCurrentTaskHandle 1

#ifndef tracerMAX_EVENT_MESSAGE_LEN
#define tracerMAX_EVENT_MESSAGE_LEN configMAX_TASK_NAME_LEN
#else
static_assert(tracerMAX_EVENT_MESSAGE_LEN >= configMAX_TASK_NAME_LEN);
#endif

#ifndef tracerTASK_ID_TYPE
#define tracerTASK_ID_TYPE decltype(TaskStatus_t::xTaskNumber)
#endif

#ifndef tracerTASK_PRIO_TYPE
#define tracerTASK_PRIO_TYPE decltype(TaskStatus_t::uxCurrentPriority)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    void trace_task_create(void *task);
    void trace_task_delete(void *task);
    void trace_task_switched_in(void *current_tcb);
    void trace_system_tick(size_t tick_count);
    void trace_malloc(void *addr, size_t size);
    void trace_free(void *addr, size_t size);

    /// @brief Alias for user-defined print function, to provide them with tracing results
    typedef int print_traces_cb_t(const char *format, ...);

    /// @brief Alias for user-defined time getter
    typedef uint64_t get_timestamp_cb_t(void);

    /// @brief Traces settings
    typedef struct
    {
        /// Pointer to buffer made for storing actual tracing data
        uint8_t *buff;
        /// Capacity of the buffer made for storing actual tracing data
        size_t capacity;
        /// Pointer to function that allows to get steady timestamps
        get_timestamp_cb_t *get_timestamp_cb;
        /// User defined provider for tracing data
        print_traces_cb_t *print_traces_cb;
        /// The size for the data traces queue, 2 is minimal value, that should be enough
        int data_queue_size;
    } TracesSettings;

    /// @brief Initiates FreeRTOS event-tracer
    /// @param settings Traces settings
    void traces_init(TracesSettings settings);

#ifdef __cplusplus
}  // extern "C"
#endif

#ifdef tracerUSE_TASK_TRACES
// #define traceMOVED_TASK_TO_READY_STATE(task)
// #define tracePOST_MOVED_TASK_TO_READY_STATE(pxTCB)
#define traceTASK_CREATE(task) trace_task_create(task)
// #define traceTASK_CREATE_FAILED(pxNewTCB)
// #define traceTASK_DELAY()
// #define traceTASK_DELAY_UNTIL()
#define traceTASK_DELETE(task) trace_task_delete(task)
// #define traceTASK_NOTIFY(uxIndexToNotify)
// #define traceTASK_NOTIFY_FROM_ISR(uxIndexToNotify)
// #define traceTASK_NOTIFY_GIVE_FROM_ISR(uxIndexToNotify)
// #define traceTASK_NOTIFY_TAKE(uxIndexToWait)
// #define traceTASK_NOTIFY_TAKE_BLOCK(uxIndexToWait)
// #define traceTASK_NOTIFY_WAIT(uxIndexToWait)
// #define traceTASK_NOTIFY_WAIT_BLOCK(uxIndexToWait)
// #define traceTASK_PRIORITY_DISINHERIT(pxTCBOfMutexHolder, uxOriginalPriority)
// #define traceTASK_PRIORITY_INHERIT(pxTCBOfMutexHolder, uxInheritedPriority)
// #define traceTASK_PRIORITY_SET(task, uxNewPriority)
// #define traceTASK_RESUME(task)
// #define traceTASK_RESUME_FROM_ISR(task)
// #define traceTASK_SUSPEND(task)
#define traceTASK_SWITCHED_IN() trace_task_switched_in(pxCurrentTCB)
// #define traceTASK_SWITCHED_OUT()
#endif

#ifdef tracerUSE_TASK_PERF
#define traceTASK_INCREMENT_TICK(tick_count) trace_system_tick((size_t)tick_count)
#endif

#ifdef tracerUSE_QUEUE_TRACES
#error Queue traces are not implemented yet
#endif

#ifdef tracerUSE_SYNCH_TRACES
#error Sync traces are not implemented yet
#endif

#ifdef tracerUSE_SBUFF_TRACES
#error Stream buffer traces are not implemented yet
#endif

#ifdef tracerUSE_TIMER_TRACES
#error Timer traces are not implemented yet
#endif
#ifdef tracerUSE_POWER_TRACES
#error Power traces are not implemented yet
#endif

#ifdef tracerUSE_ALLOC_TRACES
#define traceMALLOC(addr, size) trace_malloc(addr, size)
#define traceFREE(addr, size) trace_free(addr, size)
#endif

#ifdef tracerUSE_EGROUP_TRACES
#error Event group traces are not implemented yet
#endif
