/*++
 *
 *  WOW v1.0
 *
 *  Copyright (c) 1991, Microsoft Corporation
 *
 *  WTASK.C
 *  WOW32 Task maangement routines for x64
 *
 *  History:
 *  Created 08-Dec-2020 by leecher1337
--*/

void WINAPI OldYield(void);
void WINAPI xxxDirectedYield( DWORD dwThreadId );
BOOL    xxxUserYield(VOID);
BOOL	xxxWaitForMsgAndEvent(IN HANDLE hevent);
