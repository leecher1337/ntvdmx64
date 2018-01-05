/* Project: ldntvdm
* Module : detour
* Author : leecher@dose.0wnz.at
* Descr. : This module implements simple functions for creating an inline
*          hook in KERNEL32.dll
* Changes: 01.07.2017  - Created
*/

#include "ldntvdm.h"

#ifdef TARGET_WIN7

static PBYTE Hook_FindAddr(PVOID src)
{
	MEMORY_BASIC_INFORMATION mbi;
	ULONG_PTR addr, min = src>0x80000000 ? (ULONG_PTR)src - 0x80000000 : 0, i;

	RtlZeroMemory(&mbi, sizeof(mbi));
	for (addr = (ULONG_PTR)src, i = 0; addr > min && i < 1000; addr = (ULONG_PTR)mbi.BaseAddress - 1, i++)
	{
		if (VirtualQuery((LPCVOID)addr, &mbi, sizeof(mbi)) != sizeof(MEMORY_BASIC_INFORMATION))
		{
			TRACE("VirtualQuery failed: %08X", GetLastError());
			break;
		}

		if (mbi.State == MEM_FREE)
		{
			PBYTE ret;
			TRACE("About to alloc page @%08X", mbi.BaseAddress);
			if ((ret = VirtualAlloc(mbi.BaseAddress, 0x1000, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE)))
				return ret;
		}
	}
	return NULL;
}
static PBYTE Hook_ReserveSpace(PVOID src, DWORD cbPatch)
{
	LPBYTE hMod = (LPBYTE)GetModuleHandle(_T("kernel32.dll"));
	PIMAGE_DOS_HEADER DosHeader = (PIMAGE_DOS_HEADER)hMod;
	PIMAGE_NT_HEADERS NtHeaders = (PIMAGE_NT_HEADERS)(hMod + DosHeader->e_lfanew);
	PIMAGE_SECTION_HEADER Sect = GetEnclosingSectionHeader((LPBYTE)src - hMod, NtHeaders);

	/* Is there enough room at the end of the section? */
	if (Sect && (Sect->SizeOfRawData - Sect->Misc.VirtualSize) >= cbPatch)
		return hMod + Sect->VirtualAddress + Sect->Misc.VirtualSize;
	return 0;
}

#ifndef _WIN64
#pragma comment(lib, "LDE64")
size_t __stdcall LDE(const LPVOID lpData, unsigned int size);
#else
#pragma comment(lib, "LDE64x64")
size_t __fastcall LDE(const LPVOID lpData, unsigned int size);
#endif

static DWORD Hook_DetermineLength(PBYTE Address, DWORD dwSizeReq)
{
	ULONG LenCount = 0, Len = 0;
	while (LenCount <= dwSizeReq) //at least 15 bytes
	{
#ifdef _WIN64
		Len = LDE(Address, 64);
#else
		Len = LDE(Address, 0);
#endif
		Address = Address + Len;
		LenCount = LenCount + Len;
	}
	return LenCount;
}

LPBYTE Hook_Inline(PVOID src, PVOID tgt)
{
	// Find free region for allocation
	DWORD cbContext;
	PBYTE context = NULL, p;
	DWORD flOld = 0, OldProt = 0, dwOrigSize;
	static DWORD SectOffset = 0;

	TRACE("Hook_Inline(%08X, %08X, %d)", src, tgt, dwOrigSize);

	// Determine dwOrigSize with LDASM. We need at least 2 + sizeof(DWORD) bytes 
	dwOrigSize = Hook_DetermineLength(src, 2 + sizeof(DWORD));
	TRACE("dwOrigSize detected: %d\n", dwOrigSize);
	cbContext = dwOrigSize + 1 + sizeof(DWORD) + sizeof(ULONG_PTR) + 2 + sizeof(DWORD);

	// Reuse wasted space at the end of text section if possible
	if (!(context = Hook_ReserveSpace(src, cbContext)))
	{
		if (!(context = Hook_FindAddr(src)))
		{
			TRACE("Hook_FindAddr failed.\n");
			return NULL;
		}
	}
	else
	{
		context += SectOffset;
		SectOffset += cbContext;
		VirtualProtect(context, cbContext, PAGE_READWRITE, &OldProt);
	}

	// Save original. Length check missing
	RtlMoveMemory(context, src, dwOrigSize);
	p = context + dwOrigSize;
	*p = 0xE9;
	*(DWORD*)(++p) = (DWORD)((ULONG_PTR)src - (ULONG_PTR)context - 5);
	p += sizeof(DWORD);

	// final jump address
	*((ULONG_PTR*)p) = tgt;
	p += sizeof(ULONG_PTR);

	// Build jump (jmp [rip - offset])
	*p = 0xFF;
	*(++p) = 0x25;
#ifdef _WIN64
	*(PDWORD)(++p) = (DWORD)((ULONG_PTR)context - (ULONG_PTR)src + dwOrigSize + 1 + sizeof(DWORD) - 2 - sizeof(DWORD));
#else
	*(PDWORD)(++p) = (DWORD)((ULONG_PTR)context + dwOrigSize + 1 + sizeof(DWORD));
#endif

	if (OldProt) VirtualProtect(context, cbContext, OldProt, &OldProt);

	// write hook
	if (!VirtualProtect(src, 2 + sizeof(DWORD), PAGE_EXECUTE_READWRITE, &flOld))
	{
		TRACE("Hook_Inline failed: VirtualProtect err=%08X", GetLastError());
		return 0;
	}
	RtlMoveMemory(src, context + dwOrigSize + 1 + sizeof(DWORD) + sizeof(ULONG_PTR), 2 + sizeof(DWORD));
	VirtualProtect(src, 2 + sizeof(DWORD), flOld, &flOld);

	TRACE("Hook_Inline context=%08X", context);
	return context;
}
#endif
