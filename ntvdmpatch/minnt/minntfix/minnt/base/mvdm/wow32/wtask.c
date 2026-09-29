/*++
 *
 *  WOW v1.0
 *
 *  Copyright (c) 1991, Microsoft Corporation
 *
 *  WTASK.C
 *  WOW32 Task management routines for x64
 *
 *  History:
 *  Created 08-Dec-2020 by leecher1337
--*/


#include "precomp.h"
#pragma hdrstop

#ifndef PM_QS_SENDMESSAGE
#define PM_QS_SENDMESSAGE (QS_SENDMESSAGE << 16) 
#endif 

extern PTD     gptdTaskHead;               // Linked List of TDs
extern CRITICAL_SECTION gcsWOW;            // WOW Critical Section used when updating task linked list 

#ifdef W10
/***********************************************************************
 *           OldYield  (KERNEL.117)
 */
void WINAPI OldYield(void)
{
    DWORD count;
    
    LOGDEBUG(LOG_TRACE,("OldYield\n"));
    ReleaseThunkLock(&count);
    RestoreThunkLock(count);
}


/***********************************************************************
 *           DirectedYield  (KERNEL.150)
 */
#define MAX_DIRECTEDYIELD_WAITERS 64

void WINAPI xxxDirectedYield( DWORD dwThreadId )
{
    PTD chdthd, ptd;
    BOOL failed = FALSE;
    DWORD dwThreadID = GetCurrentThreadId(), count;
    PTD    aptdWaiters[MAX_DIRECTEDYIELD_WAITERS];
    HANDLE ahWaiters[MAX_DIRECTEDYIELD_WAITERS];
    int    cWaiters = 0, i;
#ifdef DEBUG
	register PVDMFRAME pFrame;

    ptd = CURRENTPTD();
	GETFRAMEPTR(ptd->vpStack, pFrame);
#endif

    LOGDEBUG(LOG_TRACE,("%04X          xxxDirectedYield(%d)\n", pFrame->wTDB, dwThreadId));

    //
    // Everything from the task lookup through creating the yield/
    // yield_wait events has to happen as one atomic step under gcsWOW.
    // Without this, two overlapping xxxDirectedYield calls (from two
    // different threads) can each see "nothing pending" in the scan
    // below and then both go on to CreateEvent() a fresh yield_wait_event
    // for the same target ptd -- whichever call wins the race silently
    // clobbers the other's handle in ptd->yield_wait_event. If a thread
    // is already inside _EnterSysLevel's WaitForSingleObject on the
    // handle that just got orphaned that way, nothing will ever signal
    // it again and that thread hangs forever. This manifested as an
    // intermittent WOWEXEC hang after closing WinHelp (opened from
    // Write): the affected thread enters _EnterSysLevel's wait and
    // never returns.
    //
    // Because _EnterSysLevel (wsyslevel.c) now also atomically captures
    // and clears ptd->yield_wait_event under the same lock (so a later
    // xxxDirectedYield call can safely reuse that ptd once it's been
    // consumed, and so a *stale* handle can never be waited on twice),
    // this function can no longer rely on re-reading
    // ptd->yield_wait_event during cleanup to know what to signal -- by
    // then it may already have been claimed and nulled out by the
    // target thread's own _EnterSysLevel call. So each handle this
    // function creates is remembered locally (aptdWaiters/ahWaiters) and
    // signaled from that local copy instead.
    //
    EnterCriticalSection(&gcsWOW);

    for (chdthd = gptdTaskHead; chdthd && chdthd->dwThreadID != dwThreadID; chdthd = chdthd->ptdNext);

    if (!chdthd)
    {
        LeaveCriticalSection(&gcsWOW);
        OldYield();
        return;
    }
    if (chdthd->yield_event)
    {
        LeaveCriticalSection(&gcsWOW);
        LOGDEBUG(LOG_WARNING,("nested DirectedYield doesnt work.\n"));
        OldYield();
        return;
    }
    for (ptd = gptdTaskHead; ptd; ptd = ptd->ptdNext)
    {
        if (ptd->yield_event)
        {
            if (ptd->dwThreadID == dwThreadID)
            {
                LeaveCriticalSection(&gcsWOW);
                ReleaseThunkLock(&count);
                Sleep(10);
                RestoreThunkLock(count);
                return;
            }
            failed = TRUE;
        }
        if (ptd->yield_wait_event)
        {
            LeaveCriticalSection(&gcsWOW);
            return;
        }
    }
    if (failed)
    {
        LeaveCriticalSection(&gcsWOW);
        LOGDEBUG(LOG_WARNING,("nested DirectedYield doesnt work.\n"));
        OldYield();
        return;
    }
    chdthd->yield_event = CreateEvent(NULL, TRUE, FALSE, NULL);
    for (ptd = gptdTaskHead; ptd; ptd = ptd->ptdNext)
    {
        if (ptd->dwThreadID != dwThreadID)
        {
            HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

            ptd->yield_wait_event = hEvent;
            if (cWaiters < MAX_DIRECTEDYIELD_WAITERS)
            {
                aptdWaiters[cWaiters] = ptd;
                ahWaiters[cWaiters] = hEvent;
                cWaiters++;
            }
        }
    }

    LeaveCriticalSection(&gcsWOW);

    ReleaseThunkLock(&count);

    /*
     * In win16, if hTask doesn't wait events, another task will be executed.
     * Here, wait until timeout.
     */
    WaitForSingleObject(chdthd->yield_event, 100);
    RestoreThunkLock(count);
    CloseHandle(chdthd->yield_event);
    chdthd->yield_event = NULL;

    for (i = 0; i < cWaiters; i++)
    {
        LOGDEBUG(LOG_TRACE,("%04X          xxxDirectedYield setting yield_wait_event on TID %d\n", pFrame->wTDB, aptdWaiters[i]->dwThreadID));
        SetEvent(ahWaiters[i]);
    }
    LOGDEBUG(LOG_TRACE,("%04X          xxxDirectedYield done\n", pFrame->wTDB));
}

BOOL    xxxUserYield(VOID)
{
    /* Windows 10 x64 Scheduler doesn't support 16bit tasks. :(
     *
     * NtUserYieldTask -> xxxUserYield just does xxxReceiveMessage, but doesn't
     * contain WOW16 task handling (i.e. xxxSleepTask) at all.
     * Therefore we need to "Schedule" on our own, wake up the
     * next task and enter Sleep.
     */
    DWORD ret;
#ifdef DEBUG
    PTD ptd;
    register PVDMFRAME pFrame;
    
    ptd = CURRENTPTD();
    GETFRAMEPTR(ptd->vpStack, pFrame);
    LOGDEBUG(LOG_TRACE,("%04X          xxxUserYield\n", pFrame->wTDB));
#endif
     if (ret = GetQueueStatus(QS_SENDMESSAGE | QS_TIMER))
     {
         DWORD count;
         MSG msg;
         
         ReleaseThunkLock(&count);
         PeekMessage( &msg, 0, 0, 0, PM_REMOVE | (ret & 0xFFFF0000) );
         if (msg.message == WM_TIMER)
         {
             TranslateMessage(&msg);
             DispatchMessage(&msg);
         }
         RestoreThunkLock(count);
         return TRUE;
     }
     OldYield();
     return TRUE;
}

BOOL    xxxWaitForMsgAndEvent(IN HANDLE hevent)
{
    DWORD count, ret;
#ifdef DEBUG
    PTD ptd;
    register PVDMFRAME pFrame;
    
    ptd = CURRENTPTD();
    GETFRAMEPTR(ptd->vpStack, pFrame);
    LOGDEBUG(LOG_TRACE,("%04X          xxxWaitForMsgAndEvent\n", pFrame->wTDB));
#endif
    ReleaseThunkLock(&count);
    ret = MsgWaitForMultipleObjects(1, &hevent, FALSE, INFINITE, QS_ALLEVENTS);
    RestoreThunkLock(count);
    LOGDEBUG(LOG_TRACE,("%04X          xxxWaitForMsgAndEvent: Received %s\n", pFrame->wTDB, (ret==WAIT_OBJECT_0)?"hEvent":"msg"));

    //
    // Report which side woke us, per the documented WK32WowWaitForMsgAndEvent
    // contract (TRUE = the event was signalled, FALSE = a message arrived).
    // WOWEXEC's main loop ignores this, but the shared-WOW event-model wake
    // bridge in WK32WowWaitForMsgAndEvent needs it: it must post
    // WM_WOWEXECSTARTAPP only when CSRSS actually signalled the event (a queued
    // command), not on ordinary window messages -- otherwise every user
    // interaction with the WowExec window triggers a spurious command poll.
    //
    return (ret == WAIT_OBJECT_0);
}

#endif 
