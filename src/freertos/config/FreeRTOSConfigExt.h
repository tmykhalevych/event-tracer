#pragma once

#include "stddef.h"

// extern tracing functions to not bring any dependencies here, the declarations are in freertos/backend/traces.cpp
void trace_task_create(void *task);
void trace_task_delete(void *task);
void trace_task_switched_in(void *current_tcb);
void trace_system_tick(size_t tick_count);
void trace_malloc(void *addr, size_t size);
void trace_free(void *addr, size_t size);

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
