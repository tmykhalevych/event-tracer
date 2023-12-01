#pragma once

#include "FreeRTOSTraces.h"

#define configUSE_TRACE_FACILITY 1
#define INCLUDE_xTaskGetCurrentTaskHandle 1
#define INCLUDE_vTaskSuspend 1

#ifdef tracerUSE_TASK_TRACES
#define traceTASK_CREATE(task) trace_task_create(task)
#define traceTASK_DELETE(task) trace_task_delete(task)
#define traceTASK_SWITCHED_IN() trace_task_switched_in(pxCurrentTCB)
#endif

#ifdef tracerUSE_TASK_PERF
#define traceTASK_INCREMENT_TICK(tick_count) trace_system_tick((size_t)tick_count)
#endif

#ifdef tracerUSE_ALLOC_TRACES
#define traceMALLOC(addr, size) trace_malloc(addr, size)
#define traceFREE(addr, size) trace_free(addr, size)
#endif
