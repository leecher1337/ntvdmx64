/*++
 *
 *  WOW v1.0
 *
 *  Copyright (c) 1991, Microsoft Corporation
 *
 *  WSYSLEVEL.C
 *  WOW32 Syslevel Routines
 *
 *  History:
 *  Created 08-Dec-2020 by leecher1337
--*/


#include "precomp.h"
#pragma hdrstop

#ifdef W10
// Remember Win95? Win16Mutex is back, harharhar!
static SYSLEVEL Win16Mutex;
static CRITICAL_SECTION_DEBUG critsect_debug =
{
    0, 0, &Win16Mutex.crst,
    { &critsect_debug.ProcessLocksList, &critsect_debug.ProcessLocksList },
      0, 0, { (DWORD_PTR)(__FILE__ ": Win16Mutex") }
};
static SYSLEVEL Win16Mutex = { { &critsect_debug, -1, 0, 0, 0, 0 }, 1 };

/************************************************************************
 *           GetpWin16Lock    (KERNEL32.93)
 */
VOID WINAPI GetpWin16Lock(SYSLEVEL **lock)
{
    *lock = &Win16Mutex;
}

/************************************************************************
 *           _CreateSysLevel    (KERNEL.438)
 */
VOID WINAPI _CreateSysLevel(SYSLEVEL *lock, INT level)
{
    RtlInitializeCriticalSection( &lock->crst );
    lock->level = level;

    LOGDEBUG(LOG_TRACE,("(%p, %d): handle is %p\n",
                  lock, level, lock->crst.LockSemaphore ));
}

/************************************************************************
 *           _EnterSysLevel    (KERNEL32.97)
 *           _EnterSysLevel    (KERNEL.439)
 */
VOID WINAPI _EnterSysLevel(SYSLEVEL *lock)
{
    PTD ptd = CURRENTPTD();
    int i;
#ifdef DEBUG
    register PVDMFRAME pFrame;

    GETFRAMEPTR(ptd->vpStack, pFrame);
#endif
    LOGDEBUG(LOG_TRACE,("%04X          _EnterSysLevel(%p, level %d): thread %x count before %d\n",
          pFrame->wTDB, lock, lock->level, GetCurrentThreadId(), ptd->sys_count[lock->level] ));

    for ( i = 3; i > lock->level; i-- )
        if ( ptd->sys_count[i] > 0 )
        {
            LOGDEBUG(LOG_ALWAYS,("%04X          _EnterSysLevel(%p, level %d): Holding %p, level %d. Expect deadlock!\n",
                        pFrame->wTDB, lock, lock->level, ptd->sys_mutex[i], i ));
        }

    RtlEnterCriticalSection( &lock->crst );       

    if (ptd->yield_wait_event)
    {
        HANDLE event = ptd->yield_wait_event;
        DWORD mutex_count, count;
		
        RtlLeaveCriticalSection(&lock->crst);
        mutex_count = _ConfirmSysLevel(lock);
        count = mutex_count;
        /* release lock */
        while (count-- > 0)
        {
            if (--ptd->sys_count[lock->level] == 0)
                ptd->sys_mutex[lock->level] = NULL;
            RtlLeaveCriticalSection(&lock->crst);
        }
        LOGDEBUG(LOG_TRACE,("%04X          _EnterSysLevel waiting for yield_wait_event\n", pFrame->wTDB));
        WaitForSingleObject(event, INFINITE);
        LOGDEBUG(LOG_TRACE,("%04X          _EnterSysLevel got yield_wait_event\n", pFrame->wTDB));
        count = mutex_count;
        /* restore lock */
        while (count-- > 0)
        {
            ptd->sys_count[lock->level]++;
            ptd->sys_mutex[lock->level] = lock;
            RtlEnterCriticalSection(&lock->crst);
        }
        RtlEnterCriticalSection(&lock->crst);
        CloseHandle(event);
    }
    
    ptd->sys_count[lock->level]++;
    ptd->sys_mutex[lock->level] = lock;
    SetEvent(ptd->yield_event);

    LOGDEBUG(LOG_TRACE,("%04X          _EnterSysLevel(%p, level %d): thread %x count after  %d\n",
          pFrame->wTDB, lock, lock->level, GetCurrentThreadId(), ptd->sys_count[lock->level] ));
}

/************************************************************************
 *           _LeaveSysLevel    (KERNEL32.98)
 *           _LeaveSysLevel    (KERNEL.440)
 */
VOID WINAPI _LeaveSysLevel(SYSLEVEL *lock)
{
    PTD ptd = CURRENTPTD();
#ifdef DEBUG
    register PVDMFRAME pFrame;

    GETFRAMEPTR(ptd->vpStack, pFrame);
#endif
    LOGDEBUG(LOG_TRACE,("%04X          _LeaveSysLevel(%p, level %d): thread %x count before %d\n",
          pFrame->wTDB, lock, lock->level, GetCurrentThreadId(), ptd->sys_count[lock->level] ));

    if ( ptd->sys_count[lock->level] <= 0 || ptd->sys_mutex[lock->level] != lock )
    {
        LOGDEBUG(LOG_ALWAYS,("%04X          _LeaveSysLevel(%p, level %d): Invalid state: count %d mutex %p.\n",
                    pFrame->wTDB, lock, lock->level, ptd->sys_count[lock->level],
                    ptd->sys_mutex[lock->level] ));
    }
    else
    {
        if ( --ptd->sys_count[lock->level] == 0 )
            ptd->sys_mutex[lock->level] = NULL;
    }

    RtlLeaveCriticalSection( &lock->crst );   
    SwitchToThread();

    LOGDEBUG(LOG_TRACE,("%04X          _LeaveSysLevel(%p, level %d): thread %x count after  %d\n",
          pFrame->wTDB, lock, lock->level, GetCurrentThreadId(), ptd->sys_count[lock->level] ));
}

/************************************************************************
 *           _ConfirmSysLevel    (KERNEL32.95)
 *           _ConfirmSysLevel    (KERNEL.436)
 */
DWORD WINAPI _ConfirmSysLevel(SYSLEVEL *lock)
{
    if ( lock && lock->crst.OwningThread == (HANDLE)GetCurrentThreadId() )
        return lock->crst.RecursionCount;
    else
        return 0L;
}


/************************************************************************
 *           _CheckNotSysLevel    (KERNEL32.94)
 *           _CheckNotSysLevel    (KERNEL.437)
 */
VOID WINAPI _CheckNotSysLevel(SYSLEVEL *lock)
{
    if (lock && lock->crst.OwningThread == (HANDLE)GetCurrentThreadId() &&
        lock->crst.RecursionCount)
    {
        LOGDEBUG(LOG_ALWAYS,( "Holding lock %p level %d\n", lock, lock->level ));
        DbgBreakPoint();
    }
}

/************************************************************************
 *           _EnterWin16Lock			[KERNEL.480]
 */
VOID WINAPI _EnterWin16Lock(void)
{
    _EnterSysLevel(&Win16Mutex);
}

/************************************************************************
 *           _LeaveWin16Lock		[KERNEL.481]
 */
VOID WINAPI _LeaveWin16Lock(void)
{
    _LeaveSysLevel(&Win16Mutex);
}

/************************************************************************
 *           ReleaseThunkLock    (KERNEL32.48)
 */
VOID WINAPI ReleaseThunkLock(DWORD *mutex_count)
{
    DWORD count = _ConfirmSysLevel(&Win16Mutex);
    *mutex_count = count;

    LOGDEBUG(LOG_TRACE,("ReleaseThunkLock %d\n", count));
    while (count-- > 0)
        _LeaveSysLevel(&Win16Mutex);
}

/************************************************************************
 *           RestoreThunkLock    (KERNEL32.49)
 */
VOID WINAPI RestoreThunkLock(DWORD mutex_count)
{
    LOGDEBUG(LOG_TRACE,("RestoreThunkLock %d\n", mutex_count));
    while (mutex_count-- > 0)
        _EnterSysLevel(&Win16Mutex);
}

#endif 
