#pragma once

#include "stddef.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void vTraceSystemTick(size_t uiTickCount);

#ifdef __cplusplus
}  // extern "C"
#endif

#define traceTASK_INCREMENT_TICK(xTickCount) vTraceSystemTick((size_t)xTickCount)
