/* Project: ldntvdm
 * Module : matcheng
 * Author : leecher@dose.0wnz.at
 * Descr. : Provides routines to search for a FLIRT-Signature in a file
 * Changes: 29.06.2022  - Created
 */

#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include "ldntvdm.h"
#include "matcheng.h"

#ifdef NEED_APPINFO

DWORD MatchEng_LoadModule(char *pszFile, MATCHENG_INST *pInst)
{
	DWORD dwRet;

	pInst->hFile = CreateFileA(pszFile, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	
	if(pInst->hFile != INVALID_HANDLE_VALUE)
	{
		pInst->dwSize = GetFileSize(pInst->hFile, NULL);
		if (pInst->hMap = CreateFileMappingA (pInst->hFile, NULL, PAGE_READONLY, 0, 0, NULL))
		{
			if (pInst->lpMem = MapViewOfFile (pInst->hMap, FILE_MAP_READ, 0, 0, 0))
			{
				return 0;
			}
			else
			{
				TRACE("Cannot map view of file %s: %d", pszFile, GetLastError());
				dwRet = -3;
			}
			CloseHandle (pInst->hMap);
		}
		else
		{
			TRACE("Cannot map view of file %s: %d", pszFile, GetLastError());
			dwRet = -2;
		}
		CloseHandle(pInst->hFile);
	}
	else
	{
		TRACE("Cannot open file %s: %d", pszFile, GetLastError());
		dwRet = -1;
	}

	return dwRet;
}


ULONG64 MatchEng_FindSig(MATCHENG_INST *pInst, BYTE *lpSig, DWORD cbSig, DWORD step)
{
	BYTE *lpOffset, *lpEnd;
	DWORD i;

	for (lpOffset=pInst->lpMem, lpEnd=pInst->lpMem+pInst->dwSize-cbSig; lpOffset<lpEnd; lpOffset+=step)
	{
		for (i=0; i<cbSig; i++)
			if (lpSig[i]!=0x99 && lpOffset[i]!=lpSig[i]) break;
		if (i == cbSig) 
		{
			TRACE("MatchEng Pattern found at %X\n", lpOffset - pInst->lpMem);
			return lpOffset - pInst->lpMem;
		}
	}
	return 0;
}


void MatchEng_UnloadModule(MATCHENG_INST *pInst)
{
	UnmapViewOfFile (pInst->lpMem);
	CloseHandle (pInst->hMap);
	CloseHandle(pInst->hFile);
}

#endif
