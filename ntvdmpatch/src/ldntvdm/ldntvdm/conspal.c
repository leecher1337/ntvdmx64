/*Project: ldntvdm
* Module : conspal
* Author : leecher@dose.0wnz.at
* Descr. : This module fixes a bug in SetConsolePalette call leading it
*          to fail, because hPalette cannot be made public.
*          For implementation details see mySetConsolePalette
* Changes: 29.05.2020  - Moved to seperate module
*/

#include "ldntvdm.h"
#include "iathook.h"

#if !defined(_WIN64) && !defined(TARGET_WINXP)

typedef BOOL(WINAPI *fpSetConsolePalette)(IN HANDLE hConsoleOutput, IN HPALETTE hPalette, IN UINT dwUsage);
fpSetConsolePalette SetConsolePaletteReal;
typedef BOOL(WINAPI *fpOpenClipboard)(IN HWND hWndNewOwner);
fpOpenClipboard pOpenClipboard = NULL;
typedef HANDLE(WINAPI *fpSetClipboardData)(IN UINT uFormat, IN HANDLE hMem);
fpSetClipboardData pSetClipboardData = NULL;
typedef BOOL(WINAPI *fpCloseClipboard)(VOID);
fpCloseClipboard pCloseClipboard = NULL;

BOOL WINAPI mySetConsolePalette(IN HANDLE hConsoleOutput, IN HPALETTE hPalette, IN UINT dwUsage)
{
	/* This dirty hack via clipboard causes hPalette to become public (GreSetPaletteOwner(hPalette, OBJECT_OWNER_PUBLIC))
	* as Microsoft seemes to have removed NtUserConsoleControl(ConsolePublicPalette, ..) call from kernel :(
	* For the record, ConsoleControl() is a function located in USER32.DLL that could be used, but as said,
	* ConsolePublicPalette call isn't implemented anymore in WIN32k, another stupidity by our friends at M$...
	*/
	BOOL bRet;

	// Avoid USER32.DLL dependency of loader by manually loading functions
	if (!pOpenClipboard)
	{
		HMODULE hUser32 = GetModuleHandle(_T("user32.dll"));
		if (!hUser32 && !(hUser32 = LoadLibrary(_T("user32.dll"))))
		{
			TRACE("Loadlibrary user32.dll fail\n");
			return SetConsolePaletteReal(hConsoleOutput, hPalette, dwUsage);
		}
		pOpenClipboard = (fpOpenClipboard)GetProcAddress(hUser32, "OpenClipboard");
		pSetClipboardData = (fpSetClipboardData)GetProcAddress(hUser32, "SetClipboardData");
		pCloseClipboard = (fpCloseClipboard)GetProcAddress(hUser32, "CloseClipboard");
	}
	pOpenClipboard(NULL);
	pSetClipboardData(CF_PALETTE, hPalette);
	pCloseClipboard();

	bRet = SetConsolePaletteReal(hConsoleOutput, hPalette, dwUsage);
	//TRACE("SetConsolePalette = %d\n", bRet);
	return bRet;
}

void NTAPI HookSetConsolePaletteAPC(ULONG_PTR Parameter)
{
	static int iTries = 0;
	if (Hook_IAT_x64_IAT((LPBYTE)GetModuleHandle(NULL), "KERNEL32.DLL", "SetConsolePalette", mySetConsolePalette, (PULONG_PTR)&SetConsolePaletteReal) == -3)
	{
		HANDLE hThread;
		if (iTries >= 10) return;

		// Queue APC to main thread and try again
		if (hThread = OpenThread(THREAD_SET_CONTEXT, FALSE, *((DWORD*)(__readfsdword(0x18) + 36))))
		{
			if (!QueueUserAPC(HookSetConsolePaletteAPC, hThread, 0))
			{
				TRACE("QueueUserAPC failed gle=%d\n", GetLastError());
			}
			CloseHandle(hThread);
		}
		else { TRACE("OpenThread error %d\n", GetLastError()); }
		iTries++;
	}
}

#endif /* !WIN64 */
