#pragma once

#include "stddef.h"

/// @file Extern tracing functions to not bring any dependencies here,
///       the definitions are in freertos/backend/traces.cpp

#ifdef __cplusplus
extern "C"
{
#endif

// Task context
void trace_task_create(void *task);
void trace_task_delete(void *task);
void trace_malloc(void *addr, size_t size);
void trace_free(void *addr, size_t size);

// ISR context
void ISR_trace_task_switched_in(void *current_tcb);
void ISR_trace_system_tick(size_t tick_count);

#ifdef __cplusplus
}  // extern "C"
#endif
