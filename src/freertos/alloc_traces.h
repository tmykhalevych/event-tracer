#pragma once

#include "stddef.h"

#ifdef __cplusplus
    extern "C" {
#endif

void vTraceMalloc(void *pvAddress, size_t uiSize);
void vTraceFree(void *pvAddress, size_t uiSize);

#ifdef __cplusplus
    } // extern "C"
#endif

#define traceMALLOC(pvAddress, uiSize) vTraceMalloc(pvAddress, uiSize)
#define traceFREE(pvAddress, uiSize) vTraceFree(pvAddress, uiSize)
