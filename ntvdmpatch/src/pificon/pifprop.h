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
#ifndef __PIFPROP_H___
#define __PIFPROP_H___

typedef HANDLE (STDAPICALLTYPE *fpSHELL32_PifMgr_OpenProperties)(LPCTSTR lpszApp, LPCTSTR lpszPIF, UINT hInf, UINT flOpt);
typedef int (STDAPICALLTYPE  *fpSHELL32_PifMgr_GetProperties)(HANDLE hProps, LPCSTR lpszGroup, void *lpProps, int cbProps, UINT flOpt);
typedef int (STDAPICALLTYPE *fpSHELL32_PifMgr_SetProperties)(HANDLE hProps, LPCSTR lpszGroup, void *lpProps, int cbProps, UINT flOpt);
typedef HANDLE (STDAPICALLTYPE  *fpSHELL32_PifMgr_CloseProperties)(HANDLE hProps, UINT flOpt);

extern fpSHELL32_PifMgr_OpenProperties SHELL32_PifMgr_OpenProperties;
extern fpSHELL32_PifMgr_GetProperties SHELL32_PifMgr_GetProperties;
extern fpSHELL32_PifMgr_SetProperties SHELL32_PifMgr_SetProperties;
extern fpSHELL32_PifMgr_CloseProperties SHELL32_PifMgr_CloseProperties;

#define PifMgr_OpenProperties SHELL32_PifMgr_OpenProperties
#define PifMgr_GetProperties SHELL32_PifMgr_GetProperties 
#define PifMgr_SetProperties SHELL32_PifMgr_SetProperties 
#define PifMgr_CloseProperties SHELL32_PifMgr_CloseProperties 

BOOL PifProp_LoadDLL(void);

#endif
