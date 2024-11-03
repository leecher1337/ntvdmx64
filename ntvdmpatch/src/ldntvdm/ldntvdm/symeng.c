/* Project: ldntvdm
 * Module : symeng
 * Author : leecher@dose.0wnz.at
 * Descr. : Provides routines for DLL-internal symbol Resolution using 
 *          Microsoft Symbol Server
 * Changes: 01.04.2016  - Created
 */

#define _CRT_SECURE_NO_WARNINGS
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

typedef struct {
	DWORD sizeofstruct;
	char file[MAX_PATH + 1];
	BOOL  stripped;
	DWORD timestamp;
	DWORD size;
	char dbgfile[MAX_PATH + 1];
	char pdbfile[MAX_PATH + 1];
	GUID  guid;
	DWORD sig;
	DWORD age;
} SYMSRV_INDEX_INFO, *PSYMSRV_INDEX_INFO;

typedef BOOL(WINAPI *fpSymSrvGetFileIndexInfo)(
	__in PCSTR File,
	__out PSYMSRV_INDEX_INFO Info,
	__in DWORD Flags
	);

typedef BOOL
(CALLBACK *PFINDFILEINPATHCALLBACK)(
	__in PCSTR filename,
	__in PVOID context
	);

#define SSRVOPT_GUIDPTR             0x00000008

typedef BOOL(WINAPI *fpSymFindFileInPath)(
	__in           HANDLE                  hprocess,
	__in_opt       PCSTR                   SearchPathA,
	__in           PCSTR                   FileName,
	__in_opt       PVOID                   id,
	__in           DWORD                   two,
	__in           DWORD                   three,
	__in           DWORD                   flags,
	__out          PSTR                    FoundFile,
	__in_opt       PFINDFILEINPATHCALLBACK callback,
	__in_opt       PVOID                   context
	);


fpSymInitialize SymInitialize;
fpSymSetOptions SymSetOptions;
fpSymGetOptions SymGetOptions;
fpSymSetSearchPath SymSetSearchPath;
fpSymLoadModule64 SymLoadModule64;
fpSymFromName SymFromName;
fpSymUnloadModule64 SymUnloadModule64;
fpSymSrvGetFileIndexInfo SymSrvGetFileIndexInfo;
fpSymFindFileInPath SymFindFileInPath;

static int InitSymEng(void)
{
	HANDLE hProcess = GetCurrentProcess();
	char szPath[(MAX_PATH + 128)*2];
	HMODULE hDbgHelp;

	GetWindowsDirectoryA(szPath, sizeof(szPath));
	strcat(szPath, "\\symbols\\dbghelp\\"
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
		(SymFindFileInPath = (fpSymFindFileInPath)GetProcAddress(hDbgHelp, "SymFindFileInPath")) &&
		(SymSrvGetFileIndexInfo = (fpSymSrvGetFileIndexInfo)GetProcAddress(hDbgHelp, "SymSrvGetFileIndexInfo")) &&
		(SymUnloadModule64 = (fpSymUnloadModule64)GetProcAddress(hDbgHelp, "SymUnloadModule64"))
		)
	{
#ifdef TARGET_WINXP
		// msdl Server doesn't support plain HTTP anymore, however Windows XP/2003 doesn't support
		// newer encryption standard, therefore we use a symbol proxy with HTTP-support
		const char *pszSymSrv = "http://msdl.0wnz.at/download/symbols";
#else
		const char *pszSymSrv = "http://msdl.microsoft.com/download/symbols";
#endif
		char *p = szPath, *p2;

		// Prefer global Windows Symbols path over local temp path
		strcpy(szPath, "SRV*"); p += 4;
		p2 = p;
		p += GetWindowsDirectoryA(p, MAX_PATH);
		strcpy(p, "\\Symbols"); p += 8;
		CreateDirectoryA(p2, NULL);
		*p++ = '*';
		p += GetTempPathA(MAX_PATH, p);
		strcpy(p, "SymbolCache*"); p += 12;
		strcpy(p, pszSymSrv);
		if (!SymInitialize(hProcess, 0, FALSE))
		{
			TRACE("SymInitialize failed: %08X\n", GetLastError());
			return -2;
		}

		TRACE("Symsrv options: %08X\n", SymGetOptions());

		SymSetOptions(SymGetOptions() | SYMOPT_ALLOW_ABSOLUTE_SYMBOLS | SYMOPT_DEBUG);
		SymSetOptions(SymGetOptions() & (~SYMOPT_DEFERRED_LOADS));
		SymSetSearchPath(hProcess, szPath);

		return 0;
	}
	TRACE("InitSymEng failed.\n");
	return -1;
}

BOOL SymEng_GetPDBFile(char *pszSymPath, char *pszFile, char *pszPathPDB)
{
	BOOL fRet;
	SYMSRV_INDEX_INFO Info;

	Info.sizeofstruct = sizeof(Info);
	if (!SymSrvGetFileIndexInfo(pszFile, &Info, 0))
	{
		TRACE("SymSrvGetFileIndexInfo(%s) failed: %d", pszFile, GetLastError());
		return FALSE;
	}
	fRet = SymFindFileInPath(GetCurrentProcess(), pszSymPath, Info.pdbfile, &Info.guid, Info.age, Info.sig, SSRVOPT_GUIDPTR, pszPathPDB, NULL, NULL);
	if (!fRet)
	{
		TRACE("SymFindFileInPath(%s, %d, %d) failed: %d", pszFile, Info.age, Info.sig, GetLastError());
	}
	return fRet;
}

void SymEng_Delete0ByteSyms(char *pszFile)
{
	char szPDB[MAX_PATH];

	// Check, if the PDB is 0 bytes (common problem with symbol server, it seems, then SymSrv is stuck)
	if (SymEng_GetPDBFile(NULL, pszFile, szPDB))
	{
		WIN32_FILE_ATTRIBUTE_DATA fad;
		if (GetFileAttributesExA(szPDB, GetFileExInfoStandard, &fad) && fad.nFileSizeLow == 0)
		{
			TRACE("Detected 0 byte symbol file: %s, removing\n", szPDB);
			DeleteFileA(szPDB);
			/* Now it has beed removed from main Symbol cache.
			* However, the damaged file can also sit in the temporary cache,
			* so we must locate and delete it there, too
			*/
			char szSymPath[MAX_PATH], *p = szSymPath;

			p += GetTempPathA(MAX_PATH, p);
			strcpy(p, "SymbolCache");
			if (SymEng_GetPDBFile(szSymPath, pszFile, szPDB) &&
				GetFileAttributesExA(szPDB, GetFileExInfoStandard, &fad) &&
				fad.nFileSizeLow == 0)
			{
				TRACE("Detected 0 byte symbol file: %s, removing\n", szPDB);
				DeleteFileA(szPDB);
			}
		}
	}
}

int SymEng_LoadModule(char *pszFile, DWORD64 *pdwBase)
{
	static int iStatus = -1;

	if (iStatus < 0)
	{
		iStatus = InitSymEng();
		if (iStatus < 0) return iStatus;
	}

	if (!pszFile) return -1;

	SymEng_Delete0ByteSyms(pszFile);

	if (!(*pdwBase = SymLoadModule64(GetCurrentProcess(), 0, pszFile, 0, 0, 0)))
	{
		TRACE("SymLoadModule64 %s failed: %08X\n", pszFile, GetLastError());
		return -3;
	}

	return 0;
}


ULONG64 SymEng_GetAddr(DWORD64 dwBase, char *pszSymbol)
{
	SYMBOL_INFO symInfo;

	RtlZeroMemory(&symInfo, sizeof(symInfo));
	TRACE("SymEng_GetAddr %s\n", pszSymbol);
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