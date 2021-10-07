/* Project: ldntvdm
 * Module : injector
 * Author : leecher@dose.0wnz.at
 * Descr. : Defines structures for the common shellcode used in the injector modules
 * Changes: 11.09.2021  - Created
 */
#pragma once

#ifndef DEFINE_BASEMSG32
#include "basemsg64.h"
#endif

typedef NTSTATUS(NTAPI *pLdrLoadDll)(PWCHAR, ULONG, PUNICODE_STRING, PHANDLE);
typedef NTSTATUS(NTAPI *pNtTerminateThread)(IN HANDLE ThreadHandle, IN NTSTATUS ExitStatus);
typedef NTSTATUS(*pLdrpInitializeProcess)(IN PCONTEXT Context OPTIONAL, IN PVOID SystemDllBase);

typedef struct _THREAD_DATA
{
	pLdrLoadDll fnLdrLoadDll;
	union
	{
		pNtTerminateThread fnNtTerminateThread;
		PPS_POST_PROCESS_INIT_ROUTINE fnOrigPostProcessInit;
		pLdrpInitializeProcess fnLdrpInitializeProcess;
	};
	UNICODE_STRING UnicodeString;
	WCHAR DllName[MAX_PATH];
	PWCHAR DllPath;
	ULONG Flags;
	LPTHREAD_START_ROUTINE OrigEIP;
	PVOID EIPParams;
	HANDLE ModuleHandle;
}THREAD_DATA, *PTHREAD_DATA;

typedef struct _UNICODE_STRING32 {
	USHORT Length;
	USHORT MaximumLength;
	ULONG  Buffer;
} UNICODE_STRING32;

typedef struct
{
	ULONG fnLdrLoadDll;
	union
	{
		ULONG fnNtTerminateThread;
		ULONG fnOrigPostProcessInit;
		ULONG fnLdrpInitializeProcess;
	};
	UNICODE_STRING32 UnicodeString;
	WCHAR DllName[MAX_PATH];
	ULONG DllPath;
	ULONG Flags;
	ULONG OrigEIP;
	ULONG EIPParams;
	ULONG ModuleHandle;
}THREAD_DATA32, *PTHREAD_DATA32;

EXTERN_C NTSTATUS NTAPI RtlCreateUserThread(
	HANDLE,
	PSECURITY_DESCRIPTOR,
	BOOLEAN,
	ULONG,
	PULONG,
	PULONG,
	PVOID,
	PVOID,
	PHANDLE,
	CLIENT_ID*);
