#pragma once

#include <event.hpp>

namespace event_tracer::freertos
{

/// @brief Tracing events enumeration
enum class EventId : event_tracer::Event<>::id_t
{
    UNDEFINED = 0,
    // user defined events
    EVENT_USER_1,
    EVENT_USER_2,
    EVENT_USER_3,
    EVENT_USER_4,
    EVENT_USER_5,
    // task events
    TASK_MOVED_TO_READY_STATE,
    TASK_POST_MOVED_TO_READY_STATE,
    TASK_CREATE,
    TASK_CREATE_FAILED,
    TASK_DELAY,
    TASK_DELAY_UNTIL,
    TASK_DELETE,
    TASK_INCREMENT_TICK,
    TASK_NOTIFY,
    TASK_NOTIFY_FROM_ISR,
    TASK_NOTIFY_GIVE_FROM_ISR,
    TASK_NOTIFY_TAKE,
    TASK_NOTIFY_TAKE_BLOCK,
    TASK_NOTIFY_WAIT,
    TASK_NOTIFY_WAIT_BLOCK,
    TASK_PRIORITY_DISINHERIT,
    TASK_PRIORITY_INHERIT,
    TASK_PRIORITY_SET,
    TASK_RESUME,
    TASK_RESUME_FROM_ISR,
    TASK_SUSPEND,
    TASK_SWITCHED_IN,
    TASK_SWITCHED_OUT,
    TICK_COUNT_INCREASE,
    // queue events
    QUEUE_BLOCKING_ON_PEEK,
    QUEUE_BLOCKING_ON_RECEIVE,
    QUEUE_BLOCKING_ON_SEND,
    QUEUE_CREATE,
    QUEUE_CREATE_FAILED,
    QUEUE_DELETE,
    QUEUE_PEEK,
    QUEUE_PEEK_FAILED,
    QUEUE_PEEK_FROM_ISR,
    QUEUE_PEEK_FROM_ISR_FAILED,
    QUEUE_REGISTRY_ADD,
    QUEUE_RECEIVE,
    QUEUE_RECEIVE_FAILED,
    QUEUE_RECEIVE_FROM_ISR,
    QUEUE_RECEIVE_FROM_ISR_FAILED,
    QUEUE_SEND,
    QUEUE_SEND_FAILED,
    QUEUE_SEND_FROM_ISR,
    QUEUE_SEND_FROM_ISR_FAILED,
    // synchronisation events
    COUNTING_SEMAPHORE_CREATE,
    COUNTING_SEMAPHORE_CREATE_FAILED,
    MUTEX_CREATE,
    MUTEX_CREATE_FAILED,
    MUTEX_RECURSIVE_GIVE,
    MUTEX_RECURSIVE_GIVE_FAILED,
    MUTEX_RECURSIVE_TAKE,
    MUTEX_RECURSIVE_TAKE_FAILED,
    // stream buffer events
    STREAM_BUFFER_BLOCKING_ON_RECEIVE,
    STREAM_BUFFER_BLOCKING_ON_SEND,
    STREAM_BUFFER_CREATE,
    STREAM_BUFFER_CREATE_FAILED,
    STREAM_BUFFER_CREATE_STATIC_FAILED,
    STREAM_BUFFER_DELETE,
    STREAM_BUFFER_RECEIVE,
    STREAM_BUFFER_RECEIVE_FAILED,
    STREAM_BUFFER_RECEIVE_FROM_ISR,
    STREAM_BUFFER_RESET,
    STREAM_BUFFER_SEND,
    STREAM_BUFFER_SEND_FAILED,
    STREAM_BUFFER_SEND_FROM_ISR,
    // timer events
    PEND_FUNC_CALL,
    PEND_FUNC_CALL_FROM_ISR,
    TIMER_COMMAND_RECEIVED,
    TIMER_COMMAND_SEND,
    TIMER_CREATE,
    TIMER_CREATE_FAILED,
    TIMER_EXPIRED,
    // power management events
    LOW_POWER_IDLE_BEGIN,
    LOW_POWER_IDLE_END,
    // allocator events
    MALLOC,
    FREE,
    // event group events
    EVENT_GROUP_CLEAR_BITS,
    EVENT_GROUP_CLEAR_BITS_FROM_ISR,
    EVENT_GROUP_CREATE,
    EVENT_GROUP_CREATE_FAILED,
    EVENT_GROUP_DELETE,
    EVENT_GROUP_SET_BITS,
    EVENT_GROUP_SET_BITS_FROM_ISR,
    EVENT_GROUP_SYNC_BLOCK,
    EVENT_GROUP_WAIT_BITS_BLOCK,
};

}  // namespace event_tracer::freertos
