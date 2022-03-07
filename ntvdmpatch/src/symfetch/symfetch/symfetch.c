#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>


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


typedef BOOL(WINAPI *fpSymInitialize)(
	_In_ HANDLE hProcess,
	_In_opt_ PCSTR UserSearchPath,
	_In_ BOOL fInvadeProcess
	);

typedef DWORD(WINAPI *fpSymSetOptions)(
	_In_ DWORD   SymOptions
	);

typedef DWORD(WINAPI *fpSymGetOptions)(
	VOID
	);

typedef BOOL(WINAPI *fpSymSetSearchPath)(
	_In_ HANDLE hProcess,
	_In_opt_ PCSTR SearchPath
	);

typedef DWORD64(WINAPI *fpSymLoadModule64)(
	_In_ HANDLE hProcess,
	_In_opt_ HANDLE hFile,
	_In_opt_ PCSTR ImageName,
	_In_opt_ PCSTR ModuleName,
	_In_ DWORD64 BaseOfDll,
	_In_ DWORD SizeOfDll
	);

typedef BOOL(WINAPI *fpSymFromName)(
	_In_ HANDLE hProcess,
	_In_ PCSTR Name,
	_Inout_ PSYMBOL_INFO Symbol
	);

typedef BOOL(WINAPI *fpSymUnloadModule64)(
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

static void fprintf(DWORD dwConsole, const char  *pszFormat, ...)
{
	DWORD dwLen;
	static char szBuf[4096];
	va_list ap;
	HANDLE hConsole = GetStdHandle(dwConsole);

	va_start(ap, pszFormat);
	dwLen = wvsprintfA(szBuf, pszFormat, ap);
	WriteConsoleA(hConsole, szBuf, dwLen, &dwLen, NULL);
	va_end(ap);
}

static int InitSymEng(void)
{
	HANDLE hProcess = GetCurrentProcess();
	char szPath[MAX_PATH + 128];
	HMODULE hDbgHelp;
	OSVERSIONINFO ov;
	DWORD len;

	ov.dwOSVersionInfoSize = sizeof(ov);
	GetVersionEx(&ov);

	len = GetWindowsDirectoryA(szPath, sizeof(szPath));
	strcat(szPath, "\\symbols\\dbghelp\\"
#ifdef _WIN64
		"x64"
#else
		"x86"
#endif
		"\\dbghelp.dll"
		);
	if (!(hDbgHelp = LoadLibraryA(szPath)) &&
		!(hDbgHelp = LoadLibraryA(szPath + len + 1)))
		hDbgHelp = LoadLibraryA("dbghelp.dll");

	if (hDbgHelp &&
		(SymInitialize = (fpSymInitialize)GetProcAddress(hDbgHelp, "SymInitialize")) &&
		(SymSetOptions = (fpSymSetOptions)GetProcAddress(hDbgHelp, "SymSetOptions")) &&
		(SymGetOptions = (fpSymGetOptions)GetProcAddress(hDbgHelp, "SymGetOptions")) &&
		(SymSetSearchPath = (fpSymSetSearchPath)GetProcAddress(hDbgHelp, "SymSetSearchPath")) &&
		(SymLoadModule64 = (fpSymLoadModule64)GetProcAddress(hDbgHelp, "SymLoadModule64")) &&
		(SymFromName = (fpSymFromName)GetProcAddress(hDbgHelp, "SymFromName")) &&
		(SymUnloadModule64 = (fpSymUnloadModule64)GetProcAddress(hDbgHelp, "SymUnloadModule64")) &&
		(SymFindFileInPath = (fpSymFindFileInPath)GetProcAddress(hDbgHelp, "SymFindFileInPath")) &&
		(SymSrvGetFileIndexInfo = (fpSymSrvGetFileIndexInfo)GetProcAddress(hDbgHelp, "SymSrvGetFileIndexInfo"))
		)
	{
		char *p = szPath, *p2, *pszSymSrv;

		if (ov.dwMajorVersion == 5) pszSymSrv =  "http://msdl.0wnz.at/download/symbols";
		else pszSymSrv = "http://msdl.microsoft.com/download/symbols";
		// Prefer global Windows Symbols path over local temp path
		strcpy(szPath, "SRV*"); p += 4;
		p += GetWindowsDirectoryA(p, MAX_PATH);
		strcpy(p, "\\Symbols*"); p += 9;
		p2 = p;
		p += GetTempPathA(MAX_PATH, p);
		strcpy(p, "SymbolCache"); p += 11;
		CreateDirectoryA(p2, NULL);
		*p = '*'; p++;
		strcpy(p, pszSymSrv);

		if (!SymInitialize(hProcess, 0, FALSE))
		{
			fprintf(STD_ERROR_HANDLE, "SymInitialize failed: %08X\n", GetLastError());
			return -2;
		}

		//fprintf(STD_ERROR_HANDLE, "Symsrv options: %08X\n", SymGetOptions());

		SymSetOptions(SymGetOptions() | SYMOPT_ALLOW_ABSOLUTE_SYMBOLS | SYMOPT_DEBUG);
		SymSetOptions(SymGetOptions() & (~SYMOPT_DEFERRED_LOADS));
		SymSetSearchPath(hProcess, szPath);

		return 0;
	}
	fprintf(STD_ERROR_HANDLE, "InitSymEng failed.\n");
	return -1;
}

BOOL SymEng_GetPDBFile(char *pszSymPath, char *pszFile, char *pszPathPDB)
{
	BOOL fRet;
	SYMSRV_INDEX_INFO Info;

	Info.sizeofstruct = sizeof(Info);
	if (!SymSrvGetFileIndexInfo(pszFile, &Info, 0))
	{
		fprintf(STD_ERROR_HANDLE, "SymSrvGetFileIndexInfo(%s) failed: %d", pszFile, GetLastError());
		return FALSE;
	}
	fRet = SymFindFileInPath(GetCurrentProcess(), pszSymPath, Info.pdbfile, &Info.guid, Info.age, Info.sig, SSRVOPT_GUIDPTR, pszPathPDB, NULL, NULL);
	if (!fRet)
	{
		fprintf(STD_ERROR_HANDLE, "SymFindFileInPath(%s, %d, %d) failed: %d", pszFile, Info.age, Info.sig, GetLastError());
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
			fprintf(STD_OUTPUT_HANDLE, "Detected 0 byte symbol file: %s, removing\n", szPDB);
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
				fprintf(STD_OUTPUT_HANDLE, "Detected 0 byte symbol file: %s, removing\n", szPDB);
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

	if (!(*pdwBase = SymLoadModule64(GetCurrentProcess(), 0, pszFile, 0, 0, 0)))
	{
		fprintf(STD_ERROR_HANDLE, "SymLoadModule64 %s failed: %08X\n", pszFile, GetLastError());
		return -3;
	}

	SymEng_Delete0ByteSyms(pszFile);
	
	return 0;
}

void SymEng_UnloadModule(DWORD64 dwBase)
{
	SymUnloadModule64(GetCurrentProcess(), dwBase);
}

int main(int argc, char **argv)
{
	int i, iRet = 0;
	DWORD64 dwBase;

	if (argc < 2) 
	{
		fprintf(STD_ERROR_HANDLE, "Usage: %s <File1> [File2...]\n", argv[0]);
		return -1;
	}

	for (i = 1; i < argc; i++)
	{
		if ((iRet = SymEng_LoadModule(argv[i], &dwBase)) < 0)
		{
			switch (iRet)
			{
			case -1: fprintf(STD_ERROR_HANDLE, "Cannot load dbghelp library\n"); break;
			case -2: fprintf(STD_ERROR_HANDLE, "Cannot initialize symbol engine\n"); break;
			case -3: fprintf(STD_ERROR_HANDLE, "Cannot load symbols for module\n"); break;
			}
			return iRet;
		}

		SymEng_UnloadModule(dwBase);
	}
	return iRet;
}

#ifndef DEBUG
// ------------------------------------------------------------------------- //

// Normally implemeted in Shell-API
static LPSTR myPathGetArgs(LPSTR pszPath)
{
	if (*pszPath == '"')
	{
		pszPath++;
		while (*pszPath && (*pszPath != '"')) pszPath++;
		if (*pszPath == '"') pszPath++;
	}
	else
	{
		while (*pszPath> ' ') pszPath++;
	}

	while (*pszPath && (*pszPath <= ' ')) pszPath++;
	return pszPath;
}

void mainCRTStartup(void)
{
	char *argv[2];

	argv[0] = "symfetch";
	argv[1] = myPathGetArgs(GetCommandLineA());
	ExitProcess(main(argv[1] && *argv[1]?2:1, argv));
}

#endif