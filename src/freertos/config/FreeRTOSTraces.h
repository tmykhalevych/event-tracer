#pragma once

#include "stddef.h"

// extern tracing functions to not bring any dependencies here, the definitions are in freertos/backend/traces.cpp
void trace_task_create(void *task);
void trace_task_delete(void *task);
void trace_task_switched_in(void *current_tcb);
void trace_system_tick(size_t tick_count);
void trace_malloc(void *addr, size_t size);
void trace_free(void *addr, size_t size);
