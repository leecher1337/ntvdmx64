/*Project: ldntvdm
* Module : xpwrap
* Author : leecher@dose.0wnz.at
* Descr. : This module wraps some functions that are not available
*          on Windows XP/Windows Server 2003
* Changes: 31.05.2020  - Created
*/

#include <ntstatus.h>
#define WIN32_NO_STATUS
#include "ldntvdm.h"
#include "xpwrap.h"
#include "ntmmapi.h"
#include "ntpsapi.h"

#ifdef TARGET_WINXP

typedef struct _SYSTEM_THREAD_INFORMATION {
	LARGE_INTEGER   KernelTime;             // time spent in kernel mode
	LARGE_INTEGER   UserTime;               // time spent in user mode
	LARGE_INTEGER   CreateTime;             // thread creation time
	ULONG           WaitTime;               // wait time
	PVOID           StartAddress;           // start address
	CLIENT_ID       ClientId;               // thread and process IDs
	KPRIORITY       Priority;               // dynamic priority
	KPRIORITY       BasePriority;           // base priority
	ULONG           ContextSwitchCount;     // number of context switches
	LONG            State;                  // current state
	LONG            WaitReason;             // wait reason
} SYSTEM_THREAD_INFORMATION, *PSYSTEM_THREAD_INFORMATION;


__kernel_entry NTSTATUS
NTAPI
NtGetNextThread(
	_In_ HANDLE ProcessHandle,
	_In_ HANDLE ThreadHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_ ULONG HandleAttributes,
	_In_ ULONG Flags,
	_Out_ PHANDLE NewThreadHandle
	)
{
	// Looks pretty ugly, any better idea?
	PxpSYSTEM_PROCESS_INFORMATION ProcessInfo;
	PUCHAR  pucLargeBuffer = 0;
	ULONG   LargeBufferSize = 32 * 1024;
	ULONG   RegionSize;
	NTSTATUS status;
	ULONG TotalOffset;
	PROCESS_BASIC_INFORMATION BasicInfo;

	status = NtQueryInformationProcess(
		ProcessHandle,
		ProcessBasicInformation,
		&BasicInfo,
		sizeof(BasicInfo),
		NULL
		);
	if (!NT_SUCCESS(status)) return status;

	status = STATUS_INFO_LENGTH_MISMATCH;
	while (status == STATUS_INFO_LENGTH_MISMATCH) 
	{
		status = NtAllocateVirtualMemory(NtCurrentProcess(), &pucLargeBuffer, 0, &LargeBufferSize, MEM_COMMIT, PAGE_READWRITE);
		if (!NT_SUCCESS(status))
		{
			TRACE("Out of memory? %08X\n", status);
			return status;
		}

		RegionSize = 0;
		status = NtQuerySystemInformation(SystemProcessInformation, pucLargeBuffer, LargeBufferSize, &TotalOffset);
		if (NT_SUCCESS(status)) break;
		else if (status == STATUS_INFO_LENGTH_MISMATCH) 
		{
			LargeBufferSize += 8192;
			NtFreeVirtualMemory(NtCurrentProcess(), &pucLargeBuffer, &RegionSize, MEM_RELEASE);
			pucLargeBuffer = NULL;
		} else {
			if (pucLargeBuffer) NtFreeVirtualMemory(NtCurrentProcess(), &pucLargeBuffer, &RegionSize, MEM_RELEASE);
			TRACE("Error query SystemProcessInformation: %08X\n", status);
			return status;
		}
	}

	TotalOffset = 0;
	ProcessInfo = (PxpSYSTEM_PROCESS_INFORMATION)pucLargeBuffer;

	while (TRUE) 
	{
		//TRACE("Process %d: %S\n", ProcessInfo->UniqueProcessId, ProcessInfo->ImageName.Buffer);
		if (ProcessInfo->UniqueProcessId == BasicInfo.UniqueProcessId)
		{
			ULONG i = 0;
			PSYSTEM_THREAD_INFORMATION Thread;
			OBJECT_ATTRIBUTES   ObjectAttributes;

			Thread = (PSYSTEM_THREAD_INFORMATION)((PBYTE)ProcessInfo + sizeof(xpSYSTEM_PROCESS_INFORMATION));
			if (ThreadHandle)
			{
				THREAD_BASIC_INFORMATION ThreadInfo;

				status = NtQueryInformationThread(ThreadHandle, 0, (PVOID)&ThreadInfo, sizeof(ThreadInfo), NULL);
				if (!NT_SUCCESS(status))
				{
					TRACE("NtQueryInformationThread failed with %08X\n", status);
					break;
				}

				while (i < ProcessInfo->NumberOfThreads)
				{
					i++;
					if (Thread->ClientId.UniqueThread == ThreadInfo.ClientId.UniqueThread) break;
					Thread++;
				}

				if (i >= ProcessInfo->NumberOfThreads)
				{
					status = STATUS_NO_MORE_ENTRIES;
					TRACE("Thread with ID %d not found.\n", ThreadInfo.ClientId.UniqueThread);
					break;
				}
			}
			InitializeObjectAttributes(&ObjectAttributes, NULL, 0, NULL, NULL);
			status = NtOpenThread(NewThreadHandle, DesiredAccess, &ObjectAttributes, &Thread->ClientId);
			if (!NT_SUCCESS(status)) { TRACE("NtOpenThread(%d) failed: %08X\n", Thread->ClientId.UniqueThread); }
			break;
		}
		if (ProcessInfo->NextEntryOffset == 0) 
		{
			status = STATUS_NO_MORE_ENTRIES;
			TRACE("Process with ID %d not found.\n", BasicInfo.UniqueProcessId);
			break;
		}
		TotalOffset += ProcessInfo->NextEntryOffset;
		ProcessInfo = (PxpSYSTEM_PROCESS_INFORMATION)&pucLargeBuffer[TotalOffset];
	}

	if (pucLargeBuffer) NtFreeVirtualMemory(NtCurrentProcess(), &pucLargeBuffer, &RegionSize, MEM_RELEASE);
	return status;
}


typedef struct _OBJECT_NAME_INFORMATION {
	UNICODE_STRING Name;
} OBJECT_NAME_INFORMATION, *POBJECT_NAME_INFORMATION;

DWORD
WINAPI
GetMappedFileNameW(
	HANDLE hProcess,
	LPVOID lpv,
	LPWSTR lpFilename,
	DWORD nSize
	)
{
	struct {
		OBJECT_NAME_INFORMATION ObjectNameInfo;
		WCHAR FileName[MAX_PATH];
	} s;
	DWORD ReturnLength;
	NTSTATUS Status;
	DWORD cb;

	//
	// See if we can figure out the name associated with
	// this mapped region
	//

	Status = NtQueryVirtualMemory(hProcess,
		lpv,
		MemoryMappedFilenameInformation,
		&s.ObjectNameInfo,
		sizeof(s),
		&ReturnLength
		);

	if (!NT_SUCCESS(Status)) {
		SetLastError(RtlNtStatusToDosError(Status));
		return(0);
	}

	nSize *= sizeof(WCHAR);

	cb = s.ObjectNameInfo.Name.MaximumLength;
	if (nSize < cb) {
		cb = nSize;
	}

	RtlMoveMemory(lpFilename, s.ObjectNameInfo.Name.Buffer, cb);

	if (cb == s.ObjectNameInfo.Name.MaximumLength) {
		cb -= sizeof(WCHAR);
	}

	return(cb / sizeof(WCHAR));
}

#endif