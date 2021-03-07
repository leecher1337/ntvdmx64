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


#ifdef W10
VOID WINAPI GetpWin16Lock(SYSLEVEL **lock);
VOID WINAPI _CreateSysLevel(SYSLEVEL *lock, INT level);
VOID WINAPI _EnterSysLevel(SYSLEVEL *lock);
VOID WINAPI _LeaveSysLevel(SYSLEVEL *lock);
DWORD WINAPI _ConfirmSysLevel(SYSLEVEL *lock);
VOID WINAPI _CheckNotSysLevel(SYSLEVEL *lock);
VOID WINAPI _EnterWin16Lock(void);
VOID WINAPI _LeaveWin16Lock(void);
VOID WINAPI ReleaseThunkLock(DWORD *mutex_count);
VOID WINAPI RestoreThunkLock(DWORD mutex_count);
#endif 
