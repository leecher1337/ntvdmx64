/*Project: ldntvdm
* Module : conhostproc
* Author : leecher@dose.0wnz.at
* Descr. : This module fixes a bug in 32bit version of NtQueryInformationProcess
*          that fails to provide the correct handle of the attached Conhost.exe
*          process (Information class ProcessConsoleHostProcess)
* Changes: 29.05.2020  - Moved to seperate module
*/

#include "ldntvdm.h"
#include "iathook.h"
#include "wow64ext.h"

#define ProcessConsoleHostProcess 49

#ifndef TARGET_WINXP
#ifndef _WIN64
NTSTATUS
NTAPI
myNtQueryInformationProcess(
	IN HANDLE ProcessHandle,
	IN PROCESSINFOCLASS ProcessInformationClass,
	OUT PVOID ProcessInformation,
	IN ULONG ProcessInformationLength,
	OUT PULONG ReturnLength OPTIONAL
	)
{
	static ULONGLONG ntqip = 0;

	if (ProcessInformationClass == ProcessConsoleHostProcess)
	{
		ULONGLONG ret = 0, ProcessInformation64 = 0;
		NTSTATUS status;

		if (0 == ntqip)
			ntqip = GetProcAddress64(getNTDLL64(), "NtQueryInformationProcess");
		if (ntqip)
		{
			status = (NTSTATUS)X64Call(ntqip, 5, (ULONGLONG)-1, (ULONGLONG)ProcessConsoleHostProcess, (ULONGLONG)&ProcessInformation64, (ULONGLONG)sizeof(ProcessInformation64), (ULONGLONG)NULL);
			if (NT_SUCCESS(status))
			{
				*((ULONG*)ProcessInformation) = (ULONG)ProcessInformation64;
				if (ReturnLength) *ReturnLength = sizeof(ULONG);
			}
			return status;
		}
	}
	return NtQueryInformationProcess(ProcessHandle, ProcessInformationClass, ProcessInformation, ProcessInformationLength, ReturnLength);
}
#else /* WIN32 */
#define myNtQueryInformationProcess NtQueryInformationProcess
#endif

HANDLE GetConsoleHost(void)
{
	static ULONG_PTR hConHost = 0;

	if (!hConHost)
	{
		if (myNtQueryInformationProcess((HANDLE)-1, ProcessConsoleHostProcess, &hConHost, sizeof(hConHost), NULL) != STATUS_SUCCESS)
			return INVALID_HANDLE_VALUE;
		if (hConHost & 1) hConHost &= ~1; // else hConHost = 0;
	}
	return (HANDLE)hConHost;
}

void HookNtQueryInformationProcess(HMODULE hKernelBase, HMODULE hKrnl32)
{
	Hook_IAT_x64_IAT((LPBYTE)hKrnl32, "ntdll.dll", "NtQueryInformationProcess", myNtQueryInformationProcess, NULL);
	Hook_IAT_x64_IAT((LPBYTE)hKernelBase, "ntdll.dll", "NtQueryInformationProcess", myNtQueryInformationProcess, NULL);
}

#endif /* TARGET_WINXP */