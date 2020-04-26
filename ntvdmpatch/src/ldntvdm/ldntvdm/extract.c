/****************************** Module Header ******************************\
*
* Module Name: extract.c
*
* Copyright (c) 1985-1996, Microsoft Corporation
*
* Icon Extraction Routines
*
* History:
\***************************************************************************/
#include "extract.h"
#include "newexe.h"
#ifdef EXTRACTICON_HOOK


#define MZMAGIC         ((WORD)'M'+((WORD)'Z'<<8))
#define PEMAGIC         ((WORD)'P'+((WORD)'E'<<8))

typedef struct new_exe          NEWEXE, *LPNEWEXE;
typedef struct exe_hdr          EXEHDR, *LPEXEHDR;
typedef struct rsrc_nameinfo    RESNAMEINFO, *LPRESNAMEINFO;
typedef struct rsrc_typeinfo    RESTYPEINFO, *LPRESTYPEINFO;
typedef struct rsrc_typeinfo    UNALIGNED    *ULPRESTYPEINFO;
typedef struct new_rsrc         RESTABLE, *LPRESTABLE;

#define RESOURCE_VA(x)        ((x)->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress)
#define RESOURCE_SIZE(x)      ((x)->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].Size)
#define NUMBER_OF_SECTIONS(x) ((x)->FileHeader.NumberOfSections)


#define WIN32VER30  0x00030000  // for CreateIconFromResource()

#define GET_COUNT   424242

/***************************************************************************\
* RVAtoP
*
*
\***************************************************************************/

LPVOID RVAtoP(
	LPVOID pBase,
	DWORD  rva)
{
	LPEXEHDR             pmz;
	IMAGE_NT_HEADERS     *ppe;
	IMAGE_SECTION_HEADER *pSection; // section table
	int                  i;
	DWORD                size;

	pmz = (LPEXEHDR)pBase;
	ppe = (IMAGE_NT_HEADERS*)((BYTE*)pBase + pmz->e_lfanew);

	/*
	* Scan the section table looking for the RVA
	*/
	pSection = IMAGE_FIRST_SECTION(ppe);

	for (i = 0; i < NUMBER_OF_SECTIONS(ppe); i++) {

		size = pSection[i].Misc.VirtualSize ?
			pSection[i].Misc.VirtualSize : pSection[i].SizeOfRawData;

		if (rva >= pSection[i].VirtualAddress &&
			rva <  pSection[i].VirtualAddress + size) {

			return (LPBYTE)pBase + pSection[i].PointerToRawData + (rva - pSection[i].VirtualAddress);
		}
	}

	return NULL;
}

/***************************************************************************\
* GetResourceTablePE
*
*
\***************************************************************************/

LPVOID GetResourceTablePE(
	LPVOID pBase)
{
	LPEXEHDR         pmz;
	IMAGE_NT_HEADERS *ppe;

	pmz = (LPEXEHDR)pBase;
	ppe = (IMAGE_NT_HEADERS*)((BYTE*)pBase + pmz->e_lfanew);

	if (pmz->e_magic != MZMAGIC)
		return 0;

	if (ppe->Signature != IMAGE_NT_SIGNATURE)
		return 0;

	if (ppe->FileHeader.SizeOfOptionalHeader < IMAGE_SIZEOF_NT_OPTIONAL_HEADER)
		return 0;

	return RVAtoP(pBase, RESOURCE_VA(ppe));
}

/****************************************************************************
* FindResourcePE
*
*   given a PE resource directory will find a resource in it.
*
*   if iResIndex < 0 we will search for the specific index
*   if iResIndex >= 0 we will return the Nth index
*   if iResIndex == GET_COUNT the count of resources will be returned
*
\*****************************************************************************/

LPVOID FindResourcePE(
	LPVOID pBase,
	LPVOID prt,
	int    iResIndex,
	int    ResType,
	DWORD  *pcb)
{
	int                            i;
	int                            cnt;
	IMAGE_RESOURCE_DIRECTORY       *pdir;
	IMAGE_RESOURCE_DIRECTORY_ENTRY *pres;
	IMAGE_RESOURCE_DATA_ENTRY      *pent;

	pdir = (IMAGE_RESOURCE_DIRECTORY *)prt;

	/*
	* First find the type always a ID so ignore strings totaly
	*/
	cnt = pdir->NumberOfIdEntries + pdir->NumberOfNamedEntries;
	pres = (IMAGE_RESOURCE_DIRECTORY_ENTRY*)(pdir + 1);

	for (i = 0; i < cnt; i++) {

		if (pres[i].Name == (DWORD)ResType)
			break;
	}

	if (i == cnt)             // did not find the type
		return 0;

	/*
	* Now go find the actual resource  either by id (iResIndex < 0) or
	* by ordinal (iResIndex >= 0)
	*/
	pdir = (IMAGE_RESOURCE_DIRECTORY*)((LPBYTE)prt +
		(pres[i].OffsetToData & ~IMAGE_RESOURCE_DATA_IS_DIRECTORY));

	cnt = pdir->NumberOfIdEntries + pdir->NumberOfNamedEntries;
	pres = (IMAGE_RESOURCE_DIRECTORY_ENTRY*)(pdir + 1);

	/*
	* If we just want size, do it.
	*/
	if (iResIndex == GET_COUNT)
		return (LPVOID)cnt;

	/*
	* if we are to search for a specific id do it.
	*/
	if (iResIndex < 0) {

		for (i = 0; i < cnt; i++)
			if (pres[i].Name == (DWORD)(-iResIndex))
				break;
	}
	else {
		i = iResIndex;
	}

	/*
	* is the index in range?
	*/
	if (i >= cnt)
		return 0;

	/*
	* if we get this far the resource has a language part, ick!
	* !!!for now just punt and return the first one.
	* !!!BUGBUG we dont handle multi-language icons
	*/
	if (pres[i].OffsetToData & IMAGE_RESOURCE_DATA_IS_DIRECTORY) {

		pdir = (IMAGE_RESOURCE_DIRECTORY*)((LPBYTE)prt +
			(pres[i].OffsetToData & ~IMAGE_RESOURCE_DATA_IS_DIRECTORY));
		pres = (IMAGE_RESOURCE_DIRECTORY_ENTRY*)(pdir + 1);
		i = 0;  // choose first one
	}

	/*
	* Nested way to deep for me!
	*/
	if (pres[i].OffsetToData & IMAGE_RESOURCE_DATA_IS_DIRECTORY)
		return 0;

	pent = (IMAGE_RESOURCE_DATA_ENTRY*)((LPBYTE)prt + pres[i].OffsetToData);

	/*
	* all OffsetToData fields except the final one are relative to
	* the start of the section.  the final one is a virtual address
	* we need to go back to the header and get the virtual address
	* of the resource section to do this right.
	*/
	*pcb = pent->Size;
	return RVAtoP(pBase, pent->OffsetToData);
}


/***************************************************************************\
* GetResourceTableNE
*
*
\***************************************************************************/

LPVOID GetResourceTableNE(
	LPVOID pBase)
{
	LPNEWEXE pne;
	LPEXEHDR pmz;

	pmz = (LPEXEHDR)pBase;
	pne = (LPNEWEXE)((LPBYTE)pBase + pmz->e_lfanew);

	if (pmz->e_magic != MZMAGIC)
		return 0;

	if (pne->ne_magic != NEMAGIC)           // must be a NEWEXE
		return 0;

	if (pne->ne_exetyp != NE_WINDOWS &&     // must be a Win DLL/EXE/386
		pne->ne_exetyp != NE_DEV386)
		return 0;

	if (pne->ne_expver < 0x0300)            // must be 3.0 or greater
		return 0;

	if (pne->ne_rsrctab == pne->ne_restab)  // no resources
		return 0;

	return (LPBYTE)pne + pne->ne_rsrctab;   // return resource table pointer
}

/***************************************************************************\
* FindResourceNE
*
* This returns a pointer to the rsrc_nameinfo of the resource with the
* given index and type, if it is found, otherwise it returns NULL.
*
* if iResIndex is < 0, then it is assumed to be a ID and the res table
* will be searched for a matching id.
*
* if iResIndex is >= 0, then it is assumed to be a index and the Nth
* resorce of the specifed type will be returned.
*
* if iResIndex == GET_COUNT the count of resources will be returned
*
\***************************************************************************/

LPVOID FindResourceNE(
	LPVOID lpBase,
	LPVOID prt,
	int    iResIndex,
	int    iResType,
	DWORD  *pcb)
{
	LPRESTABLE     lpResTable;
	ULPRESTYPEINFO ulpResTypeInfo;
	LPRESNAMEINFO  lpResNameInfo;  // 16 bit alignment ok - had ushorts only
	int            i;

	lpResTable = (LPRESTABLE)prt;
	//ulpResTypeInfo = (ULPRESTYPEINFO)(LPWBYTE)&lpResTable->rs_typeinfo;
	ulpResTypeInfo = (ULPRESTYPEINFO)((LPBYTE)lpResTable + 2);

	while (ulpResTypeInfo->rt_id) {

		if (ulpResTypeInfo->rt_id == (iResType | RSORDID)) {

			lpResNameInfo = (LPRESNAMEINFO)(ulpResTypeInfo + 1);

			if (iResIndex == GET_COUNT)
				return (LPVOID)ulpResTypeInfo->rt_nres;

			if (iResIndex < 0) {

				for (i = 0; i < (int)ulpResTypeInfo->rt_nres; i++) {

					if (lpResNameInfo[i].rn_id == ((-iResIndex) | RSORDID))
						break;
				}

				iResIndex = i;
			}

			if (iResIndex >= (int)ulpResTypeInfo->rt_nres)
				return NULL;

			*pcb = ((DWORD)lpResNameInfo[iResIndex].rn_length) << lpResTable->rs_align;
			return (LPBYTE)lpBase + ((long)lpResNameInfo[iResIndex].rn_offset << lpResTable->rs_align);
		}

		ulpResTypeInfo =
			(ULPRESTYPEINFO)((LPRESNAMEINFO)(ulpResTypeInfo + 1) +
				ulpResTypeInfo->rt_nres);
	}

	*pcb = 0;
	return NULL;
}

/***************************************************************************\
* ExtractIconFromEXE
*
*
\***************************************************************************/

UINT ExtractIconFromEXE(
	HANDLE hFile,
	int    nIconIndex,
	int    cxIconSize,
	int    cyIconSize,
	HICON  *phicon,
	UINT   *piconid,
	UINT   nIcons,
	UINT   flags)
{
	HANDLE           hFileMap = INVALID_HANDLE_VALUE;
	LPVOID           lpFile = NULL;
	EXEHDR           *pmz;
	NEWEXE UNALIGNED *pne;
	LPVOID           pBase;
	LPVOID           pres = NULL;
	UINT             result = 0;
	LONG             FileLength;
	DWORD            cbSize;
	int              cxIcon;
	int              cyIcon;

	LPVOID(*FindResourceX)(LPVOID pBase,
		LPVOID prt,
		int    iResIndex,
		int    iResType,
		DWORD  *pcb);

	FileLength = (LONG)GetFileSize(hFile, NULL);

	hFileMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (hFileMap == NULL)
		goto exit;

	lpFile = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 0);
	if (lpFile == NULL)
		goto exit;

	pBase = (LPVOID)lpFile;
	pmz = (struct exe_hdr *)pBase;

	//_try{

		if (pmz->e_magic != MZMAGIC)
		goto exit;

	if (pmz->e_lfanew <= 0)             // not a new exe
		goto exit;

	if (pmz->e_lfanew >= FileLength)    // not a new exe
		goto exit;

	pne = (NEWEXE UNALIGNED *)((BYTE*)pmz + pmz->e_lfanew);

	switch (pne->ne_magic) {
	case NEMAGIC:
		pres = GetResourceTableNE(pBase);
		FindResourceX = FindResourceNE;
		break;

	case PEMAGIC:
		pres = GetResourceTablePE(pBase);
		FindResourceX = FindResourcePE;
		break;
	}

	/*
	* cant find the resource table, fail
	*/
	if (pres == NULL)
		goto exit;

	/*
	* do we just want a count?
	*/
	if (phicon == NULL) {
		result = (UINT)FindResourceX(pBase,
			pres,
			GET_COUNT,
			(int)RT_GROUP_ICON,
			&cbSize);
		goto exit;
	}

	while (result < nIcons) {

		LPVOID lpIconDir;
		LPVOID lpIcon;
		int    idIcon;

		cxIcon = cxIconSize;
		cyIcon = cyIconSize;

		/*
		*  find the icon dir for this icon.
		*/
		lpIconDir = FindResourceX(pBase,
			pres,
			nIconIndex,
			(int)RT_GROUP_ICON,
			&cbSize);

		if (lpIconDir == NULL)
			goto exit;

		if ((((LPNEWHEADER)lpIconDir)->Reserved != 0) ||
			(((LPNEWHEADER)lpIconDir)->ResType != FT_ICON)) {

			goto exit;
		}
	again:
		idIcon = LookupIconIdFromDirectoryEx((LPBYTE)lpIconDir,
			TRUE,
			LOWORD(cxIcon),
			LOWORD(cyIcon),
			flags);
		lpIcon = FindResourceX(pBase,
			pres,
			-idIcon,
			(int)RT_ICON,
			&cbSize);

		if (lpIcon == NULL)
			goto exit;

		if ((((UPBITMAPINFOHEADER)lpIcon)->biSize != sizeof(BITMAPINFOHEADER)) &&
			(((UPBITMAPINFOHEADER)lpIcon)->biSize != sizeof(BITMAPCOREHEADER))) {

			goto exit;
		}

#ifndef WINNT
		/* touch this memory before calling USER
		* so if we page fault we will do it outside of the Win16Lock
		* most icons aren't more than 2 pages
		*/
		ReadAByte(((BYTE *)lpIcon) + cbSize - 1);
#endif

		if (piconid)
			piconid[result] = idIcon;

		phicon[result++] = CreateIconFromResourceEx((LPBYTE)lpIcon,
			cbSize,
			TRUE,
			WIN32VER30,
			LOWORD(cxIcon),
			LOWORD(cyIcon),
			flags);

		/*
		* check for large/small icon extract
		*/
		if (HIWORD(cxIcon)) {

			cxIcon = HIWORD(cxIcon);
			cyIcon = HIWORD(cyIcon);

			goto again;
		}

		nIconIndex++;       // next icon index
	}

	/*} _except(EXCEPTION_EXECUTE_HANDLER) {
		result = 0;
	}*/

exit:

	if (lpFile)
		UnmapViewOfFile(lpFile);

	if (hFileMap != INVALID_HANDLE_VALUE)
		CloseHandle(hFileMap);

	return result;
}

#endif
