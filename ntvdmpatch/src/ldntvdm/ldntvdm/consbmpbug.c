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

#ifndef TARGET_WINXP

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


/* Returns true if the target system doesn't have a conhostv1.dll to directly use */
BOOL ConsBmpBug_Install(HMODULE *phmodConhost)
{
	BOOL fNoConhostDll = FALSE;

#ifdef TARGET_WIN7
#ifdef _WIN64
	ConsBmp_Install();
#endif
	Hook_IAT_x64_IAT((LPBYTE)GetModuleHandle(NULL), "ntdll.dll", "RtlAllocateHeap", RtlAllocateHeapHook, (PULONG_PTR)&RtlAllocateHeapReal);
#else
	if (*phmodConhost = GetModuleHandle(_T("ConHostV1.dll")))
		Hook_IAT_x64_IAT((LPBYTE)*phmodConhost, "ntdll.dll", "RtlAllocateHeap", RtlAllocateHeapHook, (PULONG_PTR)&RtlAllocateHeapReal);
	else
	{
		char szFile[MAX_PATH];

		GetSystemDirectoryA(szFile, sizeof(szFile) / sizeof(szFile[0]));
		strcat(szFile, "\\ConhostV1.dll");
		if (fNoConhostDll = GetFileAttributesA(szFile) == 0xFFFFFFFF) // Windows 8
			Hook_IAT_x64_IAT((LPBYTE)GetModuleHandle(NULL), "ntdll.dll", "RtlAllocateHeap", RtlAllocateHeapHook, (PULONG_PTR)&RtlAllocateHeapReal);
		else
			Hook_IAT_x64_IAT((LPBYTE)GetModuleHandle(NULL), "api-ms-win-core-libraryloader-l1-2-0.dll", "LoadLibraryExW", LoadLibraryExWHook, (PULONG_PTR)&LoadLibraryExWReal);
	}
#endif /* TARGET_WIN7 */
	return fNoConhostDll;
}
#endif /* TARGET_WINXP */
