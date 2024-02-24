#pragma once

#include <prohibit_copy_move.hpp>

#include <FreeRTOS.h>
#include <task.h>

namespace event_tracer::freertos
{

/// @brief ISR critical section
/// @note Nesting is handled by FreeRTOS port implementation
class IsrPreemtion
{
public:
    void lock() { m_saved_interrupt_status = taskENTER_CRITICAL_FROM_ISR(); }
    void unlock() { taskEXIT_CRITICAL_FROM_ISR(m_saved_interrupt_status); }

private:
    UBaseType_t m_saved_interrupt_status;
};

/// @brief Task critical section
/// @note Nesting is handled by FreeRTOS port implementation
class Interrupts
{
public:
    void lock() { taskENTER_CRITICAL(); }
    void unlock() { taskEXIT_CRITICAL(); }
};

static inline IsrPreemtion ISR_PREEMPTION{};
static inline Interrupts INTERRUPTS{};

}  // namespace event_tracer::freertos
