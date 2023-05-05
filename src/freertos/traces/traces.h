#pragma once

#include "inttypes.h"
#include "stddef.h"

#ifdef USE_TASK_TRACES
    #include "task_traces.h"
#endif

#ifdef USE_ALLOCATOR_TRACES
    #include "alloc_traces.h"
#endif

/// @brief Alias for user-defined print function, to provide them with tracing results
typedef int xPrintTraces (const char *pszFormat, ...);

/// @brief 
/// @param puBuff 
/// @param uxCapasity 
extern "C" void vTracesInit(uint8_t *puBuff, size_t uxCapasity);

/// @brief 
/// @param pfnDataOutputMethod 
extern "C" void vTracesSetDataOutputMethod(xPrintTraces *pfnDataOutputMethod);

/// @brief 
/// @param pfnMetaOutputMethod 
extern "C" void vTracesSetMetaOutputMethod(xPrintTraces *pfnMetaOutputMethod);
