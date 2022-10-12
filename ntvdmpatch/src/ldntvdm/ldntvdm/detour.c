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

struct _HOOK_HEAPPTR;
struct _HOOK_HEAPPTR {
	struct _HOOK_HEAPPTR *Next;
	HANDLE hProcess;
	PVOID BaseAddress;
	DWORD Offset;
	PBYTE pMem;
};
typedef struct _HOOK_HEAPPTR HOOK_HEAPPTR;
HOOK_HEAPPTR *m_pHeapPtrList = NULL;

static HOOK_HEAPPTR *Hook_AddrList_Find(HANDLE hProcess, PVOID BaseAddress, DWORD cbReqSize)
{
	HOOK_HEAPPTR *ptr;

	for (ptr = m_pHeapPtrList; ptr; ptr = ptr->Next)
	{
		if (ptr->hProcess == hProcess && ptr->BaseAddress == BaseAddress)
		{
			if (0x1000 - ptr->Offset > cbReqSize) return ptr;
		}
	}
	return NULL;
}

static PVOID Hook_AddrList_Reserve(HOOK_HEAPPTR *pList, DWORD cbReqSize)
{
	PVOID pRet = pList->pMem + pList->Offset;
	pList->Offset += cbReqSize;
	return pRet;
}

static HOOK_HEAPPTR *Hook_AddrList_Add(HANDLE hProcess, PVOID BaseAddress, DWORD cbReqSize, PBYTE pMem)
{
	HOOK_HEAPPTR **ppNext = &m_pHeapPtrList;

	while (*ppNext) ppNext = &(*ppNext)->Next;
	if (!(*ppNext = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(HOOK_HEAPPTR)))) return NULL;
	(*ppNext)->hProcess = hProcess;
	(*ppNext)->BaseAddress = BaseAddress;
	(*ppNext)->pMem = pMem;
	return *ppNext;
}

static PBYTE Hook_FindAddr(HANDLE hProcess, ULONG_PTR src, DWORD cbReqSize)
{
	MEMORY_BASIC_INFORMATION mbi;
	ULONG_PTR addr, min = src>0x80000000 ? src - 0x80000000 : 0, i;
	HOOK_HEAPPTR *pList;

	RtlZeroMemory(&mbi, sizeof(mbi));
	for (addr = src, i = 0; addr > min && i < 1000; addr = (ULONG_PTR)mbi.BaseAddress - 1, i++)
	{
		if (VirtualQueryEx(hProcess, (LPCVOID)addr, &mbi, sizeof(mbi)) != sizeof(MEMORY_BASIC_INFORMATION))
		{
			TRACE("VirtualQuery failed: %08X\n", GetLastError());
			break;
		}

		if (mbi.State == MEM_FREE)
		{
			PBYTE ret;
			TRACE("About to alloc page @%"PRIxPTR"\n", mbi.BaseAddress);
			if ((ret = VirtualAllocEx(hProcess, mbi.BaseAddress, 0x1000, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE)))
			{
				if (pList = Hook_AddrList_Add(hProcess, mbi.BaseAddress, cbReqSize, ret))
					return Hook_AddrList_Reserve(pList, cbReqSize);
			}
		}
		else
		{
			// If we already have reserved space and it's big enough, just reuse the reserved page
			if (pList = Hook_AddrList_Find(hProcess, mbi.BaseAddress, cbReqSize))
				return Hook_AddrList_Reserve(pList, cbReqSize);
		}
	}
	return NULL;
}
static PBYTE Hook_ReserveSpace(HANDLE hModule, PVOID src, DWORD cbPatch)
{
	LPBYTE hMod = (LPBYTE)hModule, ret = 0;
	PIMAGE_DOS_HEADER DosHeader = (PIMAGE_DOS_HEADER)hMod;
	PIMAGE_NT_HEADERS NtHeaders = (PIMAGE_NT_HEADERS)(hMod + DosHeader->e_lfanew);
	PIMAGE_SECTION_HEADER Sect = GetEnclosingSectionHeader((DWORD)((LPBYTE)src - hMod), NtHeaders);

	/* Is there enough room at the end of the section? */
	if (Sect && (Sect->SizeOfRawData - Sect->Misc.VirtualSize) >= cbPatch)
	{
		/* Check if already in hook address list */
		HOOK_HEAPPTR *pList;

		if (Hook_AddrList_Find(GetCurrentProcess(), (PVOID)Sect->VirtualAddress, 0))
		{
			if (pList = Hook_AddrList_Find(GetCurrentProcess(), (PVOID)Sect->VirtualAddress, cbPatch))
				return Hook_AddrList_Reserve(pList, cbPatch);
			return FALSE; // Not enough space left
		}
		else
		{
			if (pList = Hook_AddrList_Add(GetCurrentProcess(), (PVOID)Sect->VirtualAddress, cbPatch, hMod + Sect->VirtualAddress))
			{
				pList->Offset = Sect->Misc.VirtualSize;
				return Hook_AddrList_Reserve(pList, cbPatch);
			}
		}
		//ret = hMod + Sect->VirtualAddress + Sect->Misc.VirtualSize;
	}
	return ret;
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

BOOL Hook_EnoughSpace(HANDLE hProcess, PBYTE Address, DWORD dwSizeReq)
{
	PBYTE t, pEnd;
	BYTE buffer[128]; // Just assuming it, as this is enough for our internal needs

	if (hProcess == GetCurrentProcess())
	{
		if (IsBadReadPtr(Address, dwSizeReq)) return FALSE;
		t = Address;
		pEnd = Address + dwSizeReq;
	}
	else
	{
		SIZE_T dwRead;

		if (dwSizeReq > sizeof(buffer) ||
			!ReadProcessMemory(hProcess, Address, buffer, dwSizeReq, &dwRead) || dwRead != dwSizeReq)
			return FALSE;
		t = buffer;
		pEnd = buffer + dwSizeReq;
	}

	for (; t < pEnd; t++) if (*t != 0xCC && *t != 0x90) break;
	return t == pEnd;
}

// ptr_size default: ULONG_PTR
LPBYTE Hook_Inline_Func(HANDLE hProcess, HANDLE hModule, PVOID src, PVOID tgt, SIZE_T ptr_size, const char *pszHookName)
{
	// Find free region for allocation
	DWORD cbContext;
	PBYTE context = NULL, p, dst_ptr;
	DWORD flOld = 0, OldProt = 0, dwOrigSize;
	HANDLE hCurrentProcess = GetCurrentProcess();
	PVOID src_ptr;
	BYTE buffer[128];
	DWORD len, result;
	SIZE_T dwRead;

	TRACE("Hook_Inline(%"PRIxPTR", %"PRIxPTR", %s)\n", src, tgt, pszHookName);

	len = sizeof(DWORD) + 1; // JMP ....
	if (hProcess == hCurrentProcess)
	{
		src_ptr = src;
	}
	else
	{
		if (!ReadProcessMemory(hProcess, (PBYTE)src - len, buffer, 32 + len, &dwRead))
			return NULL;
		src_ptr = buffer + len;
	}

	// If this is a Hot-patchable function, we can save ourselves a lot of work
	if (ptr_size == sizeof(DWORD) && *((WORD*)src_ptr) == 0xFF8B)
	{
		if (Hook_EnoughSpace(hProcess, (PBYTE)src - len, len) && VirtualProtectEx(hProcess, (PBYTE)src - len, len + 2, PAGE_EXECUTE_READWRITE, &OldProt))
		{
			/* Looks good, simple trampoline:
			 * JMP tgt     E9 xx xx xx xx
			 *
			 * Patched function header:
			 * JMP $-6     EB F9
			 * <orig fun>  ...
			 */
			p = (PBYTE)src_ptr - len; // JMP ....
			*p = 0xE9;
			*((PDWORD)(p + 1)) = (DWORD)tgt - (DWORD)src;
			*((PWORD)(p + 5)) = 0xF9EB;
			if (hProcess != hCurrentProcess) result = WriteProcessMemory(hProcess, (PBYTE)src - len, buffer, len + 2, &dwRead); else result = TRUE;
			VirtualProtectEx(hProcess, (PBYTE)src - len, len + 2, OldProt, &OldProt);
			FlushInstructionCache(hProcess, NULL, 0);
			if (result)
			{
				TRACE("Hook_Inline did hotpatch -> context=%"PRIxPTR"\n", p + 7);
				return (PBYTE)src + 2;
			}
		}
	}

	// Determine dwOrigSize with LDASM. We need at least 2 + sizeof(DWORD) bytes 
	dwOrigSize = (DWORD)Hook_DetermineLength(src_ptr, 2 + sizeof(DWORD));
	TRACE("dwOrigSize detected: %d\n", dwOrigSize);
	cbContext = dwOrigSize + 1 + sizeof(DWORD) + ptr_size;

	/* First check if there is enough space before the function */
	context = (PBYTE)src - cbContext;
	if (Hook_EnoughSpace(hProcess, context, cbContext) &&
		VirtualProtectEx(hProcess, context, cbContext, PAGE_EXECUTE_READWRITE, &OldProt))
	{
		TRACE("Hook: Enough space before function\n")
	}
	else
	{
		// Reuse wasted space at the end of text section if possible
		if (hProcess != hCurrentProcess || !(context = Hook_ReserveSpace(hModule, src, cbContext)))
		{
			if (!(context = Hook_FindAddr(hProcess, (ULONG_PTR)src, cbContext)))
			{
				TRACE("Hook_FindAddr failed.\n");
				return NULL;
			}
		}
		else
		{
			VirtualProtectEx(hProcess, context, cbContext, PAGE_EXECUTE_READWRITE, &OldProt);
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
	if (hProcess != hCurrentProcess) dst_ptr = buffer; else dst_ptr = context;
	RtlMoveMemory(dst_ptr, src_ptr, dwOrigSize);
	p = dst_ptr + dwOrigSize;
	*p = 0xE9;
	*(DWORD*)(++p) = (DWORD)((ULONG_PTR)src - (ULONG_PTR)context - len);
	p += sizeof(DWORD);

	// final jump address
	if (ptr_size == sizeof(ULONGLONG))
	{
		*((ULONGLONG*)p) = (ULONG_PTR)tgt;
		p += sizeof(ULONGLONG);
	}
	else
	{
		*((DWORD*)p) = (DWORD)tgt;
		p += sizeof(DWORD);
	}
	if (hProcess != hCurrentProcess && !WriteProcessMemory(hProcess, context, dst_ptr, cbContext, &dwRead)) return FALSE;
	if (OldProt) VirtualProtectEx(hProcess, context, cbContext, OldProt, &OldProt);

	// write hook
	len = 2 + sizeof(DWORD);
	if (!VirtualProtectEx(hProcess, src, len, PAGE_EXECUTE_READWRITE, &flOld))
	{
		TRACE("Hook_Inline failed: VirtualProtect err=%08X\n", GetLastError());
		return 0;
	}

	// Build jump (jmp [rip - offset])
	p = hProcess != hCurrentProcess?buffer:src;
	*p = 0xFF;
	*(++p) = 0x25;
	if (ptr_size == sizeof(ULONGLONG))
		*(PDWORD)(++p) = (DWORD)((ULONG_PTR)context - (ULONG_PTR)src + dwOrigSize + 1 + sizeof(DWORD) - 2 - sizeof(DWORD));
	else
		*(PDWORD)(++p) = (DWORD)((ULONG_PTR)context + dwOrigSize + 1 + sizeof(DWORD));

	if (hProcess != hCurrentProcess && !WriteProcessMemory(hProcess, src, buffer, len, &dwRead)) return FALSE;
	VirtualProtectEx(hProcess, src, len, flOld, &flOld);

	TRACE("Hook_Inline context=%"PRIxPTR"\n", context);
	return context;
}
#endif
