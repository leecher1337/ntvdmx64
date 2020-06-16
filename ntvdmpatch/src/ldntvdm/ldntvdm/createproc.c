/*Project: ldntvdm
* Module : createproc
* Author : leecher@dose.0wnz.at
* Descr. : If CreateProcess hooking should be used (may be unstable!),
*          this module implements the necessary functions for it
* Changes: 29.05.2020 - Moved to separate module
*/

#include "ldntvdm.h"
#include "injector32.h"
#include "symcache.h"
#include "detour.h"
#include "iathook.h"

#ifdef CREATEPROCESS_HOOK
BOOL InjectIntoCreatedThread(LPPROCESS_INFORMATION lpProcessInformation)
{
	PROCESS_BASIC_INFORMATION basicInfo;
	ULONG ImageSubsystem;
	LPTHREAD_START_ROUTINE pLoadLibraryW;
	DWORD result;
	BOOL bIsWow64 = FALSE, bRet = FALSE;

	if (!NT_SUCCESS(NtQueryInformationProcess(lpProcessInformation->hProcess, ProcessBasicInformation, &basicInfo, sizeof(basicInfo), NULL))) return FALSE;
	ReadProcessMemory(lpProcessInformation->hProcess, (PVOID)((ULONG_PTR)basicInfo.PebBaseAddress + SUBSYS_OFFSET), &ImageSubsystem, sizeof(ImageSubsystem), &result);
	if (ImageSubsystem == IMAGE_SUBSYSTEM_WINDOWS_CUI)
	{
		// Commandline application, inject DLL 
		IsWow64Process(lpProcessInformation->hProcess, &bIsWow64);
#ifdef _WIN64
		/* 64 -> 32 */
		/* To inject  into WOW64 process, unfortunately we have to wait until process becomes ready, otherwise
		Ldr is NULL and we cannot find LoadLibraryW entry point in target process
		*/
		if (bIsWow64) CreateThread(NULL, 0, InjectLdntvdmWow64Thread, basicInfo.UniqueProcessId, 0, NULL);
		/*
		if (pLoadLibraryW = (LPTHREAD_START_ROUTINE)GetLoadLibraryAddressX32(lpProcessInformation->hProcess))
		{
		PBYTE *pLibRemote;
		if (pLibRemote = VirtualAllocEx(lpProcessInformation->hProcess, NULL, sizeof(LDNTVDM_NAME), MEM_COMMIT, PAGE_READWRITE))
		{
		HANDLE hThread;

		WriteProcessMemory(lpProcessInformation->hProcess, pLibRemote, (void*)LDNTVDM_NAME, sizeof(LDNTVDM_NAME), NULL);
		bRet = (hThread = CreateRemoteThread(lpProcessInformation->hProcess, NULL, 0, pLoadLibraryW, pLibRemote, 0, NULL))!=0;
		if (hThread) WaitForSingleObject(hThread, INFINITE);
		VirtualFreeEx(lpProcessInformation->hProcess, pLibRemote, 0, MEM_RELEASE);
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
			if (!(bRet = inject_dll_x64(lpProcessInformation->hProcess, szDLL)))
			{
				OutputDebugStringA("Injecting 64bit DLL from 32bit failed");
			}
		}
#endif
		/* 64 -> 64, 32 -> 32 */
		else {
			if (!(bRet = injectLdrLoadDLL(lpProcessInformation->hProcess, lpProcessInformation->hThread, LDNTVDM_NAME, METHOD_CREATETHREAD))) OutputDebugStringA("Inject LdrLoadDLL failed.");
		}
	}
	return bRet;
}

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
	UpdateSymbolCache();
#endif
	bRet = CreateProcessAReal(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes,
		bInheritHandles, dwCreationFlags | dwDebugged, lpEnvironment, lpCurrentDirectory, lpStartupInfo,
		lpProcessInformation);
	if (dwDebugged && bRet && lpProcessInformation->hThread)
	{
		InjectIntoCreatedThread(lpProcessInformation);
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
	UpdateSymbolCache();
#endif
	bRet = CreateProcessWReal(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes,
		bInheritHandles, dwCreationFlags | dwDebugged, lpEnvironment, lpCurrentDirectory, lpStartupInfo,
		lpProcessInformation);

	if (dwDebugged && bRet && lpProcessInformation->hThread)
	{
		InjectIntoCreatedThread(lpProcessInformation);
		if (!(dwCreationFlags & CREATE_SUSPENDED)) ResumeThread(lpProcessInformation->hThread);
	}
	return bRet;
}

void CreateProcessHook_Install(HMODULE hKrnl32)
{
#if defined (TARGET_WINXP) || defined(TARGET_WIN7)
	CreateProcessAReal = Hook_Inline(CreateProcessA, CreateProcessAHook);
	CreateProcessWReal = Hook_Inline(CreateProcessW, CreateProcessWHook);
#else
	/* Newer Windows Versions use l1-1-0 instead of l1-1-2 */
	if (Hook_IAT_x64_IAT((LPBYTE)hKrnl32, "api-ms-win-core-processthreads-l1-1-2.dll", "CreateProcessA", CreateProcessAHook, &CreateProcessAReal)<0)
		Hook_IAT_x64_IAT((LPBYTE)hKrnl32, "api-ms-win-core-processthreads-l1-1-0.dll", "CreateProcessA", CreateProcessAHook, &CreateProcessAReal);
	if (Hook_IAT_x64_IAT((LPBYTE)hKrnl32, "api-ms-win-core-processthreads-l1-1-2.dll", "CreateProcessW", CreateProcessWHook, &CreateProcessWReal)<0)
		Hook_IAT_x64_IAT((LPBYTE)hKrnl32, "api-ms-win-core-processthreads-l1-1-0.dll", "CreateProcessW", CreateProcessWHook, &CreateProcessWReal);
#endif
}

#endif
