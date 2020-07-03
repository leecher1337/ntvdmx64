#define WINNT
#define _WIN32_WINNT 0x0501
#define ROUNDUNITS(a,b)    (((a)+(b)-1)/(b))
#define INITGUID
//typedef void BLENDFUNCTION ; // Dummy for SDK headers
#define _SHLGUID_H_
#include <windows.h>
#include "winbasep.h"
#include <shlobj.h>
#include <shlwapi.h>
#include "port32.h"
#undef DEBUG
#include "debug.h"
#include "ccstock.h"
#include "shguidp.h"
#include "shlguid.h"
#include "doshelp.h"
#include "ids.h"
#include "uastrfnc.h"
#ifndef NO_PIF_HDRS
#include "pifmgrp.h"
#include "piffntp.h"
#include "pifinfp.h"
#endif

// If shlobj.h too old...
#ifndef CFSTR_MOUNTEDVOLUME
#define CFSTR_MOUNTEDVOLUME                 TEXT("MountedVolume")
#endif

#define _fstrstr    strstr

// If shlwapi.h too old..
#ifndef SHGetValue
#ifdef UNICODE
#define SHQueryValueEx SHQueryValueExW
#define SHGetValue  SHGetValueW
#define wvnsprintf              wvnsprintfW
#else
#define SHQueryValueEx SHQueryValueExA
#define SHGetValue  SHGetValueA
#define wvnsprintf              wvnsprintfA
#endif
#endif


//----------------------------------------------------------------------------
// Internal helper macro
//----------------------------------------------------------------------------

#define _IOffset(class, itf)         ((UINT)&(((class *)0)->itf))
#define IToClass(class, itf, pitf)   ((class  *)(((LPSTR)pitf)-_IOffset(class, itf)))
#define IToClassN(class, itf, pitf)  IToClass(class, itf, pitf)

/* We want to enforce the use of LOCAL versions of these functions, as on
 * x64 SHELL32.dll, they are exported, but nullstubs
 */
HANDLE STDAPICALLTYPE  myPifMgr_OpenProperties(LPCTSTR lpszApp, LPCTSTR lpszPIF, UINT hInf, UINT flOpt);
int STDAPICALLTYPE  myPifMgr_GetProperties(HANDLE hProps, LPCSTR lpszGroup, void *lpProps, int cbProps, UINT flOpt);
int STDAPICALLTYPE myPifMgr_SetProperties(HANDLE hProps, LPCSTR lpszGroup, void *lpProps, int cbProps, UINT flOpt);
HANDLE STDAPICALLTYPE  myPifMgr_CloseProperties(HANDLE hProps, UINT flOpt);
#define PifMgr_OpenProperties  myPifMgr_OpenProperties
#define PifMgr_GetProperties   myPifMgr_GetProperties
#define PifMgr_SetProperties   myPifMgr_SetProperties
#define PifMgr_CloseProperties myPifMgr_CloseProperties

// lang platform
extern UINT g_uCodePage;

extern HINSTANCE g_hinst; 

extern UINT g_cfHIDA;

#ifdef UNICODE
#define RealRegisterClass   RegisterClassW
#define RealRegisterClassEx RegisterClassExW
#else
#define RealRegisterClass   RegisterClassA
#define RealRegisterClassEx RegisterClassExA
#endif

#define hmemcpy     MoveMemory

// bitbuck.c
void  RelayMessageToChildren(HWND hwnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
BOOL IsFileInBitBucket(LPCTSTR pszPath);


//int WINAPI PifMgr_SetProperties(HANDLE hProps, LPCSTR lpszGroup, void *lpProps, int cbProps, UINT flOpt);
//int WINAPI PifMgr_GetProperties(HANDLE hProps, LPCSTR lpszGroup, void *lpProps, int cbProps, UINT flOpt);
//HANDLE WINAPI PifMgr_CloseProperties(HANDLE hProps, UINT flOpt);
