/*Project: ldntvdm
* Module : createproc
* Author : leecher@dose.0wnz.at
* Descr. : If CreateProcess hooking should be used (may be unstable!),
*          this module implements the necessary functions for it
* Changes: 29.05.2020 - Moved to separate module
*/

#include <ntstatus.h>
#define WIN32_NO_STATUS
#include "ldntvdm.h"
#include "injector32.h"
#include "injector64.h"
#include "wow64inj.h"
#include "symcache.h"
#include "detour.h"
#include "iathook.h"
#include "xpwrap.h"
#include "ntpsapi.h"
#include <Psapi.h>

#ifdef CREATEPROCESS_HOOK

#ifdef _WIN64
#define SUBSYS_OFFSET 0x128
#else
#define SUBSYS_OFFSET 0xB4
#endif

BOOL InjectIntoCreatedThread(LPPROCESS_INFORMATION lpProcessInformation, UCHAR method);

#ifdef NO_APPINIT_DLL
BOOL InjectIntoChildren(HANDLE ProcessHandle)
{
		// Looks pretty ugly, any better idea?
		PxpSYSTEM_PROCESS_INFORMATION ProcessInfo;
		PUCHAR  pucLargeBuffer = 0;
		ULONG   LargeBufferSize = 32 * 1024;
		ULONG   RegionSize;
		NTSTATUS status;
		ULONG TotalOffset;
		PROCESS_BASIC_INFORMATION BasicInfo;
		PROCESS_INFORMATION ProcessInformation = { 0 };

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
			if (!(pucLargeBuffer = VirtualAlloc(pucLargeBuffer, LargeBufferSize, MEM_COMMIT, PAGE_READWRITE)))
			{
				TRACE("Out of memory? %08X\n", GetLastError());
				return FALSE;
			}

			RegionSize = 0;
			status = NtQuerySystemInformation(SystemProcessInformation, pucLargeBuffer, LargeBufferSize, &TotalOffset);
			if (NT_SUCCESS(status)) break;
			else if (status == STATUS_INFO_LENGTH_MISMATCH)
			{
				LargeBufferSize += 8192;
				VirtualFree(pucLargeBuffer, 0, MEM_RELEASE);
				pucLargeBuffer = NULL;
			}
			else {
				if (pucLargeBuffer) VirtualFree(pucLargeBuffer, 0, MEM_RELEASE);
				TRACE("Error query SystemProcessInformation: %08X\n", status);
				return status;
			}
		}

		TotalOffset = 0;
		ProcessInfo = (PxpSYSTEM_PROCESS_INFORMATION)pucLargeBuffer;

		while (TRUE)
		{
			//TRACE("Process %d: %S\n", ProcessInfo->UniqueProcessId, ProcessInfo->ImageName.Buffer);
			if (ProcessInfo->InheritedFromUniqueProcessId == BasicInfo.UniqueProcessId)
			{
				TRACE("Process has child with PID %d\n", ProcessInfo->UniqueProcessId);
				if (ProcessInformation.hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION, FALSE, ProcessInfo->UniqueProcessId))
				{
					char szProcess[MAX_PATH];
					int iLen;
					BOOL fIsConhost;

					fIsConhost = (iLen = GetModuleFileNameExA(ProcessInformation.hProcess, NULL, szProcess, sizeof(szProcess))) && 
						iLen > 11 && __stricmp(szProcess+iLen-11, "conhost.exe") == 0;
					{
						TRACE("Want to inject into child (conhost=%d, proc=%s)", fIsConhost, szProcess);
						SuspendThread(ProcessInformation.hThread);
						InjectIntoCreatedThread(&ProcessInformation, fIsConhost?METHOD_CREATETHREAD:METHOD_HOOKLDR);
						ResumeThread(ProcessInformation.hThread);
					}
					CloseHandle(ProcessInformation.hProcess);
				}
				else
				{
					TRACE("Cannot open process %d: %08X\n", ProcessInfo->UniqueProcessId, GetLastError());
				}
			}
			if (ProcessInfo->NextEntryOffset == 0)
			{
				status = STATUS_NO_MORE_ENTRIES;
				break;
			}
			TotalOffset += ProcessInfo->NextEntryOffset;
			ProcessInfo = (PxpSYSTEM_PROCESS_INFORMATION)&pucLargeBuffer[TotalOffset];
		}

		if (pucLargeBuffer) VirtualFree(pucLargeBuffer, 0, MEM_RELEASE);
		return status;

}
#endif // NO_APPINIT_DLL

BOOL InjectIntoCreatedThread(LPPROCESS_INFORMATION lpProcessInformation, UCHAR method)
{
	PROCESS_BASIC_INFORMATION basicInfo;
	HANDLE hProcess = lpProcessInformation->hProcess;
	ULONG ImageSubsystem;
	LPTHREAD_START_ROUTINE pLoadLibraryW;
	DWORD result;
	BOOL bIsWow64 = FALSE, bRet = FALSE;
	NTSTATUS Status;

	if (!NT_SUCCESS((Status = NtQueryInformationProcess(hProcess, ProcessBasicInformation, &basicInfo, sizeof(basicInfo), NULL))))
	{
		if (Status == STATUS_OBJECT_TYPE_MISMATCH)
		{
			// This most likely is a DOS/WOW process. We need to find its corresponding NTVDM.EXE to inject to
			// by getting it from hThread
			// NB: GetProcessIdOfThread() API may not be available on XP, so let's be safe and use:
			THREAD_BASIC_INFORMATION tbi;

			if (NT_SUCCESS(NtQueryInformationThread(lpProcessInformation->hThread, ThreadBasicInformation, &tbi, sizeof(tbi), NULL)))
			{
				if (!(hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION, FALSE, (DWORD)tbi.ClientId.UniqueProcess)))
				{
					TRACE("Cannot open NTVDM process with PID %d: %d\n", (DWORD)tbi.ClientId.UniqueProcess, GetLastError());
					return FALSE;
				}
			}
			else
			{
				TRACE("Cannot get ThreadBasicInformation for hThread %X\n", lpProcessInformation->hThread);
				return FALSE;
			}

		}
		else
		{
			TRACE("Cannot get ProcessBasicInformation for hProcess %X\n", hProcess);
			return FALSE;
		}
	}
#ifndef NO_APPINIT_DLL
	ReadProcessMemory(hProcess, (PVOID)((ULONG_PTR)basicInfo.PebBaseAddress + SUBSYS_OFFSET), &ImageSubsystem, sizeof(ImageSubsystem), &result);
	if (ImageSubsystem == IMAGE_SUBSYSTEM_WINDOWS_CUI)
#endif
	{
		// Commandline application, inject DLL 
		IsWow64Process(hProcess, &bIsWow64);
		TRACE("Injecting into WOW64 Process? %d\n", bIsWow64)
#ifdef _WIN64
		/* 64 -> 32 */
		if (bIsWow64 && !(bRet = injectLdrLoadDLLWow64(hProcess, lpProcessInformation->hThread, LDNTVDM_NAME, method))) OutputDebugStringA("Inject LdrLoadDLLWow64 failed.");

		/* To inject  into WOW64 process, unfortunately we have to wait until process becomes ready, otherwise
		Ldr is NULL and we cannot find LoadLibraryW entry point in target process
		*/
		//if (bIsWow64) CloseHandle(CreateThread(NULL, 0, InjectLdntvdmWow64Thread, basicInfo.UniqueProcessId, 0, NULL));
		/*
		if (pLoadLibraryW = (LPTHREAD_START_ROUTINE)GetLoadLibraryAddressX32(hProcess))
		{
		PBYTE *pLibRemote;
		if (pLibRemote = VirtualAllocEx(hProcess, NULL, sizeof(LDNTVDM_NAME), MEM_COMMIT, PAGE_READWRITE))
		{
		HANDLE hThread;

		WriteProcessMemory(hProcess, pLibRemote, (void*)LDNTVDM_NAME, sizeof(LDNTVDM_NAME), NULL);
		bRet = (hThread = CreateRemoteThread(hProcess, NULL, 0, pLoadLibraryW, pLibRemote, 0, NULL))!=0;
		if (hThread) WaitForSingleObject(hThread, INFINITE);
		VirtualFreeEx(hProcess, pLibRemote, 0, MEM_RELEASE);
		}

		}
		*/
#else
		if (!bIsWow64)
		{
			WCHAR szDLL[256];

			/* 32 -> 64 */
			GetSystemDirectoryW(szDLL, sizeof(szDLL) / sizeof(WCHAR));
			wcscat(szDLL, L"\\"LDNTVDM_NAME);
			if (!(bRet = inject_dll_x64(hProcess, szDLL)))
			{
				OutputDebugStringA("Injecting 64bit DLL from 32bit failed");
			}
		}
#endif
		/* 64 -> 64, 32 -> 32 */
		else {
			if (!(bRet = injectLdrLoadDLL(hProcess, lpProcessInformation->hThread, LDNTVDM_NAME, method))) OutputDebugStringA("Inject LdrLoadDLL failed.");
		}
	}
	if (hProcess != lpProcessInformation->hProcess) CloseHandle(hProcess);
	return bRet;
}

#ifndef CREATEPROCESS_HOOKNTCREATE

typedef BOOL(WINAPI *CreateProcessAFunc)(LPCSTR lpApplicationName, LPSTR lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCSTR lpCurrentDirectory,
	LPSTARTUPINFOA lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation);
CreateProcessAFunc CreateProcessAReal;
BOOL WINAPI CreateProcessAHook(LPCSTR lpApplicationName, LPSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment,
	LPCSTR lpCurrentDirectory, LPSTARTUPINFOA lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation)
{
	BOOL bRet;
	DWORD dwDebugged = (dwCreationFlags&DEBUG_PROCESS) ? 0 : CREATE_SUSPENDED;

#ifdef USE_SYMCACHE
	UpdateSymbolCache(TRUE);
#endif
	bRet = CreateProcessAReal(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes,
		bInheritHandles, dwCreationFlags | dwDebugged, lpEnvironment, lpCurrentDirectory, lpStartupInfo,
		lpProcessInformation);
	if (dwDebugged && bRet && lpProcessInformation->hThread)
	{
		InjectIntoCreatedThread(lpProcessInformation, METHOD_HOOKLDR);
		if (!(dwCreationFlags & CREATE_SUSPENDED)) ResumeThread(lpProcessInformation->hThread);
	}
	return bRet;
}

typedef BOOL(WINAPI *CreateProcessWFunc)(LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles,
	DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation);
CreateProcessWFunc CreateProcessWReal;
BOOL WINAPI CreateProcessWHook(LPCWSTR lpApplicationName, LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment,
	LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation)
{
	BOOL bRet;
	DWORD dwDebugged = (dwCreationFlags&DEBUG_PROCESS) ? 0 : CREATE_SUSPENDED;

#ifdef USE_SYMCACHE
	UpdateSymbolCache(TRUE);
#endif
	bRet = CreateProcessWReal(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes,
		bInheritHandles, dwCreationFlags | dwDebugged, lpEnvironment, lpCurrentDirectory, lpStartupInfo,
		lpProcessInformation);

	if (dwDebugged && bRet && lpProcessInformation->hThread)
	{
		InjectIntoCreatedThread(lpProcessInformation, METHOD_HOOKLDR);
		if (!(dwCreationFlags & CREATE_SUSPENDED)) ResumeThread(lpProcessInformation->hThread);
	}
	return bRet;
}

void CreateProcessHook_Install(HMODULE hKrnl32)
{
#if defined (TARGET_WINXP) || defined(TARGET_WIN7)
	CreateProcessAReal = Hook_Inline(GetCurrentProcess(), hKrnl32, CreateProcessA, CreateProcessAHook);
	CreateProcessWReal = Hook_Inline(GetCurrentProcess(), hKrnl32, CreateProcessW, CreateProcessWHook);
#else
	/* Newer Windows Versions use l1-1-0 instead of l1-1-2 */
	if (Hook_IAT_x64_IAT((LPBYTE)hKrnl32, "api-ms-win-core-processthreads-l1-1-2.dll", "CreateProcessA", CreateProcessAHook, (PULONG_PTR)&CreateProcessAReal)<0)
		Hook_IAT_x64_IAT((LPBYTE)hKrnl32, "api-ms-win-core-processthreads-l1-1-0.dll", "CreateProcessA", CreateProcessAHook, (PULONG_PTR)&CreateProcessAReal);
	if (Hook_IAT_x64_IAT((LPBYTE)hKrnl32, "api-ms-win-core-processthreads-l1-1-2.dll", "CreateProcessW", CreateProcessWHook, (PULONG_PTR)&CreateProcessWReal)<0)
		Hook_IAT_x64_IAT((LPBYTE)hKrnl32, "api-ms-win-core-processthreads-l1-1-0.dll", "CreateProcessW", CreateProcessWHook, (PULONG_PTR)&CreateProcessWReal);
#endif
}

#endif // CREATEPROCESS_HOOKNTCREATE

#endif
