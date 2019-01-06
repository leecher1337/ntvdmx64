/* Project: ldntvdm<
* Module : consbmp.c
* Author : (C) 1989-1993 Microsoft Corporation, fixes by leecher@dose.0wnz.at
* Descr. : CreateConsoleBitmp function in Windows 7 conhost.exe is seriously flawed.
*          The function is needed to create a console screen buffer for drawing via 
*          CreateConsoleScreenBuffer function. For this in order to work, it has to
*          map a shared memory section to the client's process memory so that the 
*          client can draw to the screen buffer. However the implementation in Windows
*          7 assumes the client process to be NtCurrentProcess(), which is conhost.exe
*          instead of the proper console user. 
*          This bug was fortunately fixed in Windows 10, but for Windows 7, we need
*          to reimplement and hook this function in order to fix the bug.
* Changes: 01.01.2018  - Created
*/

#if defined(TARGET_WIN7) && defined(_WIN64)

#include "ldntvdm.h"
#include "consbmp.h"
#include "reg.h"

#define STATUS_SUCCESS                   ((NTSTATUS)0x00000000L)    // ntsubauth
#define DPFLTR_USERGDI_ID 112
#define MAKE_TAG( t ) (RTL_HEAP_MAKE_TAG( *pdwConBaseTag, t ))
#define BMP_TAG 1
#define CONSOLE_CLIENTPROCESSHANDLE() hClientProc
#define NtCurrentProcess() ( (HANDLE)(LONG_PTR) -1 )
#define  HEAP_MAKE_TAG_FLAGS(b, o)   ((ULONG)((b) + ((o) << 16))) 
#define RTL_HEAP_MAKE_TAG HEAP_MAKE_TAG_FLAGS

typedef struct _GRAPHICS_BUFFER_INFO {
	ULONG BitMapInfoLength;
	LPBITMAPINFO lpBitMapInfo;
	PVOID BitMap;
	PVOID ClientBitMap;
	HANDLE ClientProcess;
	HANDLE hMutex;
	HANDLE hSection;
	DWORD dwUsage;
} GRAPHICS_BUFFER_INFO, *PGRAPHICS_BUFFER_INFO;


typedef struct _SCREEN_INFORMATION {
	ULONG Flags;
	DWORD OutputMode;
	COORD ScreenBufferSize; // dimensions of buffer
	SMALL_RECT  Window;       // window location in screen buffer coordinates
	WORD ResizingWindow;   // > 0 if we should ignore WM_SIZE messages
	WORD Attributes;        // attributes of written text
	WORD PopupAttributes;   // attributes of popup text
	BOOLEAN WindowMaximizedX;
	BOOLEAN WindowMaximizedY;
	BOOLEAN WindowMaximized;
	UINT CommandIdLow;
	UINT CommandIdHigh;
	HCURSOR CursorHandle;
	HPALETTE hPalette;
	UINT dwUsage;
	int CursorDisplayCount;
	int WheelDelta;
	union {
		GRAPHICS_BUFFER_INFO GraphicsInfo;
	} BufferInfo;
} SCREEN_INFORMATION, *PSCREEN_INFORMATION;

typedef struct _CONSOLE_GRAPHICS_BUFFER_INFO {
	DWORD dwBitMapInfoLength;
	LPBITMAPINFO lpBitMapInfo;
	DWORD dwUsage;
	HANDLE hMutex;
	PVOID lpBitMap;
} CONSOLE_GRAPHICS_BUFFER_INFO, *PCONSOLE_GRAPHICS_BUFFER_INFO;


typedef ULONG_PTR *(__fastcall *fpFindProcessInList)(HANDLE a1);
fpFindProcessInList FindProcessInList = NULL;

NTSTATUS
CreateConsoleBitmap(
	IN OUT PCONSOLE_GRAPHICS_BUFFER_INFO GraphicsInfo,
	IN OUT PSCREEN_INFORMATION ScreenInfo,
	OUT PVOID *lpBitmap,
	OUT HANDLE *hMutex
	);

typedef enum _SECTION_INHERIT { ViewShare = 1, ViewUnmap = 2 } SECTION_INHERIT;
NTSTATUS NTAPI NtMapViewOfSection(
	_In_        HANDLE          SectionHandle,
	_In_        HANDLE          ProcessHandle,
	_Inout_     PVOID           *BaseAddress,
	_In_        ULONG_PTR       ZeroBits,
	_In_        SIZE_T          CommitSize,
	_Inout_opt_ PLARGE_INTEGER  SectionOffset,
	_Inout_     PSIZE_T         ViewSize,
	_In_        SECTION_INHERIT InheritDisposition,
	_In_        ULONG           AllocationType,
	_In_        ULONG           Win32Protect
	);

NTSTATUS NTAPI NtCreateSection(
	_Out_    PHANDLE            SectionHandle,
	_In_     ACCESS_MASK        DesiredAccess,
	_In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
	_In_opt_ PLARGE_INTEGER     MaximumSize,
	_In_     ULONG              SectionPageProtection,
	_In_     ULONG              AllocationAttributes,
	_In_opt_ HANDLE             FileHandle
	);

static DWORD dwConBaseTag = 0;
static DWORD *pdwConBaseTag = &dwConBaseTag;
static HANDLE pConHeap = INVALID_HANDLE_VALUE;

BOOL ConsBmp_Install(void)
{
	HMODULE hConhost;
	DWORD flOld = 0;
	DWORD dwAddress;
	NTSTATUS Status;
	BYTE *pConsBmp;
	HKEY hKey;
	
	hConhost = GetModuleHandleA("conhost.exe");
	if (NT_SUCCESS(REG_OpenLDNTVDM(KEY_READ, &hKey)))
	{
		if (NT_SUCCESS(REG_QueryDWORD(hKey, L"dwConBaseTag", &dwAddress))) pdwConBaseTag = (DWORD*)((DWORD64)hConhost + dwAddress);
		else pdwConBaseTag = &dwConBaseTag;
		if (NT_SUCCESS(REG_QueryDWORD(hKey, L"FindProcessInList", &dwAddress))) FindProcessInList = (fpFindProcessInList)((DWORD64)hConhost + dwAddress);
		dwAddress = 0;
		if (NT_SUCCESS(REG_QueryDWORD(hKey, L"CreateConsoleBitmap", &dwAddress))) pConsBmp = (DWORD64)hConhost + dwAddress;
		REG_CloseKey(hKey);
	}

	if (!FindProcessInList || !dwAddress)
	{
		TRACE("ConsBmp_Install resolving symbols failed.");
		return FALSE;
	}

	pConHeap = GetProcessHeap();

	// Build jump (jmp [rip - offset])
	if (!VirtualProtect(pConsBmp, 6 + sizeof(ULONG_PTR), PAGE_EXECUTE_READWRITE, &flOld))
	{
		TRACE("ConsBmp_Install hooking failed: VirtualProtect(%08X, %d) -> err=%08X", pConsBmp, 6 + sizeof(ULONG_PTR), GetLastError());
		return FALSE;
	}
	RtlMoveMemory(pConsBmp, "\xFF\x25\x00\x00\x00\x00", 6);
	*((ULONG_PTR*)(pConsBmp + 6)) = (ULONG_PTR)CreateConsoleBitmap;
	VirtualProtect(pConsBmp, 6 + sizeof(ULONG_PTR), flOld, &flOld);

	TRACE("ConsBmp_Install done");

	return TRUE;
}

BOOL
MapHandle(
	IN HANDLE ClientProcessHandle,
	IN HANDLE ServerHandle,
	OUT PHANDLE ClientHandle
	)
{
	//
	// map event handle into dll's handle space.
	//

	return DuplicateHandle(NtCurrentProcess(),
		ServerHandle,
		ClientProcessHandle,
		ClientHandle,
		0,
		FALSE,
		DUPLICATE_SAME_ACCESS
		);
}

NTSTATUS
CreateConsoleBitmap(
    IN OUT PCONSOLE_GRAPHICS_BUFFER_INFO GraphicsInfo,
    IN OUT PSCREEN_INFORMATION ScreenInfo,
    OUT PVOID *lpBitmap,
    OUT HANDLE *hMutex
    )
{
    NTSTATUS Status;
    LARGE_INTEGER MaximumSize;
	PSIZE_T ViewSize;
	HANDLE hClientProc = -1;
	ULONG_PTR phProcessEntry;

	// First we need to get client process handle, extract from supplied address
	if (phProcessEntry = FindProcessInList(*(HANDLE*)((PBYTE)GraphicsInfo - 72)))
		hClientProc = *(HANDLE*)(phProcessEntry + 16);

    //
    // adjust bitmap info
    //

    if (GraphicsInfo->lpBitMapInfo->bmiHeader.biHeight > 0)
    {
		DbgPrintEx(DPFLTR_USERGDI_ID, 1, "*************** Negating biHeight\n");
        GraphicsInfo->lpBitMapInfo->bmiHeader.biHeight =
            -GraphicsInfo->lpBitMapInfo->bmiHeader.biHeight;
    }

    if (GraphicsInfo->lpBitMapInfo->bmiHeader.biCompression != BI_RGB)
    {
		DbgPrintEx(DPFLTR_USERGDI_ID, 1, "*************** setting Compression to BI_RGB)\n");
        GraphicsInfo->lpBitMapInfo->bmiHeader.biCompression = BI_RGB;
    }

    //
    // allocate screeninfo buffer data and copy it
    //

    ScreenInfo->BufferInfo.GraphicsInfo.lpBitMapInfo = (LPBITMAPINFO)HeapAlloc(pConHeap,MAKE_TAG( BMP_TAG ),GraphicsInfo->dwBitMapInfoLength);
    if (ScreenInfo->BufferInfo.GraphicsInfo.lpBitMapInfo == NULL) {
        return STATUS_NO_MEMORY;
    }
    ScreenInfo->BufferInfo.GraphicsInfo.BitMapInfoLength = GraphicsInfo->dwBitMapInfoLength;
	RtlMoveMemory(ScreenInfo->BufferInfo.GraphicsInfo.lpBitMapInfo,
           GraphicsInfo->lpBitMapInfo,
           GraphicsInfo->dwBitMapInfoLength
          );

    //
    // create bitmap section
    //

    MaximumSize.QuadPart = GraphicsInfo->lpBitMapInfo->bmiHeader.biSizeImage;
    Status = NtCreateSection(&ScreenInfo->BufferInfo.GraphicsInfo.hSection,
                             SECTION_ALL_ACCESS,
                             NULL,
                             &MaximumSize,
                             PAGE_READWRITE,
                             SEC_COMMIT,
                             NULL
                            );
    if (!NT_SUCCESS(Status)) {
        HeapFree(pConHeap,0,ScreenInfo->BufferInfo.GraphicsInfo.lpBitMapInfo);
        return Status;
    }

    //
    // map server view of section
    //

    ViewSize = GraphicsInfo->lpBitMapInfo->bmiHeader.biSizeImage;
    ScreenInfo->BufferInfo.GraphicsInfo.BitMap = 0;
    Status = NtMapViewOfSection(ScreenInfo->BufferInfo.GraphicsInfo.hSection,
                                NtCurrentProcess(),
                                &ScreenInfo->BufferInfo.GraphicsInfo.BitMap,
                                0L,
                                GraphicsInfo->lpBitMapInfo->bmiHeader.biSizeImage,
                                NULL,
                                &ViewSize,
                                ViewUnmap,
                                0L,
                                PAGE_READWRITE
                               );
    if (!NT_SUCCESS(Status)) {
        HeapFree(pConHeap,0,ScreenInfo->BufferInfo.GraphicsInfo.lpBitMapInfo);
        NtClose(ScreenInfo->BufferInfo.GraphicsInfo.hSection);
        return Status;
    }

    //
    // map client view of section
    //

    ViewSize = GraphicsInfo->lpBitMapInfo->bmiHeader.biSizeImage;
    *lpBitmap = 0;
    Status = NtMapViewOfSection(ScreenInfo->BufferInfo.GraphicsInfo.hSection,
                                CONSOLE_CLIENTPROCESSHANDLE(),
                                lpBitmap,
                                0L,
                                GraphicsInfo->lpBitMapInfo->bmiHeader.biSizeImage,
                                NULL,
                                &ViewSize,
                                ViewUnmap,
                                0L,
                                PAGE_READWRITE
                               );
    if (!NT_SUCCESS(Status)) {
        HeapFree(pConHeap,0,ScreenInfo->BufferInfo.GraphicsInfo.lpBitMapInfo);
        NtUnmapViewOfSection(NtCurrentProcess(),ScreenInfo->BufferInfo.GraphicsInfo.BitMap);
        NtClose(ScreenInfo->BufferInfo.GraphicsInfo.hSection);
        return Status;
    }
    ScreenInfo->BufferInfo.GraphicsInfo.ClientProcess = CONSOLE_CLIENTPROCESSHANDLE();
    ScreenInfo->BufferInfo.GraphicsInfo.ClientBitMap = *lpBitmap;
	TRACE("Mapped graphics buffer to client memory @%16X", *lpBitmap);

    //
    // create mutex to serialize access to bitmap, then map handle to mutex to client side
    //

    NtCreateMutant(&ScreenInfo->BufferInfo.GraphicsInfo.hMutex,
                   MUTANT_ALL_ACCESS, NULL, FALSE);
    MapHandle(CONSOLE_CLIENTPROCESSHANDLE(),
              ScreenInfo->BufferInfo.GraphicsInfo.hMutex,
              hMutex
             );

    ScreenInfo->BufferInfo.GraphicsInfo.dwUsage = GraphicsInfo->dwUsage;
    ScreenInfo->ScreenBufferSize.X = (WORD)GraphicsInfo->lpBitMapInfo->bmiHeader.biWidth;
    ScreenInfo->ScreenBufferSize.Y = (WORD)-GraphicsInfo->lpBitMapInfo->bmiHeader.biHeight;
    ScreenInfo->Window.Left = 0;
    ScreenInfo->Window.Top = 0;
    ScreenInfo->Window.Right = (SHORT)(ScreenInfo->Window.Left+ScreenInfo->ScreenBufferSize.X-1);
    ScreenInfo->Window.Bottom = (SHORT)(ScreenInfo->Window.Top+ScreenInfo->ScreenBufferSize.Y-1);
    return STATUS_SUCCESS;
}

#endif
