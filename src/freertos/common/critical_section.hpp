#pragma once

#include <prohibit_copy_move.hpp>

#include <FreeRTOS.h>
#include <task.h>

namespace event_tracer::freertos
{

/// @brief ISR critical section
class IsrPreemtion
{
public:
    IsrPreemtion() = default;
    ~IsrPreemtion() = default;

    void lock() { m_saved_interrupt_status = taskENTER_CRITICAL_FROM_ISR(); }
    void unlock() { taskEXIT_CRITICAL_FROM_ISR(m_saved_interrupt_status); }

    bool try_lock()
    {
        lock();
        return true;
    }

private:
    UBaseType_t m_saved_interrupt_status;
};

/// @brief Task critical section
class Interrupts
{
public:
    Interrupts() = default;
    ~Interrupts() = default;

    void lock() { taskENTER_CRITICAL(); }
    void unlock() { taskEXIT_CRITICAL(); }

    bool try_lock()
    {
        lock();
        return true;
    }
};

static inline IsrPreemtion ISR_PREEMPTION{};
static inline Interrupts INTERRUPTS{};

}  // namespace event_tracer::freertos
