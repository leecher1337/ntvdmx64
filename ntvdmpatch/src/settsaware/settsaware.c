#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// ------------------------------------------------------------------------- //
// Helper functions 
// ------------------------------------------------------------------------- //

// My own fast printf implementation
static void print(DWORD dwConsole, WORD wColor, const char  *pszFormat, ...)
{
	DWORD dwLen;
	static char szBuf[4096];
	va_list ap;
	HANDLE hConsole = GetStdHandle(dwConsole);
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	if (wColor)
	{
		GetConsoleScreenBufferInfo (hConsole, &csbi);
		SetConsoleTextAttribute (hConsole, (WORD)(FOREGROUND_INTENSITY | wColor));
	}
	va_start(ap, pszFormat);
	dwLen = wvsprintf(szBuf, pszFormat, ap);
	WriteConsole (hConsole, szBuf, dwLen, &dwLen, NULL);
	va_end (ap);
	if (wColor)
	{
		SetConsoleTextAttribute (hConsole, csbi.wAttributes);
	}
}

// ------------------------------------------------------------------------- //

// Normally implemeted in Shell-API
static LPTSTR PathGetArgs (LPTSTR pszPath)
{
    if( *pszPath== '"' )
    {
		pszPath++;
        while(*pszPath&& (*pszPath!= '"')) pszPath++;
        if(*pszPath== '"') pszPath++;
    }
    else    
    {
        while ( *pszPath> ' ' ) pszPath++;
    }

    while ( *pszPath&& (*pszPath<= ' ') ) pszPath++;
	return pszPath;
}

// ------------------------------------------------------------------------- //

// Show error message from system on console
static void ShowError (void)
{
	char szMsg[1024];

	FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
		MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)szMsg, sizeof(szMsg), NULL);
	print (STD_ERROR_HANDLE, 0, szMsg);
}

// ------------------------------------------------------------------------- //
// Patch functions
// ------------------------------------------------------------------------- //

static BOOL PatchFile (char *pszFile)
{
	HANDLE hFile, hMap;
	BYTE *lpMem;
	BOOL bRet = FALSE;
	DWORD dwSize;

	hFile = CreateFile(pszFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	
	if(hFile != INVALID_HANDLE_VALUE)
	{
		dwSize = GetFileSize(hFile, NULL);
		if (hMap = CreateFileMapping (hFile, NULL, PAGE_READWRITE | SEC_COMMIT, 0, 0, NULL))
		{
			if (lpMem = (PBYTE)MapViewOfFile (hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0))
			{
				IMAGE_DOS_HEADER * dos_head =(IMAGE_DOS_HEADER *)lpMem; 
				
				if (dos_head->e_magic == IMAGE_DOS_SIGNATURE)
				{
					PIMAGE_NT_HEADERS peh;
					peh = (PIMAGE_NT_HEADERS32)&lpMem[dos_head->e_lfanew];
					if (peh->Signature==IMAGE_NT_SIGNATURE)
					{
						print (STD_OUTPUT_HANDLE, 0, "Patching %s ...", pszFile);
						if (peh->OptionalHeader.DllCharacteristics & IMAGE_DLLCHARACTERISTICS_TERMINAL_SERVER_AWARE)
						{
							print (STD_ERROR_HANDLE, FOREGROUND_RED, "File %s already TS aware", pszFile);
						}
						else
						{
							peh->OptionalHeader.DllCharacteristics |= IMAGE_DLLCHARACTERISTICS_TERMINAL_SERVER_AWARE;
							print (STD_OUTPUT_HANDLE, FOREGROUND_GREEN, "Done!\n");
							bRet = TRUE;
						}
					}
					else
					{
						print (STD_ERROR_HANDLE, FOREGROUND_RED, "Not a PE file %s: ", pszFile);
					}
				}
				else
				{
					print (STD_ERROR_HANDLE, FOREGROUND_RED, "Not an EXE file %s: ", pszFile);
				}
				UnmapViewOfFile (lpMem);
			}
			else
			{
				print (STD_ERROR_HANDLE, FOREGROUND_RED, "Cannot map view of file %s: ", pszFile);
				ShowError ();
			}
			CloseHandle (hMap);
		}
		else
		{
			print (STD_ERROR_HANDLE, FOREGROUND_RED, "Cannot map view of file %s: ", pszFile);
			ShowError ();
		}
		CloseHandle(hFile);
	}
	else
	{
		print (STD_ERROR_HANDLE, FOREGROUND_RED, "Cannot open file %s: ", pszFile);
		ShowError ();
	}
	return bRet;
}

// ------------------------------------------------------------------------- //

void mainCRTStartup(void)
{
	char *pszFile;
	
	pszFile = PathGetArgs (GetCommandLine());
	print (STD_OUTPUT_HANDLE, 0, "TS-Aware-Patcher 1.0\n\n");
	if (!*pszFile)
	{
		print (STD_OUTPUT_HANDLE, 0, "Usage: settsaware <File.exe>\n");
	}
	else PatchFile (pszFile);
	ExitProcess (0);
}
