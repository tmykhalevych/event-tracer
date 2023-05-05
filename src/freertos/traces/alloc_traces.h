#pragma once

#include "stddef.h"

extern void vTraceMalloc(void *pvAddress, size_t uiSize);
extern void vTraceFree(void *pvAddress, size_t uiSize);

#define traceMALLOC(pvAddress, uiSize) vTraceMalloc(pvAddress, uiSize)
#define traceFREE(pvAddress, uiSize) vTraceFree(pvAddress, uiSize)
