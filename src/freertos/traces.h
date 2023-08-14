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
#include "queue_traces.h"
#endif
#ifdef tracerUSE_SYNCH_TRACES
#include "synch_traces.h"
#endif
#ifdef tracerUSE_SBUFF_TRACES
#include "sbuff_traces.h"
#endif
#ifdef tracerUSE_TIMER_TRACES
#include "timer_traces.h"
#endif
#ifdef tracerUSE_POWER_TRACES
#include "power_traces.h"
#endif
#ifdef tracerUSE_ALLOC_TRACES
#include "alloc_traces.h"
#endif
#ifdef tracerUSE_EGROUP_TRACES
#include "egroup_traces.h"
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
