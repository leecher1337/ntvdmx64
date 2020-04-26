#include "ldntvdm.h"
#include "newres.h"
#define WINNT

#define IMAGE_SIZEOF_NT_OPTIONAL_HEADER      224

typedef BITMAPINFOHEADER UNALIGNED *UPBITMAPINFOHEADER;

typedef int
(WINAPI *fpLookupIconIdFromDirectoryEx)(
	_In_reads_bytes_(sizeof(WORD) * 3) PBYTE presbits,
	_In_ BOOL fIcon,
	_In_ int cxDesired,
	_In_ int cyDesired,
	_In_ UINT Flags);


typedef HICON 
(WINAPI *fpCreateIconFromResourceEx)(
	PBYTE presbits,
	DWORD dwResSize,
	BOOL  fIcon,
	DWORD dwVer,
	int   cxDesired,
	int   cyDesired,
	UINT  Flags
	);

extern fpLookupIconIdFromDirectoryEx pLookupIconIdFromDirectoryEx;
extern fpCreateIconFromResourceEx pCreateIconFromResourceEx;

// Redirect win32 functions to dynamically imported functions to avoid user32.dll dependency
#define LookupIconIdFromDirectoryEx pLookupIconIdFromDirectoryEx
#define CreateIconFromResourceEx pCreateIconFromResourceEx
