#pragma once

#include "inttypes.h"
#include "stddef.h"

#define configUSE_TRACE_FACILITY 1
#define INCLUDE_xTaskGetCurrentTaskHandle 1

#ifdef __cplusplus
extern "C"
{
#endif

    void vTraceTaskCreate(void *xTask);
    void vTraceTaskDelete(void *xTask);
    void vTraceTaskSwitchedIn(void *pxCurrentTCB);
    void vTraceSystemTick(size_t uiTickCount);
    void vTraceMalloc(void *pvAddress, size_t uiSize);
    void vTraceFree(void *pvAddress, size_t uiSize);

    /// @brief Alias for user-defined print function, to provide them with tracing results
    typedef int xPrintTraces(const char *pszFormat, ...);

    /// @brief Alias for user-defined time getter
    typedef uint64_t uxGetTime(void);

    /// @brief Initiates FreeRTOS event-tracer
    /// @param puBuff Pointer to buffer made for storing actual tracing data
    /// @param uxCapasity Capacity of the buffer made for storing actual tracing data
    /// @param pfnGetSteadyTimestamp Pointer to function that allows to get steady timestamps
    /// @param pfnOutputMethod User defined provider for tracing data
    void vTracesInit(uint8_t *puBuff, size_t uxCapasity, uxGetTime *pfnGetSteadyTimestamp,
                     xPrintTraces *pfnOutputMethod);

#ifdef __cplusplus
}  // extern "C"
#endif

#ifdef tracerUSE_TASK_TRACES
// #define traceMOVED_TASK_TO_READY_STATE(xTask)
// #define tracePOST_MOVED_TASK_TO_READY_STATE(pxTCB)
#define traceTASK_CREATE(xTask) vTraceTaskCreate(xTask)
// #define traceTASK_CREATE_FAILED(pxNewTCB)
// #define traceTASK_DELAY()
// #define traceTASK_DELAY_UNTIL()
#define traceTASK_DELETE(xTask) vTraceTaskDelete(xTask)
// #define traceTASK_NOTIFY(uxIndexToNotify)
// #define traceTASK_NOTIFY_FROM_ISR(uxIndexToNotify)
// #define traceTASK_NOTIFY_GIVE_FROM_ISR(uxIndexToNotify)
// #define traceTASK_NOTIFY_TAKE(uxIndexToWait)
// #define traceTASK_NOTIFY_TAKE_BLOCK(uxIndexToWait)
// #define traceTASK_NOTIFY_WAIT(uxIndexToWait)
// #define traceTASK_NOTIFY_WAIT_BLOCK(uxIndexToWait)
// #define traceTASK_PRIORITY_DISINHERIT(pxTCBOfMutexHolder, uxOriginalPriority)
// #define traceTASK_PRIORITY_INHERIT(pxTCBOfMutexHolder, uxInheritedPriority)
// #define traceTASK_PRIORITY_SET(xTask, uxNewPriority)
// #define traceTASK_RESUME(xTask)
// #define traceTASK_RESUME_FROM_ISR(xTask)
// #define traceTASK_SUSPEND(xTask)
#define traceTASK_SWITCHED_IN() vTraceTaskSwitchedIn(pxCurrentTCB)
// #define traceTASK_SWITCHED_OUT()
#endif

#ifdef tracerUSE_TASK_PERF
#define traceTASK_INCREMENT_TICK(xTickCount) vTraceSystemTick((size_t)xTickCount)
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
#define traceMALLOC(pvAddress, uiSize) vTraceMalloc(pvAddress, uiSize)
#define traceFREE(pvAddress, uiSize) vTraceFree(pvAddress, uiSize)
#endif

#ifdef tracerUSE_EGROUP_TRACES
#error Event group traces are not implemented yet
#endif
