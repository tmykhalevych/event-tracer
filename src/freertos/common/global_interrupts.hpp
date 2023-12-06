#pragma once

#include <prohibit_copy_move.hpp>

#include <FreeRTOS.h>
#include <task.h>

namespace event_tracer::freertos
{

class GlobalInterrupts
{
public:
    GlobalInterrupts() = default;
    ~GlobalInterrupts() = default;

    void lock() { taskENTER_CRITICAL(); }
    void unlock() { taskEXIT_CRITICAL(); }

    bool try_lock()
    {
        lock();
        return true;
    }
};

static inline GlobalInterrupts GLOBAL_INTERRUPTS{};

}  // namespace event_tracer::freertos
