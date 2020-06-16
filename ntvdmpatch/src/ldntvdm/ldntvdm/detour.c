/* Project: ldntvdm
 * Module : detour
 * Author : leecher@dose.0wnz.at
 * Descr. : This module implements simple functions for creating an inline
 *          hook in a module
 * Changes: 01.07.2017  - Created
 */

#include "ldntvdm.h"
#include "iathook.h"
#include <inttypes.h>

#if defined(USE_SYMCACHE) || defined(TARGET_WINXP) || defined(EXTRACTICON_HOOK)

static PBYTE Hook_FindAddr(ULONG_PTR src)
{
	MEMORY_BASIC_INFORMATION mbi;
	ULONG_PTR addr, min = src>0x80000000 ? src - 0x80000000 : 0, i;

	RtlZeroMemory(&mbi, sizeof(mbi));
	for (addr = src, i = 0; addr > min && i < 1000; addr = (ULONG_PTR)mbi.BaseAddress - 1, i++)
	{
		if (VirtualQuery((LPCVOID)addr, &mbi, sizeof(mbi)) != sizeof(MEMORY_BASIC_INFORMATION))
		{
			TRACE("VirtualQuery failed: %08X\n", GetLastError());
			break;
		}

		if (mbi.State == MEM_FREE)
		{
			PBYTE ret;
			TRACE("About to alloc page @%"PRIxPTR"\n", mbi.BaseAddress);
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
	PIMAGE_SECTION_HEADER Sect = GetEnclosingSectionHeader((DWORD)((LPBYTE)src - hMod), NtHeaders);

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

static size_t Hook_DetermineLength(PBYTE Address, DWORD dwSizeReq)
{
	size_t LenCount = 0, Len = 0;
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

BOOL Hook_EnoughSpace(PBYTE Address, DWORD dwSizeReq)
{
	PBYTE t, pEnd = Address + dwSizeReq;

	if (!IsBadReadPtr(Address, dwSizeReq))
	{
		for (t = Address; t < pEnd; t++) if (*t != 0xCC && *t != 0x90) break;
		return t == pEnd;
	}
	return FALSE;
}

LPBYTE Hook_Inline(PVOID src, PVOID tgt)
{
	// Find free region for allocation
	DWORD cbContext;
	PBYTE context = NULL, p;
	DWORD flOld = 0, OldProt = 0, dwOrigSize;
	static DWORD SectOffset = 0;

	TRACE("Hook_Inline(%"PRIxPTR", %"PRIxPTR")\n", src, tgt);

#ifndef _WIN64
	// If this is a Hot-patchable function, we can save ourselves a lot of work
	if (*((WORD*)src) == 0xFF8B)
	{
		p = (PBYTE)src - sizeof(ULONG_PTR) - 1; // JMP ....
		if (Hook_EnoughSpace(p, (PBYTE)src - p) && VirtualProtect(p, (PBYTE)src - p + 2, PAGE_READWRITE, &OldProt))
		{
			/* Looks good, simple trampoline:
			 * JMP tgt     E9 xx xx xx xx
			 *
			 * Patched function header:
			 * JMP $-6     EB F9
			 * <orig fun>  ...
			 */
			*p = 0xE9;
			*((PDWORD)(p + 1)) = (DWORD)tgt - (DWORD)p - 5;
			*((PWORD)(p + 5)) = 0xF9EB;
			VirtualProtect((LPVOID)p, (PBYTE)src - p + 2, OldProt, &OldProt);
			FlushInstructionCache(GetCurrentProcess(), NULL, 0);
			TRACE("Hook_Inline did hotpatch -> context=%"PRIxPTR"\n", p + 7);
			return p + 7;
		}
	}
#endif

	// Determine dwOrigSize with LDASM. We need at least 2 + sizeof(DWORD) bytes 
	dwOrigSize = (DWORD)Hook_DetermineLength(src, 2 + sizeof(DWORD));
	TRACE("dwOrigSize detected: %d\n", dwOrigSize);
	cbContext = dwOrigSize + 1 + sizeof(DWORD) + sizeof(ULONG_PTR);

	/* First check if there is enough space before the function */
	context = (PBYTE)src - cbContext;
	if (Hook_EnoughSpace(context, cbContext) &&
		VirtualProtect(context, cbContext, PAGE_READWRITE, &OldProt))
	{
		TRACE("Hook: Enough space before function\n")
	}
	else
	{
		// Reuse wasted space at the end of text section if possible
		if (!(context = Hook_ReserveSpace(src, cbContext)))
		{
			if (!(context = Hook_FindAddr((ULONG_PTR)src)))
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
	}

	/* Trampoline looks as follows:
	* Original inst     xx xx ...
	* JMP <orig fun>    E9 xx xx xx xx
	* <FAR tgtaddr>     xx xx xx xx (XX XX XX XX)

	* Patched function header:
	* JMP *tgtaddr      FF 25 xx xx xx xx
	* <orig fun>        ...
	*/

	// Save original. Length check missing
	RtlMoveMemory(context, src, dwOrigSize);
	p = context + dwOrigSize;
	*p = 0xE9;
	*(DWORD*)(++p) = (DWORD)((ULONG_PTR)src - (ULONG_PTR)context - 5);
	p += sizeof(DWORD);

	// final jump address
	*((ULONG_PTR*)p) = (ULONG_PTR)tgt;
	p += sizeof(ULONG_PTR);

	if (OldProt) VirtualProtect(context, cbContext, OldProt, &OldProt);

	// write hook
	if (!VirtualProtect(src, 2 + sizeof(DWORD), PAGE_EXECUTE_READWRITE, &flOld))
	{
		TRACE("Hook_Inline failed: VirtualProtect err=%08X\n", GetLastError());
		return 0;
	}

	// Build jump (jmp [rip - offset])
	p = src;
	*p = 0xFF;
	*(++p) = 0x25;
#ifdef _WIN64
	*(PDWORD)(++p) = (DWORD)((ULONG_PTR)context - (ULONG_PTR)src + dwOrigSize + 1 + sizeof(DWORD) - 2 - sizeof(DWORD));
#else
	*(PDWORD)(++p) = (DWORD)((ULONG_PTR)context + dwOrigSize + 1 + sizeof(DWORD));
#endif

	VirtualProtect(src, 2 + sizeof(DWORD), flOld, &flOld);

	TRACE("Hook_Inline context=%"PRIxPTR"\n", context);
	return context;
}
#endif
