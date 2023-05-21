#pragma once

#include "inttypes.h"
#include "stddef.h"

#define configUSE_TRACE_FACILITY          1
#define INCLUDE_xTaskGetCurrentTaskHandle 1

#ifdef USE_TASK_TRACES
    #include "task_traces.h"
#endif

#ifdef USE_ALLOCATOR_TRACES
    #include "alloc_traces.h"
#endif

#ifdef __cplusplus
    extern "C" {
#endif

/// @brief Alias for user-defined print function, to provide them with tracing results
typedef int xPrintTraces (const char *pszFormat, ...);

/// @brief Alias for user-defined time getter
typedef uint64_t uxGetTime (void);

/// @brief Initiates FreeRTOS event-tracer
/// @param puBuff Pointer to buffer made for storing actual tracing data
/// @param uxCapasity Capacity of the buffer made for storing actual tracing data
/// @param pfnGetSteadyTimestamp Pointer to function that allows to get steady timestamps
/// @param pfnDataOutputMethod User defined provider for tracing data
/// @param pfnMetaOutputMethod User defined provider for tracing meta data (task names, associated with IDs, etc.)
void vTracesInit(uint8_t *puBuff,
                 size_t uxCapasity,
                 uxGetTime *pfnGetSteadyTimestamp,
                 xPrintTraces *pfnDataOutputMethod,
                 xPrintTraces *pfnMetaOutputMethod);

#ifdef __cplusplus
    } // extern "C"
#endif
