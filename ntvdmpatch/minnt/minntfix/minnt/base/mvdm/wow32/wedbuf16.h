/*++
 *
 *  WOW v1.0
 *
 *  Copyright (c) 1991, Microsoft Corporation
 *
 *  WEDBUF16.H
 *  EM_GETHANDLE / EM_SETHANDLE 16:32 buffer bridging for Windows 11 22H2+
 *
 *  History:
 *  Created 16-Sep-2026
--*/

#ifdef W10

HANDLE WowSyncEditHandleTo16(HANDLE h32, HAND16 hInst16);
HANDLE WowSyncEditHandleTo32(HAND16 h16, HAND16 hInst16);

#endif /* W10 */
