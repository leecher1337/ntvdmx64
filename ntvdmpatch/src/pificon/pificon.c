/* Project: pificon
 * Module : pificon
 * Author : leecher@dose.0wnz.at
 * Descr. : We need to reconstruct PIF file parser for icons on x64
 *          explorer. This adds a custom COM component that parses the .PIF
 *          files as a shell extension to show the correct icon in explorer.
 *
 *          This module implements the IExtractIcon COM component and 
 *          classes that it inherits from.
 * Changes: 17.07.2024  - Created
 */

/**** WARNING ****
  Do NOT compile this with a PlatformSDK > Windows 7 !
  Vtbl pointers in ShlObj.h got damaged with Windows 8 SDK by removing 
  the IUnknown function pointers form the declaration leading to 
  faulty interface definitions!

  It is completely unclear why this was done, as interface definitions are 
  broken since then.

  It seems that other ppl also noticed it
     https://sourceforge.net/p/mingw-w64/mailman/message/37195869/
  but it was still not fixed. If you have the possibility, file a bug report 
  to Microsoft, tell them to fix their broken COM interface definitions!
 *****************/

#include <windows.h>
#include <shlobj.h>
#include <objbase.h>
#include <tchar.h>

 // Only needed when building for Windows versions where PifMgr_ APIs got stubbed
 // in shell32 (i.e. 64bit Windows versions):
#include "pifprop.h"

// From pif.h
#ifndef GROUP_PRG
#define GROUP_PRG               1           // program group

#define ICONFILE_DEFAULT        "PIFMGR.DLL"
#define ICONINDEX_DEFAULT       0
#endif

#ifndef ARRAYSIZE
#define ARRAYSIZE(x) sizeof(x)/sizeof(x[0])
#endif

#ifdef _DEBUG
#define FENTER(x) OutputDebugString(_T(x));
#else
#define FENTER(x) 
#endif

extern const CLSID CLSID_PifFile;

typedef struct _IconHandler IconHandler;

struct _IconHandler 
{
	struct CShellExtInitImpl { 
		IShellExtInitVtbl FAR *lpVtbl;
		IconHandler *lpObj;
	} m_Shell;

	struct CPersistFileImpl { 
		IPersistFileVtbl FAR *lpVtbl;
		IconHandler *lpObj;
	} m_PersistFile;


	struct CExtractIconImpl { 
		IExtractIconVtbl FAR *lpVtbl;
		IconHandler *lpObj;
	} m_ExtractIcon;

#ifdef UNICODE
	struct CExtractIconAImpl { 
		IExtractIconAVtbl FAR *lpVtbl;
		IconHandler *lpObj;
	} m_ExtractIconA;
#endif

    LONG refCount;
	HANDLE hPifProps;
};

HRESULT STDMETHODCALLTYPE IconHandler_QueryInterface(IShellExtInit *this, REFIID riid, void **ppvObject);
ULONG STDMETHODCALLTYPE IconHandler_AddRef(IShellExtInit *this);
ULONG STDMETHODCALLTYPE IconHandler_Release(IShellExtInit *this);
HRESULT STDMETHODCALLTYPE IconHandler_Initialize(IShellExtInit *this, LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hProgID);

IShellExtInitVtbl iconHandlerVtbl = 
{
    IconHandler_QueryInterface,
    IconHandler_AddRef,
    IconHandler_Release,
    IconHandler_Initialize
};

HRESULT STDMETHODCALLTYPE IconHandlerPersistFile_QueryInterface(IPersistFile *this, REFIID riid, void **ppvObject);
ULONG STDMETHODCALLTYPE IconHandlerPersistFile_AddRef(IPersistFile *this);
ULONG STDMETHODCALLTYPE IconHandlerPersistFile_Release(IPersistFile *this);
HRESULT STDMETHODCALLTYPE IconHandlerPersistFile_GetClassID(IPersistFile *this, CLSID *pClassID);
HRESULT STDMETHODCALLTYPE IconHandlerPersistFile_IsDirty(IPersistFile *this);
HRESULT STDMETHODCALLTYPE IconHandlerPersistFile_Load(IPersistFile *this, LPCOLESTR pszFileName, DWORD dwMode);
HRESULT STDMETHODCALLTYPE IconHandlerPersistFile_Save(IPersistFile *this, LPCOLESTR pszFileName, BOOL fRemember);
HRESULT STDMETHODCALLTYPE IconHandlerPersistFile_SaveCompleted(IPersistFile *this, LPCOLESTR pszFileName);
HRESULT STDMETHODCALLTYPE IconHandlerPersistFile_GetCurFile(IPersistFile *this, LPOLESTR *ppszFileName);

IPersistFileVtbl iconHandlerPersistFileVtbl = 
{
    IconHandlerPersistFile_QueryInterface,
    IconHandlerPersistFile_AddRef,
    IconHandlerPersistFile_Release,
    IconHandlerPersistFile_GetClassID,
    IconHandlerPersistFile_IsDirty,
    IconHandlerPersistFile_Load,
    IconHandlerPersistFile_Save,
    IconHandlerPersistFile_SaveCompleted,
    IconHandlerPersistFile_GetCurFile
};

HRESULT STDMETHODCALLTYPE IconHandlerExtractIcon_QueryInterface(IExtractIcon *this, REFIID riid, void **ppvObject);
ULONG STDMETHODCALLTYPE IconHandlerExtractIcon_AddRef(IExtractIcon *this);
ULONG STDMETHODCALLTYPE IconHandlerExtractIcon_Release(IExtractIcon *this);
HRESULT STDMETHODCALLTYPE IconHandlerExtractIcon_GetIconLocation(IExtractIcon *this, UINT uFlags, LPTSTR szIconFile, UINT cchMax, int *piIndex, UINT *pwFlags);
HRESULT STDMETHODCALLTYPE IconHandlerExtractIcon_Extract(IExtractIcon *this, LPCTSTR pszFile, UINT nIconIndex, HICON *phiconLarge, HICON *phiconSmall, UINT nIconSize);

IExtractIconVtbl iconHandlerExtractIconVtbl = 
{
    IconHandlerExtractIcon_QueryInterface,
    IconHandlerExtractIcon_AddRef,
    IconHandlerExtractIcon_Release,
    IconHandlerExtractIcon_GetIconLocation,
    IconHandlerExtractIcon_Extract
};

#ifdef UNICODE
HRESULT STDMETHODCALLTYPE IconHandlerExtractIconA_QueryInterface(IExtractIconA *this, REFIID riid, void **ppvObject);
ULONG STDMETHODCALLTYPE IconHandlerExtractIconA_AddRef(IExtractIconA *this);
ULONG STDMETHODCALLTYPE IconHandlerExtractIconA_Release(IExtractIconA *this);
HRESULT STDMETHODCALLTYPE IconHandlerExtractIconA_GetIconLocation(IExtractIconA *this, UINT uFlags,LPSTR pszIconFile,UINT cchMax,int *piIndex,UINT * pwFlags);
HRESULT STDMETHODCALLTYPE IconHandlerExtractIconA_Extract(IExtractIconA *this, LPCSTR pszFile,UINT nIconIndex,HICON *phiconLarge,HICON *phiconSmall,UINT nIcons);

IExtractIconAVtbl iconHandlerExtractIconAVtbl = 
{
    IconHandlerExtractIconA_QueryInterface,
    IconHandlerExtractIconA_AddRef,
    IconHandlerExtractIconA_Release,
    IconHandlerExtractIconA_GetIconLocation,
    IconHandlerExtractIconA_Extract
};

#endif

STDAPI_(int) OleStrToStrN(LPTSTR pszOut, int cchOut, LPCWSTR pwsz, int cchWideChar)
{
    int cchOutput;

    if (cchOut > cchWideChar && -1 != cchWideChar)
        cchOut = cchWideChar;

    cchOutput = cchOut;

    while (cchOut)
    {
        if ((*pszOut++ = (TCHAR)*pwsz++) == 0)
            return cchOutput - cchOut + 1;
        cchOut--;
    }

    if (-1 == cchWideChar)
        pszOut--;              // Make room for the null 

    *pszOut = 0;
    return cchOutput;
}



////////////////////////////////////////////////////////////////////////
//  IUnknown
////////////////////////////////////////////////////////////////////////
STDMETHODIMP UnkQueryInterface(
LPUNKNOWN         lpUnkObj,       // Unknown object ptr
REFIID            riidReq,        // IID required
LPVOID FAR *      lplpUnk         // pre for returning the interface
)
{
    IconHandler       *lpObj;

	FENTER("IUnknown::QueryInterface");
    lpObj = ((struct CShellExtInitImpl FAR*)lpUnkObj)->lpObj;

    // DPF1("QueryInterface( %s )\n", DbgGetIIDString(riidReq));

    if (IsEqualIID(riidReq, &IID_IShellExtInit)) {
        *lplpUnk = (LPVOID) &lpObj->m_Shell;
        InterlockedIncrement(&lpObj->refCount);
        return NOERROR;

    } else if (IsEqualIID(riidReq, &IID_IPersistFile)) {
        *lplpUnk = (LPVOID) &lpObj->m_PersistFile;
        InterlockedIncrement(&lpObj->refCount);
        return NOERROR;

    } else if (IsEqualIID(riidReq, &IID_IExtractIcon)) {
        *lplpUnk = (LPVOID) &lpObj->m_ExtractIcon;
        InterlockedIncrement(&lpObj->refCount);
        return NOERROR;
#ifdef UNICODE
    } else if (IsEqualIID(riidReq, &IID_IExtractIconA)) {
        *lplpUnk = (LPVOID) &lpObj->m_ExtractIconA;
        InterlockedIncrement(&lpObj->refCount);
        return NOERROR;
#endif
    } else {
        *lplpUnk = (LPVOID) NULL;
        //DPF1("E_NOINTERFACE\n");
        return E_NOINTERFACE;
    }
}

STDMETHODIMP_(ULONG) UnkAddRef(LPUNKNOWN    lpUnkObj)
{
	FENTER("IUnknown::AddRef");
	return InterlockedIncrement(&((struct CShellExtInitImpl FAR*)lpUnkObj)->lpObj->refCount);
}

STDMETHODIMP_(ULONG) UnkRelease (LPUNKNOWN lpUnkObj)
{
    LONG refCount = InterlockedDecrement(&((struct CShellExtInitImpl FAR*)lpUnkObj)->lpObj->refCount);

	FENTER("IUnknown::Release");
    if (refCount == 0) {
        // DPF1("\n**^*^*^*^*^*^*^*^*^*^*^*^*Refcnt OK\n");
		IconHandler       *lpObj = ((struct CShellExtInitImpl FAR*)lpUnkObj)->lpObj;

		if (lpObj->hPifProps)
		{
			PifMgr_CloseProperties(lpObj->hPifProps, 0);
			lpObj->hPifProps = 0;
		}

		HeapFree(GetProcessHeap(), 0, lpObj);
    }

	return refCount;
} 

////////////////////////////////////////////////////////////////////////
//  IShellExtInit
////////////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE IconHandler_QueryInterface(IShellExtInit *this, REFIID riid, void **ppvObject) 
{
	FENTER("IShellExtInit::QueryInterface");
	return UnkQueryInterface((LPUNKNOWN)this, riid, ppvObject);
}

ULONG STDMETHODCALLTYPE IconHandler_AddRef(IShellExtInit *this) 
{
	FENTER("IShellExtInit::AddRef");
    return UnkAddRef((LPUNKNOWN) this);
}

ULONG STDMETHODCALLTYPE IconHandler_Release(IShellExtInit *this) 
{
	FENTER("IShellExtInit::Release");
	return UnkRelease((LPUNKNOWN) this);
}

HRESULT STDMETHODCALLTYPE IconHandler_Initialize(IShellExtInit *this, LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hProgID) 
{
	FENTER("IShellExtInit::Initialize");
    return S_OK;
}

////////////////////////////////////////////////////////////////////////
//  IPersistFile
////////////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE IconHandlerPersistFile_QueryInterface(IPersistFile *this, REFIID riid, void **ppvObject) 
{
	FENTER("IconHandlerPersistFile::QueryInterface");
    return UnkQueryInterface((LPUNKNOWN)this, riid, ppvObject);
}

ULONG STDMETHODCALLTYPE IconHandlerPersistFile_AddRef(IPersistFile *this) 
{
	FENTER("IconHandlerPersistFile::AddRef");
    return UnkAddRef((LPUNKNOWN) this);
}

ULONG STDMETHODCALLTYPE IconHandlerPersistFile_Release(IPersistFile *this) 
{
	FENTER("IconHandlerPersistFile::Release");
    return UnkRelease((LPUNKNOWN) this);
}

HRESULT STDMETHODCALLTYPE IconHandlerPersistFile_GetClassID(IPersistFile *this, CLSID *pClassID) 
{
	FENTER("IconHandlerPersistFile::GetClassID");
    if (!pClassID) 
        return E_POINTER;

    *pClassID = CLSID_PifFile;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE IconHandlerPersistFile_IsDirty(IPersistFile *this) 
{
	FENTER("IconHandlerPersistFile::IsDirty");
    return S_FALSE;
}

HRESULT STDMETHODCALLTYPE IconHandlerPersistFile_Load(IPersistFile *this, LPCOLESTR pszFileName, DWORD dwMode) 
{
	TCHAR szPath[MAX_PATH];
    IconHandler       *lpObj;

	FENTER("IconHandlerPersistFile::Load");
    lpObj = ((struct CShellExtInitImpl FAR*)this)->lpObj;

    if (pszFileName == NULL)  return E_INVALIDARG;

    OleStrToStrN(szPath, ARRAYSIZE(szPath), pszFileName, -1);

    if (lpObj->hPifProps)
        PifMgr_CloseProperties(lpObj->hPifProps, 0);

    lpObj->hPifProps = PifMgr_OpenProperties(szPath, NULL, 0, 0);

    return lpObj->hPifProps == 0 ? E_FAIL : S_OK;
}

HRESULT STDMETHODCALLTYPE IconHandlerPersistFile_Save(IPersistFile *this, LPCOLESTR pszFileName, BOOL fRemember) 
{
	FENTER("IconHandlerPersistFile::Save");
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IconHandlerPersistFile_SaveCompleted(IPersistFile *this, LPCOLESTR pszFileName) 
{
	FENTER("IconHandlerPersistFile::SaveCompleted");
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IconHandlerPersistFile_GetCurFile(IPersistFile *this, LPOLESTR *ppszFileName) 
{
	FENTER("IconHandlerPersistFile::GetCurFile");
    return E_NOTIMPL;
}

////////////////////////////////////////////////////////////////////////
//  IExtractIcon
////////////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE IconHandlerExtractIcon_QueryInterface(IExtractIcon *this, REFIID riid, void **ppvObject) 
{
	FENTER("IconHandlerExtractIcon::QueryInterface");
    return UnkQueryInterface((LPUNKNOWN)this, riid, ppvObject);
}

ULONG STDMETHODCALLTYPE IconHandlerExtractIcon_AddRef(IExtractIcon *this)
{
	FENTER("IconHandlerExtractIcon::AddRef");
    return UnkAddRef((LPUNKNOWN) this);
}

ULONG STDMETHODCALLTYPE IconHandlerExtractIcon_Release(IExtractIcon *this) 
{
	FENTER("IconHandlerExtractIcon::Release");
    return UnkRelease((LPUNKNOWN) this);
}

HRESULT STDMETHODCALLTYPE IconHandlerExtractIcon_GetIconLocation(IExtractIcon *this, UINT uFlags, LPTSTR szIconFile, UINT cchMax, int *piIndex, UINT *pwFlags) 
{
    PROPPRG ProgramProps;
    IconHandler       *lpObj;

	FENTER("IconHandlerExtractIcon::GetIconLocation");
    lpObj = ((struct CShellExtInitImpl FAR*)this)->lpObj;

    if (lpObj->hPifProps == 0)
        return E_FAIL;

    if (!PifMgr_GetProperties(lpObj->hPifProps,(LPSTR)MAKEINTATOM(GROUP_PRG), &ProgramProps, sizeof(ProgramProps), 0))
    {
        //DebugMsg(DM_TRACE, TEXT("PifFile::GetIconLocation() PifMgr_GetProperties *failed*"));
        return E_FAIL;
    }

    if (ProgramProps.achIconFile[0] == 0)
    {
        lstrcpy(szIconFile, _T(ICONFILE_DEFAULT));
        *piIndex = ICONINDEX_DEFAULT;
    }
    else
    {
#ifdef UNICODE
		MultiByteToWideChar(CP_ACP, 0, ProgramProps.achIconFile, -1, szIconFile, cchMax);
#else
        lstrcpy(szIconFile, ProgramProps.achIconFile);
#endif
        *piIndex = ProgramProps.wIconIndex;
    }
    *pwFlags = 0;

    //DebugMsg(DM_TRACE, TEXT("PifFile::GetIconLocation() ==> %s!%d"), szIconFile, *piIndex);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE IconHandlerExtractIcon_Extract(IExtractIcon *this, LPCTSTR pszFile, UINT nIconIndex, HICON *phiconLarge, HICON *phiconSmall, UINT nIconSize) 
{
	FENTER("IconHandlerExtractIcon::Extract");
    return S_FALSE;
}

#ifdef UNICODE
////////////////////////////////////////////////////////////////////////
//  IExtractIconA
////////////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE IconHandlerExtractIconA_QueryInterface(IExtractIconA *this, REFIID riid, void **ppvObject) 
{
	FENTER("IconHandlerExtractIconA::QueryInterface");
    return UnkQueryInterface((LPUNKNOWN)this, riid, ppvObject);
}

ULONG STDMETHODCALLTYPE IconHandlerExtractIconA_AddRef(IExtractIconA *this)
{
	FENTER("IconHandlerExtractIconA::AddRef");
    return UnkAddRef((LPUNKNOWN) this);
}

ULONG STDMETHODCALLTYPE IconHandlerExtractIconA_Release(IExtractIconA *this)
{
	FENTER("IconHandlerExtractIconA::Release");
    return UnkRelease((LPUNKNOWN) this);
}

HRESULT STDMETHODCALLTYPE IconHandlerExtractIconA_GetIconLocation(IExtractIconA *this, UINT uFlags,LPSTR pszIconFile,UINT cchMax,int *piIndex,UINT * pwFlags)
{
    WCHAR szIconFile[MAX_PATH];
    HRESULT hres;

	FENTER("IconHandlerExtractIconA::GetIconLocation");
    //DebugMsg(DM_TRACE, TEXT("PifFile::IExtractIconA::GetIconLocation()"));

    hres = IconHandlerExtractIcon_GetIconLocation((IExtractIcon*)this, uFlags,szIconFile,ARRAYSIZE(szIconFile),
                                    piIndex, pwFlags);

    //
    // We don't want to copy the icon file name on the S_FALSE case
    //
    if (SUCCEEDED(hres) && hres != S_FALSE)
    {
        WideCharToMultiByte(CP_ACP, 0,
                            szIconFile, -1,
                            pszIconFile, cchMax,
                            NULL, NULL);
    }
    return hres;
}

HRESULT STDMETHODCALLTYPE IconHandlerExtractIconA_Extract(IExtractIconA *this, LPCSTR pszFile,UINT nIconIndex,HICON *phiconLarge,HICON *phiconSmall,UINT nIcons)
{
	FENTER("IconHandlerExtractIconA::Extract");
    //DebugMsg(DM_TRACE, TEXT("PifFile::IExtractIconA::ExtractIcon()"));
    return S_FALSE;
}
#endif

HRESULT CPifFile_CreateInstance(REFIID riid, void **ppv) 
{
    IconHandler *pIconHandler = (IconHandler *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(IconHandler));
	HRESULT hr;
	static BOOL fPifPropLoaded = FALSE;

	FENTER("CPifFile_CreateInstance");
    if (!pIconHandler) {
        return E_OUTOFMEMORY;
    }

#ifdef __PIFPROP_H___
	if (!fPifPropLoaded)
	{
		if (!PifProp_LoadDLL()) return HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND);
		fPifPropLoaded = TRUE;
	}
#endif

    pIconHandler->refCount = 1;

	pIconHandler->m_Shell.lpVtbl = &iconHandlerVtbl;
	pIconHandler->m_Shell.lpObj = pIconHandler;

	pIconHandler->m_PersistFile.lpVtbl = &iconHandlerPersistFileVtbl;
	pIconHandler->m_PersistFile.lpObj = pIconHandler;

	pIconHandler->m_ExtractIcon.lpVtbl = &iconHandlerExtractIconVtbl;
	pIconHandler->m_ExtractIcon.lpObj = pIconHandler;

#ifdef UNICODE
	pIconHandler->m_ExtractIconA.lpVtbl = &iconHandlerExtractIconAVtbl;
	pIconHandler->m_ExtractIconA.lpObj = pIconHandler;
#endif

    hr = pIconHandler->m_Shell.lpVtbl->QueryInterface((IShellExtInit *)pIconHandler, riid, ppv);
    pIconHandler->m_Shell.lpVtbl->Release((IShellExtInit *)pIconHandler);
    return hr;
}
