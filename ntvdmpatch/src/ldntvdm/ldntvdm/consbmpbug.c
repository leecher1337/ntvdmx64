/*Project: ldntvdm
* Module : consbmpbug.c
* Author : leecher@dose.0wnz.at
* Descr. : This module fixes a missing NULL pointer initialisation
*          in ConHostV1.dll!CreateConsoleBitmap
*          (second NtMapViewOFSection doesn't have its buffer Ptr
*           initialized) by hooking RtlAllocateHeap.
*          Here is a decription of the bug:
*
NTSTATUS CreateScreenBuffer(...)
{
    ...
    ScreenInfo = HeapAlloc(pConHeap,MAKE_TAG( SCREEN_TAG ),sizeof(SCREEN_INFORMATION));
    if (ScreenInfo == NULL) return STATUS_NO_MEMORY;
    ...
    if ((ScreenInfo->Flags = Flags) & CONSOLE_TEXTMODE_BUFFER) {
	...
    } else {
        Status = CreateConsoleBitmap(GraphBufInfo, ScreenInfo);
        ...
    }
    ...
}

CreateConsoleBitmap(...)
{
   ...
    // There it is OK:
    ScreenInfo->BufferInfo.GraphicsInfo.BitMap = 0;
    Status = NtMapViewOfSection(ScreenInfo->BufferInfo.GraphicsInfo.hSection,
                                NtCurrentProcess(),
                                &ScreenInfo->BufferInfo.GraphicsInfo.BitMap,
                                0L,
                                GraphicsInfo->lpBitMapInfo->bmiHeader.biSizeImage,
                                NULL,
                                &ViewSize,
                                ViewUnmap,
                                0L,
                                PAGE_READWRITE
                               );
...
    ViewSize = GraphicsInfo->lpBitMapInfo->bmiHeader.biSizeImage;
// BUGBUG        OH NOES! YOU FORGOT THIS LINE:
ScreenInfo->BufferInfo.GraphicsInfo.ClientBitMap = 0;
// ^^ To fix, add this line here! ^^
    Status = NtMapViewOfSection(ScreenInfo->BufferInfo.GraphicsInfo.hSection,
                                CONSOLE_CLIENTPROCESSHANDLE(),
                                &ScreenInfo->BufferInfo.GraphicsInfo.ClientBitMap,
                                0L,
                                GraphicsInfo->lpBitMapInfo->bmiHeader.biSizeImage,
                                NULL,
                                &ViewSize,
                                ViewUnmap,
                                0L,
                                PAGE_READWRITE
                               );

}

* Changes: 29.05.2020  - Moved from ldntvdm.c to seperate module for better code
*                        maintainability
*/
#include "ldntvdm.h"
#include "consbmp.h"
#include "iathook.h"
#include "reg.h"
#include "symcache.h"

#if !defined(TARGET_WINXP) && defined(_WIN64)

typedef PVOID(WINAPI *RtlAllocateHeapFunc)(PVOID  HeapHandle, ULONG  Flags, SIZE_T Size);
RtlAllocateHeapFunc RtlAllocateHeapReal;
PVOID WINAPI RtlAllocateHeapHook(PVOID  HeapHandle, ULONG  Flags, SIZE_T Size)
{
#ifdef TARGET_WIN7
	if (Size == 0x130) /* Any better idea to find correct call? */ Flags |= HEAP_ZERO_MEMORY;
#else
	if (Size == 0x150) /* Any better idea to find correct call? */ Flags |= HEAP_ZERO_MEMORY;
#endif
	return RtlAllocateHeapReal(HeapHandle, Flags, Size);
}

#ifndef TARGET_WIN7
typedef HMODULE(WINAPI *LoadLibraryExWFunc)(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags);
LoadLibraryExWFunc LoadLibraryExWReal;
HMODULE WINAPI LoadLibraryExWHook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags)
{
	HANDLE hRet = LoadLibraryExW(lpLibFileName, hFile, dwFlags);

	TRACE("LDNTVDM: LoadLibraryExWHook(%S)\n", lpLibFileName);
	if (hRet && __wcsicmp((PWCHAR)lpLibFileName, L"ConHostV1.dll") == 0)
	{
		TRACE("LDNTVDM hooks Conhost RtlAllocateHeap\n");
		Hook_IAT_x64_IAT((LPBYTE)hRet, "ntdll.dll", "RtlAllocateHeap", RtlAllocateHeapHook, (PULONG_PTR)&RtlAllocateHeapReal);
	}
	return hRet;
}
#endif

#ifdef HOOK_CONHOSTV2
#include "xpwrap.h"
#include "ntpsapi.h"
DWORD GetParentProcessName(HANDLE hModule, LPSTR lpFilename, DWORD nSize)
{
	xpPROCESS_BASIC_INFORMATION BasicInfo;
	NTSTATUS status;
	HANDLE hProcess;
	DWORD dwRet;

	status = NtQueryInformationProcess(
		hModule,
		ProcessBasicInformation,
		&BasicInfo,
		sizeof(BasicInfo),
		NULL
		);
	if (!NT_SUCCESS(status)) return 0;
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, (DWORD)BasicInfo.InheritedFromUniqueProcessId);
	if (!hProcess) return 0;
	dwRet = GetModuleFileNameA(hProcess, lpFilename, nSize);
	CloseHandle(hProcess);
	return dwRet;
}

static BOOL IsV2ParentProcess(void)
{
	char szProcess[MAX_PATH];
	char *pszExcludedProcesses[] = {
		"wsl.exe",
		"wslhost.exe",
		"debian.exe"
	};
	size_t len;
	int i, proclen;

	if (proclen = GetParentProcessName(GetCurrentProcess(), szProcess, sizeof(szProcess)))
	{
		TRACE("LDNTVDM: Parent process is %s\n", szProcess)
		for (i = 0; i < sizeof(pszExcludedProcesses) / sizeof(pszExcludedProcesses[0]); i++)
		{
			len = strlen(pszExcludedProcesses[i]);
			TRACE("%s = %s?\n", szProcess + proclen - len, pszExcludedProcesses[i]);
			if (proclen >= len && strcmp(szProcess + proclen - len, pszExcludedProcesses[i]) == 0)
				return TRUE;
		}
	}
	return FALSE;
}

/* The key for forcing V1 console is in HKEY_CURRENT_USER, thus it cannot be set globally during setup.
 * Therefore we patch ShouldUseConhostV2 function in conhost to always return FALSE so that ntvdmx64
 * works for all users on the system, as long as the loader is installed.
 * If admin doesn't want to have its console V2 disabled globally (i.e users not using ntvdmx64), Admin
 * can create ObeyForceV2 DWORD-value in ldntvdm configuration key and we will leave conhost alone 
 */
void ForceV1Console(HMODULE hConhost)
{
	DWORD dwAddress;
	HKEY hKey;

	if (NT_SUCCESS(REG_OpenLDNTVDM(KEY_READ, &hKey)))
	{
		if ((!NT_SUCCESS(REG_QueryDWORD(hKey, L"ObeyForceV2", &dwAddress)) || dwAddress == 0) &&
			!IsV2ParentProcess() &&
			SymCache_GetDLLKey(hKey, L"conhost.exe", FALSE) &&
			NT_SUCCESS(REG_QueryDWORD(hKey, L"ShouldUseConhostV2", &dwAddress)))
		{
			DWORD OldProt;
			ULONG_PTR Address = (ULONG_PTR)hConhost + dwAddress;

			OldProt = PAGE_EXECUTE_READWRITE;
			if (VirtualProtect((LPVOID)Address, sizeof(DWORD), OldProt, &OldProt))
			{
				*(DWORD*)Address = 0x24C3C031; // xor eax, eax; retn
				VirtualProtect((LPVOID)Address, sizeof(DWORD), OldProt, &OldProt);
				TRACE("ShouldUseConhostV2 hook installed @%08X\n", Address);
			}
			else TRACE("ShouldUseConhostV2 hook: VirtualProtect failed\n");
		}
		REG_CloseKey(hKey);
	}
}
#endif

/* Returns true if the target system doesn't have a conhostv1.dll to directly use */
BOOL ConsBmpBug_Install(HMODULE *phmodConhost)
{
	BOOL fNoConhostDll = FALSE;
	HMODULE hConhost = GetModuleHandle(NULL);

#ifdef HOOK_CONHOSTV2
	ForceV1Console(hConhost);
#endif
#ifdef TARGET_WIN7
	ConsBmp_Install(hConhost);
	Hook_IAT_x64_IAT((LPBYTE)hConhost, "ntdll.dll", "RtlAllocateHeap", RtlAllocateHeapHook, (PULONG_PTR)&RtlAllocateHeapReal);
	return TRUE;
#else
	if (*phmodConhost = GetModuleHandle(_T("ConHostV1.dll")))
		Hook_IAT_x64_IAT((LPBYTE)*phmodConhost, "ntdll.dll", "RtlAllocateHeap", RtlAllocateHeapHook, (PULONG_PTR)&RtlAllocateHeapReal);
	else
	{
		char szFile[MAX_PATH];

		GetSystemDirectoryA(szFile, sizeof(szFile) / sizeof(szFile[0]));
		strcat(szFile, "\\ConhostV1.dll");
		if (fNoConhostDll = GetFileAttributesA(szFile) == 0xFFFFFFFF) // Windows 8
			Hook_IAT_x64_IAT((LPBYTE)hConhost, "ntdll.dll", "RtlAllocateHeap", RtlAllocateHeapHook, (PULONG_PTR)&RtlAllocateHeapReal);
		else
			Hook_IAT_x64_IAT((LPBYTE)hConhost, "api-ms-win-core-libraryloader-l1-2-0.dll", "LoadLibraryExW", LoadLibraryExWHook, (PULONG_PTR)&LoadLibraryExWReal);
	}
	return fNoConhostDll;
#endif /* TARGET_WIN7 */
}
#endif /* TARGET_WINXP */
