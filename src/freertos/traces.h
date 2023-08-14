#pragma once

#include "inttypes.h"
#include "stddef.h"

#define configUSE_TRACE_FACILITY 1
#define INCLUDE_xTaskGetCurrentTaskHandle 1

#ifdef tracerUSE_TASK_TRACES
#include "task_traces.h"
#endif
#ifdef tracerUSE_TASK_PERF
#include "task_perf.h"
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
#include "alloc_traces.h"
#endif
#ifdef tracerUSE_EGROUP_TRACES
#error Event group traces are not implemented yet
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    /// @brief Alias for user-defined print function, to provide them with tracing results
    typedef int xPrintTraces(const char* pszFormat, ...);

    /// @brief Alias for user-defined time getter
    typedef uint64_t uxGetTime(void);

    /// @brief Initiates FreeRTOS event-tracer
    /// @param puBuff Pointer to buffer made for storing actual tracing data
    /// @param uxCapasity Capacity of the buffer made for storing actual tracing data
    /// @param pfnGetSteadyTimestamp Pointer to function that allows to get steady timestamps
    /// @param pfnOutputMethod User defined provider for tracing data
    void vTracesInit(uint8_t* puBuff, size_t uxCapasity, uxGetTime* pfnGetSteadyTimestamp,
                     xPrintTraces* pfnOutputMethod);

#ifdef __cplusplus
}  // extern "C"
#endif
