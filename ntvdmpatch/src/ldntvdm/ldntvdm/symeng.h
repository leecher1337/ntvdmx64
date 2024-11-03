#pragma once
/*	 0	-	Success
	-1	-	Cannot load dbghelp library
	-2	-	Cannot initialize symbol engine
	-3	-	Cannot load symbols for module
*/
#include <tchar.h>

int SymEng_LoadModule(char  *pszFile, DWORD64 *pdwBase);
DWORD SymEng_GetAddr(DWORD64 dwBase, char *pszSymbol);
void SymEng_UnloadModule(DWORD64 dwBase);
BOOL SymEng_GetPDBFile(char *pszSymPath, char *pszFile, char *pszPathPDB);
