#pragma once

#include "stdint.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void vTraceTaskCreate(void *xTask);
    void vTraceTaskDelete(void *xTask);
    void vTraceTaskSwitchedIn(void *pxCurrentTCB);

#ifdef __cplusplus
}  // extern "C"
#endif

// #define traceMOVED_TASK_TO_READY_STATE(xTask)
// #define tracePOST_MOVED_TASK_TO_READY_STATE(pxTCB)
#define traceTASK_CREATE(xTask) vTraceTaskCreate(xTask)
// #define traceTASK_CREATE_FAILED(pxNewTCB)
// #define traceTASK_DELAY()
// #define traceTASK_DELAY_UNTIL()
#define traceTASK_DELETE(xTask) vTraceTaskDelete(xTask)
// #define traceTASK_NOTIFY(uxIndexToNotify)
// #define traceTASK_NOTIFY_FROM_ISR(uxIndexToNotify)
// #define traceTASK_NOTIFY_GIVE_FROM_ISR(uxIndexToNotify)
// #define traceTASK_NOTIFY_TAKE(uxIndexToWait)
// #define traceTASK_NOTIFY_TAKE_BLOCK(uxIndexToWait)
// #define traceTASK_NOTIFY_WAIT(uxIndexToWait)
// #define traceTASK_NOTIFY_WAIT_BLOCK(uxIndexToWait)
// #define traceTASK_PRIORITY_DISINHERIT(pxTCBOfMutexHolder, uxOriginalPriority)
// #define traceTASK_PRIORITY_INHERIT(pxTCBOfMutexHolder, uxInheritedPriority)
// #define traceTASK_PRIORITY_SET(xTask, uxNewPriority)
// #define traceTASK_RESUME(xTask)
// #define traceTASK_RESUME_FROM_ISR(xTask)
// #define traceTASK_SUSPEND(xTask)
#define traceTASK_SWITCHED_IN() vTraceTaskSwitchedIn(pxCurrentTCB)
// #define traceTASK_SWITCHED_OUT()
