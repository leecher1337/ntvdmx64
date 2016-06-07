#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "Winternl.h"
#include <tchar.h>

#define LDNTVDM_NAME _T("ldntvdm.dll")

/* This is the simplest method of entering the NTVDM.
 * It has the advantage that it doesn't need symbol server for internal
 * symbol resoution to execute loader code, however it has some problems
 * with starting DOS-applications from within DOS and it needs NTVDM
 * to be patched to ignore -w switch for WOW.
 */
//#define WOW_HACK

#undef RtlMoveMemory
EXTERN_C void _declspec(dllimport) WINAPI RtlMoveMemory(PVOID, const VOID*, SIZE_T);
#undef RtlZeroMemory
EXTERN_C void _declspec(dllimport) WINAPI RtlZeroMemory(PVOID, SIZE_T);
