/* Project: ldntvdm
 * Module : iathook
 * Author : leecher@dose.0wnz.at, some stuff from Internet, possibly by 
 *          Matt Pietrek?
 * Descr. : This module implements simple functions for hooking the Import
 *          address table. This also supports delay imports, which is necessary
 *          to hook some KERNEL32.DLL functions.
 * Changes: 01.04.2016  - Created
 */

#include "ldntvdm.h"
#include <delayimp.h>

#define GetImgDirEntryRVA( pNTHdr, IDE ) (pNTHdr->OptionalHeader.DataDirectory[IDE].VirtualAddress)
#define GetImgDirEntrySize( pNTHdr, IDE ) (pNTHdr->OptionalHeader.DataDirectory[IDE].Size)

PIMAGE_SECTION_HEADER GetEnclosingSectionHeader(DWORD rva, PIMAGE_NT_HEADERS pNTHeader)
{
	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(pNTHeader);
	unsigned int i;

	for (i = 0; i < pNTHeader->FileHeader.NumberOfSections; i++, section++)
	{
		// This 3 line idiocy is because Watcom's linker actually sets the
		// Misc.VirtualSize field to 0. (!!! - Retards....!!!)
		DWORD size = section->Misc.VirtualSize;
		if (0 == size)
			size = section->SizeOfRawData;

		// Is the RVA within this section?
		if ((rva >= section->VirtualAddress) && (rva < (section->VirtualAddress + size)))
			return section;
	}

	return 0;
}

#define MakeDelta(cast, x, y) (cast) ( (DWORD_PTR)(x) - (DWORD_PTR)(y)) 
unsigned long GetMappedSectionOffset(IMAGE_NT_HEADERS *ntHd, IMAGE_SECTION_HEADER *seHd, void *base)
{
	IMAGE_SECTION_HEADER *section = IMAGE_FIRST_SECTION(ntHd);
	unsigned int i;
	unsigned long offset = MakeDelta(unsigned long, section, base);

	for (i = 0; i < ntHd->FileHeader.NumberOfSections; i++, section++)
	{
		if (section->Name == seHd->Name)
		{
			offset = MakeDelta(unsigned long, section->VirtualAddress, section->PointerToRawData);
			break;
		}

		//offset += (section->SizeOfRawData > ntHd->OptionalHeader.SectionAlignment ?
		// section->SizeOfRawData - ntHd->OptionalHeader.SectionAlignment :
		// ntHd->OptionalHeader.SectionAlignment - section->SizeOfRawData);
	}

	return offset;
}

LPVOID GetPtrFromRVA(DWORD rva, IMAGE_NT_HEADERS *pNTHeader, PBYTE imageBase)
{
	PIMAGE_SECTION_HEADER pSectionHdr;
	INT delta;
	unsigned long offset = 0;

	pSectionHdr = GetEnclosingSectionHeader(rva, pNTHeader);

	delta = (INT)(pSectionHdr->VirtualAddress - pSectionHdr->PointerToRawData);
	offset = GetMappedSectionOffset(pNTHeader, pSectionHdr, imageBase);
	return (PVOID)(imageBase + rva - delta + offset);
}

LPVOID GetPtrFromVA(PVOID ptr, IMAGE_NT_HEADERS *pNTHeader, PBYTE pImageBase) // 'T' = PIMAGE_NT_HEADERS 
{
	// Yes, under Win64, we really are lopping off the high 32 bits of a 64 bit
	// value.  We'll knowingly believe that the two pointers are within the
	// same load module, and as such, are RVAs
	DWORD rva = PtrToLong((PBYTE)ptr - pNTHeader->OptionalHeader.ImageBase);

	return GetPtrFromRVA(rva, pNTHeader, pImageBase);
}

PIMAGE_IMPORT_DESCRIPTOR IAT_FindLibrary(LPBYTE hMod, char *LibNameBigCaseName_SmallFormat)
{
	PIMAGE_DOS_HEADER DosHeader = (PIMAGE_DOS_HEADER)hMod;
	PIMAGE_NT_HEADERS NtHeaders = (PIMAGE_NT_HEADERS)(hMod + DosHeader->e_lfanew);
	PIMAGE_IMPORT_DESCRIPTOR idata = (PIMAGE_IMPORT_DESCRIPTOR)(hMod + NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
	char *pszDLL;

	for (; idata->Name; idata++) {
		pszDLL = (char*)(hMod + idata->Name);
		if (!__stricmp(pszDLL, LibNameBigCaseName_SmallFormat)) return idata;
	}
	return NULL;
}

int IAT_FindFunction(LPBYTE hMod, PIMAGE_IMPORT_DESCRIPTOR idata, char *FunName, PULONG_PTR *ppAddress)
{
	PIMAGE_THUNK_DATA ThunkData = (PIMAGE_THUNK_DATA)(hMod + idata->OriginalFirstThunk);
	PULONG_PTR Address = (PULONG_PTR)(hMod + idata->FirstThunk);
	int i;

	for (i = 0; ThunkData->u1.ForwarderString; i++) {
		if (!(ThunkData->u1.AddressOfData & IMAGE_ORDINAL_FLAG))
		{
			char *pszIATFunName = (char*)(hMod + ThunkData->u1.ForwarderString + 2);
			if (IsBadStringPtrA(pszIATFunName, 4096))
			{
				TRACE("LDNTVDM: Cannot check for IAT entry %s of module @%08X. Thunk=%08X, AddressOfData=%08X, ForwarderString=%08X\n",
					FunName, hMod, ThunkData, ThunkData->u1.AddressOfData, ThunkData->u1.ForwarderString);
			}
			else if (!_strcmp(pszIATFunName, FunName))
			{
				if (Address[i] == ThunkData->u1.AddressOfData)
				{
					TRACE("IAT entry is not bound yet\n");
					return -3;
				}
				*ppAddress = &Address[i];
				return 0;
			}
		} ThunkData++;
	}
	return -2;
}

int IAT_SetHook(PULONG_PTR Address, LPVOID NewFun, PULONG_PTR OldFun)
{
	DWORD OldProt;
	MEMORY_BASIC_INFORMATION mbi;

	OldProt = PAGE_READWRITE;
#ifdef TARGET_WINXP
	// Windows XP has IAT on executable page, so better first check
	if (VirtualQuery(Address, &mbi, sizeof(mbi)) && mbi.Protect >= PAGE_EXECUTE)
		OldProt = PAGE_EXECUTE_READWRITE;
#endif
	VirtualProtect(Address, sizeof(ULONG_PTR), OldProt, &OldProt);
	if (OldFun) *OldFun = *Address;
	TRACE("Hooked %08X -> %08X\n", *Address, NewFun);
	*Address = (ULONG_PTR)NewFun;
	VirtualProtect(Address, sizeof(ULONG_PTR), OldProt, &OldProt);
	return 0;
}

/* -1	-	Library not found
 * -2	-	Entry point not found
 * -3	-	IAT entry not bound yet
 */
int Hook_IAT_x64_IAT(LPBYTE hMod, char LibNameBigCaseName_SmallFormat[], char FunName[], LPVOID NewFun, PULONG_PTR OldFun) 
{
	PIMAGE_IMPORT_DESCRIPTOR idata;
	PULONG_PTR Address;
	int iRet = -1;

	TRACE("Hook_IAT_x64_IAT(%X, %s, %s, %08X, %08X)\n", hMod, LibNameBigCaseName_SmallFormat, FunName, NewFun, OldFun);
	if (idata = IAT_FindLibrary(hMod, LibNameBigCaseName_SmallFormat)) 
	{
		iRet = IAT_FindFunction(hMod, idata, FunName, &Address);
		if (iRet == 0) return IAT_SetHook(Address, NewFun, OldFun);
	}
	TRACE("Hooking failed (%d)\n", iRet);
	return iRet;
}


BOOL Hook_IAT_x64(LPBYTE hMod, char *LibDelayImpName, char *FunName, LPVOID NewFun) 
{
	PIMAGE_DOS_HEADER DosHeader = (PIMAGE_DOS_HEADER)hMod;
	PIMAGE_NT_HEADERS NtHeaders = (PIMAGE_NT_HEADERS)(hMod + DosHeader->e_lfanew);
	DWORD delayImportStartRVA, delayImportSize, nDelaySizeLeft;
	PCImgDelayDescr pDelayDesc;

	TRACE("Hook_IAT_x64(%08X, %s, %s, %08X)\n", hMod, LibDelayImpName, FunName, NewFun);
	// Look up where the delay imports section is (normally in the .didat
	/// section) but not necessarily so.
	delayImportStartRVA = GetImgDirEntryRVA(NtHeaders, IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT);
	delayImportSize = GetImgDirEntrySize(NtHeaders, IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT);
	if (!delayImportStartRVA || !delayImportSize)
	{
		//OutputDebugStringA("!delayImportStartRVA || !delayImportSize");
		return FALSE;
	}

	pDelayDesc = (PCImgDelayDescr)GetPtrFromRVA(delayImportStartRVA, NtHeaders, hMod);
	if (!pDelayDesc)
	{
		//OutputDebugStringA("GetPtrFromRVA fail");
		return FALSE;
	}

	nDelaySizeLeft = (int)delayImportSize;
	while (nDelaySizeLeft > 0 && pDelayDesc->rvaDLLName)
	{
		// from more recent DELAYIMP.H:
		// enum DLAttr {                   // Delay Load Attributes
		//    dlattrRva = 0x1,                // RVAs are used instead of pointers
		//    };
		BOOL bUsingRVA = pDelayDesc->grAttrs & dlattrRva;
		BOOL bInvalid = FALSE;

		DWORD dllNameRVA = (DWORD)pDelayDesc->rvaDLLName;
		PVOID dllNameVA = (PBYTE)0 + (DWORD)pDelayDesc->rvaDLLName;

		PSTR pszDLLNameRVA = (PSTR)GetPtrFromRVA(dllNameRVA, NtHeaders, hMod);

		if (!_strcmp(pszDLLNameRVA, LibDelayImpName))
		{
			PVOID thunkVA = (PBYTE)0 + (DWORD)pDelayDesc->rvaINT;

			PVOID pvThunkRVA = GetPtrFromRVA((DWORD)pDelayDesc->rvaINT, NtHeaders, hMod);
			//PVOID pvThunkVA = GetPtrFromVA(thunkVA, NtHeaders, hMod);
			PIMAGE_THUNK_DATA thunk = (PIMAGE_THUNK_DATA)pvThunkRVA; // bUsingRVA ? (PIMAGE_THUNK_DATA)pvThunkRVA : (PIMAGE_THUNK_DATA)pvThunkVA;
			DWORD i;

			//DumpDelayedImportsImportNames(pDll, hMod, NtHeaders, (PIMAGE_THUNK_DATA)pvThunk, bUsingRVA, szDllName);
			for (i = 0; thunk->u1.AddressOfData; i++)
			{
				if (!(thunk->u1.Ordinal & IMAGE_ORDINAL_FLAG))
				{
					PIMAGE_IMPORT_BY_NAME pOrdinalName = (PIMAGE_IMPORT_BY_NAME)((PBYTE)0 + thunk->u1.AddressOfData);

					pOrdinalName = bUsingRVA
						? (PIMAGE_IMPORT_BY_NAME)GetPtrFromRVA((DWORD)thunk->u1.AddressOfData, NtHeaders, hMod)
						: (PIMAGE_IMPORT_BY_NAME)GetPtrFromVA((PVOID)pOrdinalName, NtHeaders, hMod);

					if (!_strcmp(pOrdinalName->Name, FunName))
					{
						PIMAGE_THUNK_DATA thunkIAT = GetPtrFromRVA((DWORD)pDelayDesc->rvaIAT, NtHeaders, hMod);
						return IAT_SetHook(&thunkIAT[i].u1.Function, NewFun, NULL) == 0;
					}
				}
				thunk++;            // Advance to next thunk
			}
		}
		pDelayDesc++;	// Pointer math.  Advance to next delay import desc.
		nDelaySizeLeft -= sizeof(ImgDelayDescr);
	}

	TRACE("Hook_IAT_x64 failed\n");
	return FALSE;
}
