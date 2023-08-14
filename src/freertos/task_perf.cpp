#include <FreeRTOS.h>
#include <event.hpp>
#include <event_tracer.hpp>
#include <task_perf.h>

/// @brief Retrieves pc register from stored thread context, specific to platform and/or compiler
/// @param pvContext Pointer to stored thread context
/// @note Expected to be defined by user
extern "C" portPOINTER_SIZE_TYPE portRETRIEVE_PROGRAM_COUNTER(void *pvContext);

extern "C" void vTraceSystemTick(size_t uiTickCount)
{
    // TODO: implement
}
