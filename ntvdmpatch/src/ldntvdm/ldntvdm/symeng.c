/* Project: ldntvdm
 * Module : symeng
 * Author : leecher@dose.0wnz.at
 * Descr. : Provides routines for DLL-internal symbol Resolution using 
 *          Microsoft Symbol Server
 * Changes: 01.04.2016  - Created
 */

#include <stdio.h>
#include <windows.h>
#include "ldntvdm.h"

#ifndef WOW_HACK

typedef struct _SYMBOL_INFO {
	ULONG       SizeOfStruct;
	ULONG       TypeIndex;        // Type Index of symbol
	ULONG64     Reserved[2];
	ULONG       Index;
	ULONG       Size;
	ULONG64     ModBase;          // Base Address of module comtaining this symbol
	ULONG       Flags;
	ULONG64     Value;            // Value of symbol, ValuePresent should be 1
	ULONG64     Address;          // Address of symbol including base address of module
	ULONG       Register;         // register holding value or pointer to value
	ULONG       Scope;            // scope of the symbol
	ULONG       Tag;              // pdb classification
	ULONG       NameLen;          // Actual length of name
	ULONG       MaxNameLen;
	CHAR        Name[1];          // Name of symbol
} SYMBOL_INFO, *PSYMBOL_INFO;

#define SYMOPT_ALLOW_ABSOLUTE_SYMBOLS    0x00000800
#define SYMOPT_DEFERRED_LOADS            0x00000004
#define SYMOPT_DEBUG					 0x80000000


typedef BOOL (WINAPI *fpSymInitialize)(
	_In_ HANDLE hProcess,
	_In_opt_ PCSTR UserSearchPath,
	_In_ BOOL fInvadeProcess
	);

typedef DWORD (WINAPI *fpSymSetOptions)(
	_In_ DWORD   SymOptions
	);

typedef DWORD (WINAPI *fpSymGetOptions)(
	VOID
	);

typedef BOOL (WINAPI *fpSymSetSearchPath)(
	_In_ HANDLE hProcess,
	_In_opt_ PCSTR SearchPath
	);

typedef DWORD64 (WINAPI *fpSymLoadModule64)(
	_In_ HANDLE hProcess,
	_In_opt_ HANDLE hFile,
	_In_opt_ PCSTR ImageName,
	_In_opt_ PCSTR ModuleName,
	_In_ DWORD64 BaseOfDll,
	_In_ DWORD SizeOfDll
	);

typedef BOOL (WINAPI *fpSymFromName)(
	_In_ HANDLE hProcess,
	_In_ PCSTR Name,
	_Inout_ PSYMBOL_INFO Symbol
	);

typedef BOOL (WINAPI *fpSymUnloadModule64)(
	_In_ HANDLE hProcess,
	_In_ DWORD64 BaseOfDll
	);

fpSymInitialize SymInitialize;
fpSymSetOptions SymSetOptions;
fpSymGetOptions SymGetOptions;
fpSymSetSearchPath SymSetSearchPath;
fpSymLoadModule64 SymLoadModule64;
fpSymFromName SymFromName;
fpSymUnloadModule64 SymUnloadModule64;

static int InitSymEng(void)
{
	HANDLE hProcess = GetCurrentProcess();
	char szPath[MAX_PATH + 128];
	HMODULE hDbgHelp;

	GetWindowsDirectoryA(szPath, sizeof(szPath));
	lstrcatA(szPath, "\\symbols\\dbghelp\\"
#ifdef _WIN64
		"x64"
#else
		"x86"
#endif
		"\\dbghelp.dll"
		);

	if ((hDbgHelp = LoadLibraryA(szPath)) &&
		(SymInitialize = (fpSymInitialize)GetProcAddress(hDbgHelp, "SymInitialize")) &&
		(SymSetOptions = (fpSymSetOptions)GetProcAddress(hDbgHelp, "SymSetOptions")) &&
		(SymGetOptions = (fpSymGetOptions)GetProcAddress(hDbgHelp, "SymGetOptions")) &&
		(SymSetSearchPath = (fpSymSetSearchPath)GetProcAddress(hDbgHelp, "SymSetSearchPath")) &&
		(SymLoadModule64 = (fpSymLoadModule64)GetProcAddress(hDbgHelp, "SymLoadModule64")) &&
		(SymFromName = (fpSymFromName)GetProcAddress(hDbgHelp, "SymFromName")) &&
		(SymUnloadModule64 = (fpSymUnloadModule64)GetProcAddress(hDbgHelp, "SymUnloadModule64"))
		)
	{
		lstrcpyA(szPath, "SRV*");
		GetTempPathA(sizeof(szPath) - 4, szPath + 4);
		lstrcatA(szPath, "SymbolCache");
		CreateDirectoryA(szPath + 4, NULL);

		lstrcatA(szPath, "*http://msdl.microsoft.com/download/symbols");
		if (!SymInitialize(hProcess, 0, FALSE))
		{
			TRACE("SymInitialize failed: %08X", GetLastError());
			return -2;
		}

		TRACE("Symsrv options: %08X", SymGetOptions());

		SymSetOptions(SymGetOptions() | SYMOPT_ALLOW_ABSOLUTE_SYMBOLS | SYMOPT_DEBUG);
		SymSetOptions(SymGetOptions() & (~SYMOPT_DEFERRED_LOADS));
		SymSetSearchPath(hProcess, szPath);

		return 0;
	}
	TRACE("InitSymEng failed.");
	return -1;
}

int SymEng_LoadModule(char *pszFile, DWORD64 *pdwBase)
{
	static int iStatus = -1;

	if (iStatus < 0)
	{
		iStatus = InitSymEng();
		if (iStatus < 0) return iStatus;
	}

	if (!(*pdwBase = SymLoadModule64(GetCurrentProcess(), 0, pszFile, 0, 0, 0)))
	{
		TRACE("SymLoadModule64 %s failed: %08X", pszFile, GetLastError());
		return -3;
	}

	return 0;
}


ULONG64 SymEng_GetAddr(DWORD64 dwBase, char *pszSymbol)
{
	SYMBOL_INFO symInfo;

	RtlZeroMemory(&symInfo, sizeof(symInfo));
	TRACE("SymEng_GetAddr %s", pszSymbol);
	symInfo.SizeOfStruct = sizeof(SYMBOL_INFO);
	if (!SymFromName(GetCurrentProcess(), pszSymbol, &symInfo))
	{
		TRACE("SymFromName failed: %08X\n", GetLastError());
		return 0;
	}

	return (symInfo.Address - symInfo.ModBase);
}


void SymEng_UnloadModule(DWORD64 dwBase)
{
	SymUnloadModule64(GetCurrentProcess(), dwBase);
}

#endif