#pragma once

#include "inttypes.h"
#include "stddef.h"

#ifndef tracerTASK_NAME
#define tracerTASK_NAME "event_tracer"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/// @brief Alias for user-defined print function, to provide them with tracing results
typedef int print_traces_cb_t(const char *format, ...);

/// @brief Alias for user-defined time getter
typedef uint64_t get_timestamp_cb_t(void);

/// @brief Traces settings
typedef struct
{
    /// Pointer to buffer made for storing actual tracing data
    uint8_t *buff;
    /// Capacity of the buffer made for storing actual tracing data
    size_t capacity;
    /// Pointer to function that allows to get steady timestamps
    get_timestamp_cb_t *get_timestamp_cb;
    /// User defined provider for tracing data
    print_traces_cb_t *print_traces_cb;
    /// The size for the data traces queue, 2 is minimal value, that should be enough
    int data_queue_size;
    /// Data traces queue polling interval
    size_t polling_interval_ms;
} TracesSettings;

/// @brief Initiates FreeRTOS event-tracer
/// @param settings Traces settings
void traces_init(TracesSettings settings);

#ifdef __cplusplus
}  // extern "C"
#endif
