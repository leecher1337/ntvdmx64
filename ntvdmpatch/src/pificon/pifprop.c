/* Project: pificon
 * Module : pifprop
 * Author : leecher@dose.0wnz.at
 * Descr. : We need to reconstruct PIF file parser for icons on x64
 *          explorer. This adds a custom COM component that parses the .PIF
 *          files as a shell extension to show the correct icon in explorer.
 *
 * Normally, PifMgr - Functions are exported from shell32.dll and there is no
 * need to do extra bindings. However, on 64bit Windows, these functions in 
 * Shell32.dll are just stubs, so NTVDMx64 implements and exposes them in 
 * pifprop.dll
 *
 * Therefore we skip normal binding via shell32.lib and expose our own 
 * function pointers to the module which is #including us
 *
 * Changes: 17.07.2024  - Created
 */
#include <windows.h>
#include <tchar.h>
#include "pifprop.h"

#define TRACE(x) OutputDebugStringA(x)

fpSHELL32_PifMgr_OpenProperties SHELL32_PifMgr_OpenProperties = NULL;
fpSHELL32_PifMgr_GetProperties SHELL32_PifMgr_GetProperties = NULL;
fpSHELL32_PifMgr_SetProperties SHELL32_PifMgr_SetProperties = NULL;
fpSHELL32_PifMgr_CloseProperties SHELL32_PifMgr_CloseProperties = NULL;

BOOL PifProp_LoadDLL(void)
{
	HANDLE hPifProp;
	BOOL fRet;

	if (!(hPifProp = GetModuleHandle(_T("pifprop.dll"))) && !(hPifProp = LoadLibrary(_T("pifprop.dll"))))
	{
		TRACE("PifProp.dll not present\n");
		return FALSE;
	}
	fRet = (SHELL32_PifMgr_OpenProperties = (fpSHELL32_PifMgr_OpenProperties)GetProcAddress(hPifProp, "PifMgr_OpenProperties")) &&
		(SHELL32_PifMgr_GetProperties = (fpSHELL32_PifMgr_GetProperties)GetProcAddress(hPifProp, "PifMgr_GetProperties")) &&
		(SHELL32_PifMgr_SetProperties = (fpSHELL32_PifMgr_SetProperties)GetProcAddress(hPifProp, "PifMgr_SetProperties")) &&
		(SHELL32_PifMgr_CloseProperties = (fpSHELL32_PifMgr_CloseProperties)GetProcAddress(hPifProp, "PifMgr_CloseProperties"));
	if (!fRet)
	{
		TRACE("Pifprop doesn't export Pifmgr functions, maybe better update DLL?\n");
		return FALSE;
	}
	return TRUE;
}

