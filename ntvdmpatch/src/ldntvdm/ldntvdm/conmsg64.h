#ifndef DEFINE_CONMSG32
#include "basemsg64.h"
#include "x64typespush.h"

#define CONSOLE_CREATESCREENBUFFER_MSG CONSOLE_CREATESCREENBUFFER_MSG64
#define CONSOLE_GRAPHICS_BUFFER_INFO CONSOLE_GRAPHICS_BUFFER_INFO64
#define CONSOLE_API_MSG CONSOLE_API_MSG64
#define PORT_MESSAGE PORT_MESSAGE64

#define CONSRV_SERVERDLL_INDEX          2
#define CONSRV_FIRST_API_NUMBER         512
#define CSR_MAKE_API_NUMBER( DllIndex, ApiIndex ) \
    (CSR_API_NUMBER)(((DllIndex) << 16) | (ApiIndex))

#define CONSOLE_GRAPHICS_BUFFER  2

typedef enum _CONSOLE_API_NUMBER {
	ConsolepOpenConsole = CONSRV_FIRST_API_NUMBER,
	ConsolepGetConsoleInput,
	ConsolepWriteConsoleInput,
	ConsolepReadConsoleOutput,
	ConsolepWriteConsoleOutput,
	ConsolepReadConsoleOutputString,
	ConsolepWriteConsoleOutputString,
	ConsolepFillConsoleOutput,
	ConsolepGetMode,
	ConsolepGetNumberOfFonts,
	ConsolepGetNumberOfInputEvents,
	ConsolepGetScreenBufferInfo,
	ConsolepGetCursorInfo,
	ConsolepGetMouseInfo,
	ConsolepGetFontInfo,
	ConsolepGetFontSize,
	ConsolepGetCurrentFont,
	ConsolepSetMode,
	ConsolepSetActiveScreenBuffer,
	ConsolepFlushInputBuffer,
	ConsolepGetLargestWindowSize,
	ConsolepSetScreenBufferSize,
	ConsolepSetCursorPosition,
	ConsolepSetCursorInfo,
	ConsolepSetWindowInfo,
	ConsolepScrollScreenBuffer,
	ConsolepSetTextAttribute,
	ConsolepSetFont,
	ConsolepSetIcon,
	ConsolepReadConsole,
	ConsolepWriteConsole,
	ConsolepDupHandle,
	ConsolepGetHandleInformation,
	ConsolepSetHandleInformation,
	ConsolepCloseHandle,
	ConsolepVerifyIoHandle,
	ConsolepAlloc,
	ConsolepFree,
	ConsolepGetTitle,
	ConsolepSetTitle,
	ConsolepCreateScreenBuffer,
	ConsolepInvalidateBitmapRect,
	ConsolepVDMOperation,
	ConsolepSetCursor,
	ConsolepShowCursor,
	ConsolepMenuControl,
	ConsolepSetPalette,
	ConsolepSetDisplayMode,
	ConsolepRegisterVDM,
	ConsolepGetHardwareState,
	ConsolepSetHardwareState,
	ConsolepGetDisplayMode,
	ConsolepAddAlias,
	ConsolepGetAlias,
	ConsolepGetAliasesLength,
	ConsolepGetAliasExesLength,
	ConsolepGetAliases,
	ConsolepGetAliasExes,
	ConsolepExpungeCommandHistory,
	ConsolepSetNumberOfCommands,
	ConsolepGetCommandHistoryLength,
	ConsolepGetCommandHistory,
	ConsolepSetCommandHistoryMode,
	ConsolepGetCP,
	ConsolepSetCP,
	ConsolepSetKeyShortcuts,
	ConsolepSetMenuClose,
	ConsolepNotifyLastClose,
	ConsolepGenerateCtrlEvent,
	ConsolepGetKeyboardLayoutName,
	ConsolepGetConsoleWindow,
#if defined(FE_SB) // for Kernel32 Single Binary
	ConsolepCharType,
	ConsolepSetLocalEUDC,
	ConsolepSetCursorMode,
	ConsolepGetCursorMode,
	ConsolepRegisterOS2,
	ConsolepSetOS2OemFormat,
#if defined(FE_IME)
	ConsolepGetNlsMode,
	ConsolepSetNlsMode,
	ConsolepRegisterConsoleIME,
	ConsolepUnregisterConsoleIME,
#endif // FE_IME
#endif // FE_SB
	ConsolepGetLangId,
	ConsolepMaxApiNumber
} CONSOLE_API_NUMBER, *PCONSOLE_API_NUMBER;

#pragma pack(8)
#endif

typedef struct  {
	ULONG_PTR dwBitMapInfoLength;
	LPBITMAPINFO lpBitMapInfo;
	ULONG_PTR dwUsage;
	HANDLE hMutex;
	PVOID lpBitMap;
} CONSOLE_GRAPHICS_BUFFER_INFO;

typedef struct  {
	IN HANDLE ConsoleHandle;
	IN ULONG DesiredAccess;
	IN BOOL InheritHandle;
	IN ULONG ShareMode;
	IN DWORD Flags;
	IN OUT CONSOLE_GRAPHICS_BUFFER_INFO GraphicsBufferInfo;
	OUT HANDLE hMutex;		// 0x30
	OUT PVOID lpBitmap;		// 0x38
	OUT HANDLE Handle;
} CONSOLE_CREATESCREENBUFFER_MSG;


typedef struct  {
    PORT_MESSAGE h;
    PCSR_CAPTURE_HEADER CaptureBuffer;
    CSR_API_NUMBER ApiNumber;
    ULONG ReturnValue;
    ULONG Reserved;
    union {
		// Other messages currently not required to map
		CONSOLE_CREATESCREENBUFFER_MSG CreateConsoleScreenBuffer;
    } u;
} CONSOLE_API_MSG;

#ifndef DEFINE_CONMSG32
#undef CONSOLE_CREATESCREENBUFFER_MSG
#undef CONSOLE_GRAPHICS_BUFFER_INFO
#undef CONSOLE_API_MSG
#undef PORT_MESSAGE

#include "x64typespop.h"
#pragma pack()
#define DEFINE_CONMSG32
#include "conmsg64.h"
#endif
