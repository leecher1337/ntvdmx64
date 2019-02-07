
#ifndef _HRESULT_DEFINED
#define _HRESULT_DEFINED
typedef LONG HRESULT;

#endif // !_HRESULT_DEFINED

DOSENV *dempGetDosUserEnvironment();

VOID dempInitLFNSupport();
BOOL demIsShortPathName(PCSZ PathName, BOOL bAllowWildcards);
VOID demLFNCleanup();

HRESULT demSetCurrentDirectoryLCDS(UCHAR Drive, LPSTR pszDir);
HRESULT demGetCurrentDirectoryLCDS(UCHAR Drive, LPSTR pszDir);
HRESULT demSetCurrentDirectoryGetDrive(LPSTR lpDirectoryName, PUINT pDrive);

HRESULT demLFNFileTimeControl(WORD SubFunction, LPFILETIME lpFileTime, DOSTIME *DosTime);
HRESULT dempGetFileTimeByHandle(WORD SubFunction, HANDLE hFile, DOSTIME *DosTime);
HRESULT dempSetFileTimeByHandle(BYTE SubFunction, HANDLE hFile, DOSTIME *DosTime);
ULONG   demGetFileTimeByHandle_WOW(HANDLE hFile);

VOID   demWOWLFNInit(PWOWLFNINIT pLFNInit);
ULONG  demWOWLFNEntry(LPVOID pUserFrame);
DWORD  demWOWLFNAllocateSearchHandle(HANDLE hFind);
HANDLE demWOWLFNGetSearchHandle(USHORT DosHandle);
BOOL   demWOWLFNCloseSearchHandle(USHORT DosHandle);

VOID demLFNEntry();
VOID demSetDosVarLocation();
