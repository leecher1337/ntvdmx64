/*Project: ldntvdm
* Module : consbmpxpbug.c
* Author : leecher@dose.0wnz.at
* Descr. : 
*          This module fixes a bug in Windows XP/2003 wow64win.dll:
*          whCreateConsoleScreenBuffer implementation creates as local
*          buffer for call, but fails to return hMutex and lpBitmap buffer
*          to x32 caller. 
*          Isn't that amazing? They failed implementing Console Screen Buffer
*          correctly on any 64bit Windows version since Windows XP!
*/
#include "ldntvdm.h"
#include "Winternl.h"
#include "iathook.h"
#include "csrsswrap.h"
#include "conmsg64.h"
#include "basevdm.h"

#ifdef TARGET_WINXP

#define VALID_ACCESSES (GENERIC_READ | GENERIC_WRITE)
#define VALID_SHARE_ACCESSES (FILE_SHARE_READ | FILE_SHARE_WRITE)

#define GET_CONSOLE_HANDLE (((xpPRTL_USER_PROCESS_PARAMETERS)(NtCurrentPeb())->ProcessParameters)->ConsoleHandle)
#define SET_LAST_ERROR(ERROR) (SetLastError( ERROR ) )
#define SET_LAST_NT_ERROR(ERROR) (SetLastError( RtlNtStatusToDosError( ERROR ) ) )

HANDLE
CreateConsoleScreenBuffer64(
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    CONST SECURITY_ATTRIBUTES *lpSecurityAttributes,
    IN DWORD dwFlags,
    IN PVOID lpScreenBufferData OPTIONAL
    )
{
    CONSOLE_API_MSG64 m;
    CONSOLE_CREATESCREENBUFFER_MSG64 *a = &m.u.CreateConsoleScreenBuffer;
    CONSOLE_GRAPHICS_BUFFER_INFO *GraphicsBufferInfo;
    PCSR_CAPTURE_HEADER CaptureBuffer=NULL;

    if (dwDesiredAccess & ~VALID_ACCESSES ||
        dwShareMode & ~VALID_SHARE_ACCESSES ||
        (dwFlags != CONSOLE_TEXTMODE_BUFFER &&
         dwFlags != CONSOLE_GRAPHICS_BUFFER)) {
		SET_LAST_ERROR(ERROR_INVALID_PARAMETER);
        return (HANDLE) INVALID_HANDLE_VALUE;
    }
    a->ConsoleHandle = GET_CONSOLE_HANDLE;
    a->DesiredAccess = dwDesiredAccess;
    if (ARGUMENT_PRESENT(lpSecurityAttributes)) {
        a->InheritHandle = lpSecurityAttributes->bInheritHandle;
    }
    else {
        a->InheritHandle = FALSE;
    }
    a->ShareMode = dwShareMode;
    a->Flags = dwFlags;
    if (dwFlags == CONSOLE_GRAPHICS_BUFFER) {
        if (a->InheritHandle || lpScreenBufferData == NULL) {
            SET_LAST_ERROR(ERROR_INVALID_PARAMETER);
            return (HANDLE) INVALID_HANDLE_VALUE;
        }
        GraphicsBufferInfo = lpScreenBufferData;
		a->GraphicsBufferInfo.dwBitMapInfoLength = GraphicsBufferInfo->dwBitMapInfoLength;
		a->GraphicsBufferInfo.dwUsage = GraphicsBufferInfo->dwUsage;
		a->GraphicsBufferInfo.hMutex = GraphicsBufferInfo->hMutex;
		a->GraphicsBufferInfo.lpBitMap = GraphicsBufferInfo->lpBitMap;

        CaptureBuffer = CsrAllocateCaptureBuffer( 1, a->GraphicsBufferInfo.dwBitMapInfoLength);
        if (CaptureBuffer == NULL) {
            SET_LAST_ERROR(ERROR_NOT_ENOUGH_MEMORY);
            return (HANDLE) INVALID_HANDLE_VALUE;
        }
        CsrCaptureMessageBuffer( CaptureBuffer,
                                    (PCHAR) GraphicsBufferInfo->lpBitMapInfo,
                                    a->GraphicsBufferInfo.dwBitMapInfoLength,
                                    (PVOID *) &a->GraphicsBufferInfo.lpBitMapInfo
                                );
    }
    CsrClientCallServer( (struct _CSR_API_MESSAGE*)&m,
                         CaptureBuffer,
                         CSR_MAKE_API_NUMBER( CONSRV_SERVERDLL_INDEX,
                                              ConsolepCreateScreenBuffer
                                            ),
                         sizeof( *a )
                       );
    if (CaptureBuffer != NULL) {
        CsrFreeCaptureBuffer( CaptureBuffer );
    }
    if (!NT_SUCCESS( m.ReturnValue)) {
        SET_LAST_NT_ERROR(m.ReturnValue);
        return (HANDLE) INVALID_HANDLE_VALUE;
    }
    else {
        if (dwFlags == CONSOLE_GRAPHICS_BUFFER) {
            GraphicsBufferInfo->hMutex = a->hMutex;
            GraphicsBufferInfo->lpBitMap = a->lpBitmap;
        }
        return (HANDLE)a->Handle;
    }
}

BOOL ConsBmpXpBug_Install(void)
{
	return Hook_IAT_x64_IAT((LPBYTE)GetModuleHandle(NULL), "kernel32.dll", "CreateConsoleScreenBuffer", CreateConsoleScreenBuffer64, NULL);
}

#endif /* TARGET_WINXP */
