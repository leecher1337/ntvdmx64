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
void WINAPI xxxDirectedYield( DWORD dwThreadId )
{
    PTD chdthd, ptd;
    BOOL failed = FALSE;
    DWORD dwThreadID = GetCurrentThreadId(), count;
#ifdef DEBUG
	register PVDMFRAME pFrame;

    ptd = CURRENTPTD();
	GETFRAMEPTR(ptd->vpStack, pFrame);
#endif

    LOGDEBUG(LOG_TRACE,("%04X          xxxDirectedYield(%d)\n", pFrame->wTDB, dwThreadId));
    EnterCriticalSection(&gcsWOW);
    for (chdthd = gptdTaskHead; chdthd && chdthd->dwThreadID != dwThreadID; chdthd = chdthd->ptdNext);
    LeaveCriticalSection(&gcsWOW); 
    
    if (!chdthd)
    {
        OldYield();
        return;
    }
    if (chdthd->yield_event)
    {
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
                DWORD count;
                
                ReleaseThunkLock(&count);
                Sleep(10);
                RestoreThunkLock(count);
                return;
            }
            failed = TRUE;
        }
        if (ptd->yield_wait_event) return;
    }
    if (failed)
    {
        LOGDEBUG(LOG_WARNING,("nested DirectedYield doesnt work.\n"));
        OldYield();
        return;
    }
    chdthd->yield_event = CreateEvent(NULL, TRUE, FALSE, NULL);
    for (ptd = gptdTaskHead; ptd; ptd = ptd->ptdNext)
    {
        if (ptd->dwThreadID != dwThreadID)
            ptd->yield_wait_event = CreateEvent(NULL, TRUE, FALSE, NULL);
    }   

    ReleaseThunkLock(&count);

    /*
     * In win16, if hTask doesn't wait events, another task will be executed.
     * Here, wait until timeout.
     */
    WaitForSingleObject(chdthd->yield_event, 100);
    RestoreThunkLock(count);
    CloseHandle(chdthd->yield_event);
    chdthd->yield_event = NULL;
    
    for (ptd = gptdTaskHead; ptd; ptd = ptd->ptdNext)
    {
        if (ptd->dwThreadID != dwThreadID)
        {
            LOGDEBUG(LOG_TRACE,("%04X          xxxDirectedYield setting yield_wait_event on TID %d\n", pFrame->wTDB, ptd->dwThreadID));
            SetEvent(ptd->yield_wait_event);
            ptd->yield_wait_event = NULL;
        }
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
    return FALSE;
}

#endif 
