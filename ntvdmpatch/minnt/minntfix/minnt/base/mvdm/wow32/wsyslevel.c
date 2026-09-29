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
extern CRITICAL_SECTION gcsWOW;            // WOW Critical Section used when updating task linked list, also guards ptd->yield_wait_event (see xxxDirectedYield, wtask.c)

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

    LOGDEBUG(15,("(%p, %d): handle is %p\n",
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
    LOGDEBUG(15,("%04X          _EnterSysLevel(%p, level %d): thread %x count before %d\n",
          pFrame->wTDB, lock, lock->level, GetCurrentThreadId(), ptd->sys_count[lock->level] ));

    for ( i = 3; i > lock->level; i-- )
        if ( ptd->sys_count[i] > 0 )
        {
            LOGDEBUG(LOG_ALWAYS,("%04X          _EnterSysLevel(%p, level %d): Holding %p, level %d. Expect deadlock!\n",
                        pFrame->wTDB, lock, lock->level, ptd->sys_mutex[i], i ));
        }

    RtlEnterCriticalSection( &lock->crst );

    //
    // Atomically capture-and-clear ptd->yield_wait_event under gcsWOW
    // (the same lock xxxDirectedYield now holds while scanning for and
    // creating these events, see wtask.c). This closes two bugs:
    //
    //  1) Without the lock, a concurrent xxxDirectedYield call on
    //     another thread could still be in the middle of assigning this
    //     field (or replacing it) right as this code reads it, so the
    //     value read here wasn't guaranteed to be the one that call
    //     actually signals -- xxxDirectedYield now avoids that on its
    //     side by remembering the handle it creates locally instead of
    //     re-reading this field, but capturing under the same lock here
    //     too removes any remaining ambiguity about which handle is
    //     "claimed".
    //
    //  2) The field was never reset to NULL after being consumed, so if
    //     this thread ever called _EnterSysLevel again without a new
    //     xxxDirectedYield call targeting it in between, it would see
    //     the same (already-closed) handle value and wait on it again --
    //     a dangling handle that, if its value had since been reused by
    //     an unrelated, never-signaled kernel object, would hang this
    //     thread forever waiting on something that has nothing to do
    //     with DirectedYield.
    //
    EnterCriticalSection(&gcsWOW);
    if (ptd->yield_wait_event)
    {
        HANDLE event = ptd->yield_wait_event;
        DWORD mutex_count, count;

        ptd->yield_wait_event = NULL;
        LeaveCriticalSection(&gcsWOW);

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
        LOGDEBUG(15,("%04X          _EnterSysLevel waiting for yield_wait_event\n", pFrame->wTDB));
        WaitForSingleObject(event, INFINITE);
        LOGDEBUG(15,("%04X          _EnterSysLevel got yield_wait_event\n", pFrame->wTDB));
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
    else
    {
        LeaveCriticalSection(&gcsWOW);
    }

    ptd->sys_count[lock->level]++;
    ptd->sys_mutex[lock->level] = lock;
    SetEvent(ptd->yield_event);

    LOGDEBUG(15,("%04X          _EnterSysLevel(%p, level %d): thread %x count after  %d\n",
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
    LOGDEBUG(15,("%04X          _LeaveSysLevel(%p, level %d): thread %x count before %d\n",
          pFrame->wTDB, lock, lock->level, GetCurrentThreadId(), ptd->sys_count[lock->level] ));

    if ( ptd->sys_count[lock->level] <= 0 || ptd->sys_mutex[lock->level] != lock )
    {
        // ptd->sys_count[level] is out of sync with the real OS critical-
        // section recursion depth here. Two distinct shapes hit this:
        //
        //  1) A real extra recursion level this counter didn't track (e.g.
        //     ReleaseThunkLock's _ConfirmSysLevel-driven unwind loop calls
        //     _LeaveSysLevel one more time than this array counted). Here
        //     the thread still genuinely owns the section, and the leave
        //     below is required -- skipping it once left Win16Mutex one
        //     recursion level too deep forever (confirmed via trace:
        //     WK32WOWInitTask's ReleaseThunkLock(2) only performed one real
        //     leave, so the next task's _EnterSysLevel blocked permanently
        //     on the still-held lock -- a hang at launch).
        //
        //  2) A genuine double release with nothing left to give up (e.g.
        //     WU32EnumChildWindows's RELEASE_THUNKLOCK around the whole
        //     EnumChildWindows() call already fully released this level,
        //     then its own per-child callback W32EnumWindowFunc releases
        //     again before calling into 16-bit code). Here the thread does
        //     NOT own the section any more, and calling
        //     RtlLeaveCriticalSection anyway is undefined behaviour on the
        //     OS lock -- on this port, where Win16Mutex is the actual
        //     cross-thread mutual exclusion serializing concurrent 16-bit/
        //     CPU-emulated execution across real OS threads (unlike real
        //     single-threaded WOW), that spurious release lets another
        //     task's thread believe it can run 16-bit code concurrently.
        //     Confirmed via trace: exactly this shape preceded a callback
        //     jumping to Kernel ordinal 0 (DUMMYENTRY) instead of the real
        //     16-bit enum proc, then the process died with no further log.
        //
        // Distinguish them via the OS's own bookkeeping rather than this
        // array: _ConfirmSysLevel reports whether this thread still truly
        // owns the section (and how deep), independent of ptd->sys_count.
        LOGDEBUG(LOG_ALWAYS,("%04X          _LeaveSysLevel(%p, level %d): Invalid state: count %d mutex %p.\n",
                    pFrame->wTDB, lock, lock->level, ptd->sys_count[lock->level],
                    ptd->sys_mutex[lock->level] ));

        if ( _ConfirmSysLevel(lock) > 0 )
            RtlLeaveCriticalSection( &lock->crst );
    }
    else
    {
        if ( --ptd->sys_count[lock->level] == 0 )
            ptd->sys_mutex[lock->level] = NULL;

        RtlLeaveCriticalSection( &lock->crst );
    }

    SwitchToThread();

    LOGDEBUG(15,("%04X          _LeaveSysLevel(%p, level %d): thread %x count after  %d\n",
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

    LOGDEBUG(15,("ReleaseThunkLock %d\n", count));
    while (count-- > 0)
        _LeaveSysLevel(&Win16Mutex);
}

/************************************************************************
 *           RestoreThunkLock    (KERNEL32.49)
 */
VOID WINAPI RestoreThunkLock(DWORD mutex_count)
{
    LOGDEBUG(15,("RestoreThunkLock %d\n", mutex_count));
    while (mutex_count-- > 0)
        _EnterSysLevel(&Win16Mutex);
}

#endif 
