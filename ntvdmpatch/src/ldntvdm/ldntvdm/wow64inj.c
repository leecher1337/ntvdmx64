/* Project: ldntvdm
 * Module : wow64inj
 * Author : leecher@dose.0wnz.at, Metasploit Framework
 * Descr. : This module implements functions for injecting DLLs into 64bit
 *          processes from WOW64 (32 bit) processes.
 * Changes: 01.04.2016  - Created
 */

#ifndef _WIN64
#include "ldntvdm.h"


// see '/msf3/external/source/shellcode/x86/migrate/executex64.asm'
BYTE migrate_executex64[] = 
"\x55\x89\xE5\x56\x57\x8B\x75\x08\x8B\x4D\x0C\xE8\x00\x00\x00\x00"
"\x58\x83\xC0\x25\x83\xEC\x08\x89\xE2\xC7\x42\x04\x33\x00\x00\x00"
"\x89\x02\xE8\x09\x00\x00\x00\x83\xC4\x14\x5F\x5E\x5D\xC2\x08\x00"
"\x8B\x3C\x24\xFF\x2A\x48\x31\xC0\x57\xFF\xD6\x5F\x50\xC7\x44\x24"
"\x04\x23\x00\x00\x00\x89\x3C\x24\xFF\x2C\x24";

// see '/msf3/external/source/shellcode/x64/migrate/remotethread.asm'
BYTE migrate_wownativex[] = 
"\xFC\x48\x89\xCE\x48\x89\xE7\x48\x83\xE4\xF0\xE8\xC8\x00\x00\x00"
"\x41\x51\x41\x50\x52\x51\x56\x48\x31\xD2\x65\x48\x8B\x52\x60\x48"
"\x8B\x52\x18\x48\x8B\x52\x20\x48\x8B\x72\x50\x48\x0F\xB7\x4A\x4A"
"\x4D\x31\xC9\x48\x31\xC0\xAC\x3C\x61\x7C\x02\x2C\x20\x41\xC1\xC9"
"\x0D\x41\x01\xC1\xE2\xED\x52\x41\x51\x48\x8B\x52\x20\x8B\x42\x3C"
"\x48\x01\xD0\x66\x81\x78\x18\x0B\x02\x75\x72\x8B\x80\x88\x00\x00"
"\x00\x48\x85\xC0\x74\x67\x48\x01\xD0\x50\x8B\x48\x18\x44\x8B\x40"
"\x20\x49\x01\xD0\xE3\x56\x48\xFF\xC9\x41\x8B\x34\x88\x48\x01\xD6"
"\x4D\x31\xC9\x48\x31\xC0\xAC\x41\xC1\xC9\x0D\x41\x01\xC1\x38\xE0"
"\x75\xF1\x4C\x03\x4C\x24\x08\x45\x39\xD1\x75\xD8\x58\x44\x8B\x40"
"\x24\x49\x01\xD0\x66\x41\x8B\x0C\x48\x44\x8B\x40\x1C\x49\x01\xD0"
"\x41\x8B\x04\x88\x48\x01\xD0\x41\x58\x41\x58\x5E\x59\x5A\x41\x58"
"\x41\x59\x41\x5A\x48\x83\xEC\x20\x41\x52\xFF\xE0\x58\x41\x59\x5A"
"\x48\x8B\x12\xE9\x4F\xFF\xFF\xFF\x5D\x4D\x31\xC9\x41\x51\x48\x8D"
"\x46\x18\x50\xFF\x76\x10\xFF\x76\x08\x41\x51\x41\x51\x49\xB8\x01"
"\x00\x00\x00\x00\x00\x00\x00\x48\x31\xD2\x48\x8B\x0E\x41\xBA\xC8"
"\x38\xA4\x40\xFF\xD5\x48\x85\xC0\x74\x0C\x48\xB8\x00\x00\x00\x00"
"\x00\x00\x00\x00\xEB\x0A\x48\xB8\x01\x00\x00\x00\x00\x00\x00\x00"
"\x48\x83\xC4\x50\x48\x89\xFC\xC3";

// see loadlibrary64.asm
BYTE ldr_load_library_x64[] =
"\xfc\x48\x83\xe4\xf0\xe8\xc8\x00\x00\x00\x41\x51\x41\x50\x52"
"\x51\x56\x48\x31\xd2\x65\x48\x8b\x52\x60\x48\x8b\x52\x18\x48"
"\x8b\x52\x20\x48\x8b\x72\x50\x48\x0f\xb7\x4a\x4a\x4d\x31\xc9"
"\x48\x31\xc0\xac\x3c\x61\x7c\x02\x2c\x20\x41\xc1\xc9\x0d\x41"
"\x01\xc1\xe2\xed\x52\x41\x51\x48\x8b\x52\x20\x8b\x42\x3c\x48"
"\x01\xd0\x66\x81\x78\x18\x0b\x02\x75\x72\x8b\x80\x88\x00\x00"
"\x00\x48\x85\xc0\x74\x67\x48\x01\xd0\x50\x8b\x48\x18\x44\x8b"
"\x40\x20\x49\x01\xd0\xe3\x56\x48\xff\xc9\x41\x8b\x34\x88\x48"
"\x01\xd6\x4d\x31\xc9\x48\x31\xc0\xac\x41\xc1\xc9\x0d\x41\x01"
"\xc1\x38\xe0\x75\xf1\x4c\x03\x4c\x24\x08\x45\x39\xd1\x75\xd8"
"\x58\x44\x8b\x40\x24\x49\x01\xd0\x66\x41\x8b\x0c\x48\x44\x8b"
"\x40\x1c\x49\x01\xd0\x41\x8b\x04\x88\x48\x01\xd0\x41\x58\x41"
"\x58\x5e\x59\x5a\x41\x58\x41\x59\x41\x5a\x48\x83\xec\x20\x41"
"\x52\xff\xe0\x58\x41\x59\x5a\x48\x8b\x12\xe9\x4f\xff\xff\xff"
"\x5d\x49\x89\xe1\x4c\x8d\x85\x14\x01\x00\x00\x49\x8d\x58\x08"
"\x53\x49\x8b\x18\x53\x49\x89\xe0\x48\x31\xc9\x48\x31\xd2\x41"
"\xba\x13\x9c\xbf\xbd\xff\xd5\xbb\xe0\x1d\x2a\x0a\x41\xba\xa6"
"\x95\xbd\x9d\xff\xd5\x48\x83\xc4\x28\x3c\x06\x7c\x0a\x80\xfb"
"\xe0\x75\x05\xbb\x47\x13\x72\x6f\x6a\x00\x59\x41\x89\xda\xff"
"\xd5";

typedef struct _WOW64CONTEXT
{
	union
	{
		HANDLE hProcess;
		BYTE bPadding2[8];
	} h;

	union
	{
		LPVOID lpStartAddress;
		BYTE bPadding1[8];
	} s;

	union
	{
		LPVOID lpParameter;
		BYTE bPadding2[8];
	} p;
	union
	{
		HANDLE hThread;
		BYTE bPadding2[8];
	} t;
} WOW64CONTEXT, *LPWOW64CONTEXT;

// Definitions used for running native x64 code from a wow64 process (see executex64.asm)
typedef BOOL(WINAPI * X64FUNCTION)(DWORD dwParameter);
typedef DWORD(WINAPI * EXECUTEX64)(X64FUNCTION pFunction, DWORD dwParameter);

/*
* Attempt to gain code execution in a native x64 process from a wow64 process by transitioning out of the wow64 (x86)
* enviroment into a native x64 enviroment and accessing the native win64 APIs.
*/
DWORD inject_via_remotethread_wow64(HANDLE hProcess, LPVOID lpStartAddress, LPVOID lpParameter, HANDLE * pThread)
{
	DWORD dwResult = ERROR_SUCCESS;
	EXECUTEX64 pExecuteX64 = NULL;
	X64FUNCTION pX64function = NULL;
	WOW64CONTEXT * ctx = NULL;

	// alloc a RWX buffer in this process for the EXECUTEX64 function
	if (pExecuteX64 = (EXECUTEX64)VirtualAlloc(NULL, sizeof(migrate_executex64), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE))
	{
		RtlMoveMemory(pExecuteX64, &migrate_executex64, sizeof(migrate_executex64));
		if (pX64function = (X64FUNCTION)VirtualAlloc(NULL, sizeof(migrate_wownativex) + sizeof(WOW64CONTEXT), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE))
		{
			RtlMoveMemory(pX64function, &migrate_wownativex, sizeof(migrate_wownativex));
			ctx = (WOW64CONTEXT *)((BYTE *)pX64function + sizeof(migrate_wownativex));
			ctx->h.hProcess = hProcess;
			ctx->s.lpStartAddress = lpStartAddress;
			ctx->p.lpParameter = lpParameter;
			ctx->t.hThread = NULL;

			// Transition this wow64 process into native x64 and call pX64function( ctx )
			// The native function will use the native Win64 API's to create a remote thread in the target process.
			if (!pExecuteX64(pX64function, (DWORD)ctx))
				SetLastError(dwResult = ERROR_ACCESS_DENIED);
			else
			if (!ctx->t.hThread)
				SetLastError(dwResult = ERROR_INVALID_HANDLE);
			else
				*pThread = ctx->t.hThread;
		}
	}

	if (pExecuteX64)
		VirtualFree(pExecuteX64, 0, MEM_DECOMMIT);

	if (pX64function)
		VirtualFree(pX64function, 0, MEM_DECOMMIT);

	return dwResult;
}

BOOL inject_dll_x64(HANDLE hProcess, WCHAR *wszDLL) 
{

	// classic VAE-WPM-CRT
	UNICODE_STRING uStr;
	LPBYTE lpRemoteLibraryBuffer;
	HANDLE hThread;

	uStr.Length = uStr.MaximumLength = (USHORT)lstrlenW(wszDLL)*sizeof(WCHAR);
	uStr.Buffer = NULL;
	if (!(lpRemoteLibraryBuffer = VirtualAllocEx(hProcess, NULL, sizeof(ldr_load_library_x64) - 1 + uStr.Length + sizeof(UNICODE_STRING), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE)) ||
		!WriteProcessMemory(hProcess, lpRemoteLibraryBuffer, ldr_load_library_x64, sizeof(ldr_load_library_x64) - 1, NULL) ||
		!WriteProcessMemory(hProcess, lpRemoteLibraryBuffer + sizeof(ldr_load_library_x64) - 1, &uStr, sizeof(uStr), NULL) ||
		!WriteProcessMemory(hProcess, lpRemoteLibraryBuffer + sizeof(ldr_load_library_x64) - 1 + 8, wszDLL, uStr.Length, NULL))
	{
		OutputDebugStringA("Copy loader code to target process faied.");
		return FALSE;
	}
	// First we try to inject by directly creating a remote thread in the target process
	if ((inject_via_remotethread_wow64(hProcess, lpRemoteLibraryBuffer, NULL, &hThread)) != ERROR_SUCCESS)
	{
		OutputDebugStringA("inject_via_remotethread failed");
		return FALSE;
	}
	else
	{
		ResumeThread(hThread);
		CloseHandle(hThread);
	}
	return TRUE;
}


#endif