/* Project: pifprop
 * Module : main
 * Author : leecher@dose.0wnz.at
 * Descr. : We need to reconstruct PIF editor shell extension on x64
 *          explorer. This module glues together all the PIF editor
 *          modules so that they also work on x64 Explorer.
 *          It is using the resources that are still present in SHELL32.MUI
 * Changes: 22.06.2020  - Created
 */

#include "shellprv.h"

#define DllAddRef()
#define DllRelease()

HRESULT CALLBACK CProxyPage_CreateInstance(LPUNKNOWN punkOuter, REFIID riid, LPVOID * ppvOut);

//
// Per-instance Global data (16-bit/32-bit common)
//
#pragma data_seg(DATASEG_PERINSTANCE)
HINSTANCE g_hinst = NULL;
#pragma data_seg()

UINT g_uCodePage = 0; 
UINT g_cfHIDA = 0;

const TCHAR c_szNULL[] = TEXT("");

// bitbcksf.c
void  RelayMessageToChildren(HWND hwnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
    HWND hwndChild;

    for (hwndChild = GetWindow(hwnd, GW_CHILD); hwndChild != NULL; hwndChild = GetWindow(hwndChild, GW_HWNDNEXT))
    {
        SendMessage(hwndChild, uMessage, wParam, lParam);
    }
}

BOOL _ProcessAttach(HINSTANCE hDll)
{
    g_hinst = GetModuleHandle(TEXT("shell32.dll"));

    g_uCodePage = GetACP();
    g_cfHIDA = CF_HDROP;

    return TRUE;
}

void _ProcessDetach(BOOL bProcessShutdown)
{
}

BOOL _ThreadDetach()
{
    return TRUE;
}


typedef struct {
    const IClassFactoryVtbl *cf;
    REFCLSID rclsid;
    HRESULT (CALLBACK *pfnCreate)(IUnknown *, REFIID, void **);
    ULONG flags;
    WCHAR *pszDesc;
} OBJ_ENTRY;

// flags:
#define OBJ_AGGREGATABLE 1

STDMETHODIMP CCF_QueryInterface(IClassFactory *pcf, REFIID riid, void **ppvObj)
{
    // OBJ_ENTRY *this = IToClass(OBJ_ENTRY, cf, pcf);
    if (IsEqualIID(riid, &IID_IClassFactory) || IsEqualIID(riid, &IID_IUnknown))
    {
        *ppvObj = (void *)pcf;
        DllAddRef();
        return NOERROR;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CCF_AddRef(IClassFactory *pcf)
{
    DllAddRef();
    return 2;
}

STDMETHODIMP_(ULONG) CCF_Release(IClassFactory *pcf)
{
    DllRelease();
    return 1;
}

STDMETHODIMP CCF_CreateInstance(IClassFactory *pcf, IUnknown *punkOuter, REFIID riid, void **ppvObject)
{
    OBJ_ENTRY *this = IToClass(OBJ_ENTRY, cf, pcf);

    *ppvObject = NULL; // to avoid nulling it out in every create function...

    if (punkOuter && !(this->flags & OBJ_AGGREGATABLE))
        return CLASS_E_NOAGGREGATION;

    return this->pfnCreate(punkOuter, riid, ppvObject);
}

STDMETHODIMP CCF_LockServer(IClassFactory *pcf, BOOL fLock)
{
/*  SHELL32.DLL does not implement DllCanUnloadNow, thus does not have a DLL refcount
    This means we can never unload!

    if (fLock)
        DllAddRef();
    else
        DllRelease();
*/
    return S_OK;
}

const IClassFactoryVtbl c_CFVtbl = {
    CCF_QueryInterface, CCF_AddRef, CCF_Release,
    CCF_CreateInstance,
    CCF_LockServer
};

const OBJ_ENTRY c_clsmap[] = {
    { &c_CFVtbl, &CLSID_PifProperties,              CProxyPage_CreateInstance, 0, L".PIF file property pages" },
    { NULL, NULL, NULL,0, NULL }

};


STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void **ppv)
{
    if (IsEqualIID(riid, &IID_IClassFactory) || IsEqualIID(riid, &IID_IUnknown))
    {
        const OBJ_ENTRY *pcls;
        for (pcls = c_clsmap; pcls->rclsid; pcls++)
        {
            if (IsEqualIID(rclsid, pcls->rclsid))
            {
                *ppv = (void *)&(pcls->cf);
                DllAddRef();
                return NOERROR;
            }
        }
    }

    *ppv = NULL;
    return CLASS_E_CLASSNOTAVAILABLE;;
}


STDAPI DllCanUnloadNow()
{
    // shell32 won't be able to be unloaded since there are lots of APIs and
    // other non COM things that will need to keep it loaded
    return S_FALSE;
}

STDAPI DllRegisterServer(void)
{
	HKEY hKey, hKey2 = NULL, hKey3 = NULL;
	LPOLESTR lpOleStr = NULL;
	const OBJ_ENTRY *pcls;
	HRESULT hRes = S_OK;

	/* CLSID\{CLSID} and sub keys */
	if (RegOpenKeyW(HKEY_CLASSES_ROOT,L"CLSID",&hKey)!=ERROR_SUCCESS)
		return  E_ACCESSDENIED;


    for (pcls = c_clsmap; pcls->rclsid; pcls++)
    {
		if (StringFromCLSID(pcls->rclsid, &lpOleStr) != S_OK)
		{
			hRes = E_ACCESSDENIED;
			break;
		}
		
		if (RegCreateKeyW(hKey,lpOleStr,&hKey2) != ERROR_SUCCESS ||
			RegSetValueW(hKey2,NULL,REG_SZ ,pcls->pszDesc, wcslen(pcls->pszDesc) * sizeof(WCHAR)) != ERROR_SUCCESS ||
			RegCreateKeyW(hKey2, L"InprocServer32",&hKey3) != ERROR_SUCCESS ||
			RegSetValueW(hKey3,NULL,REG_SZ, L"pifprop.dll", 
				sizeof(L"pifprop.dll") - sizeof(WCHAR)) != ERROR_SUCCESS ||
			RegSetValueW(hKey3, L"ThreadingModel", REG_SZ , L"Apartment", 
				sizeof(L"Apartment")-sizeof(WCHAR)) != ERROR_SUCCESS)

		{
			hRes = E_ACCESSDENIED;
		}
		if (hKey2)
		{
			RegCloseKey(hKey2);
			hKey2 = NULL;
		}
		if (hKey3)
		{
			RegCloseKey(hKey3);
			hKey3 = NULL;
		}
		if (hRes == S_OK && (RegOpenKeyW(HKEY_LOCAL_MACHINE, 
				L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved", &hKey2) != ERROR_SUCCESS ||
			RegSetValueExW(hKey2, lpOleStr, 0, REG_SZ, pcls->pszDesc, wcslen(pcls->pszDesc) * sizeof(WCHAR)) != ERROR_SUCCESS))
		{
			hRes = E_ACCESSDENIED;
		}
		if (hKey2)
		{
			RegCloseKey(hKey2);
			hKey2 = NULL;
		}
		//CoTaskMemFree(lpOleStr);
		//lpOleStr = NULL;
		if (hRes != S_OK) break;
    }
	RegCloseKey(hKey);

	if (hRes == S_OK) 
	{
		if (RegOpenKeyW(HKEY_CLASSES_ROOT, L"piffile\\shellex\\PropertySheetHandlers",&hKey)!=ERROR_SUCCESS)
			return E_ACCESSDENIED;
		//StringFromCLSID(c_clsmap[0].rclsid, &lpOleStr);
		if (RegCreateKeyW(hKey, L"PifProps",&hKey2) != ERROR_SUCCESS ||
			RegSetValueW(hKey2,NULL,REG_SZ ,lpOleStr, wcslen(lpOleStr) * sizeof(WCHAR)) != ERROR_SUCCESS)
		{
			hRes = E_ACCESSDENIED;
		}

		CoTaskMemFree(lpOleStr);
		lpOleStr = NULL;
		RegCloseKey(hKey2);
		RegCloseKey(hKey);
	}
	return hRes;
}


STDAPI DllUnregisterServer(void)
{
	LPOLESTR lpOleStr = NULL;
	const OBJ_ENTRY *pcls;
	HKEY hKey;
	HRESULT hRes = S_OK;
	TCHAR tszKey[512];

    for (pcls = c_clsmap; pcls->rclsid; pcls++)
    {
		if (StringFromCLSID(pcls->rclsid, &lpOleStr) == S_OK)
		{
			
			wsprintf(tszKey, TEXT("CLSID\\%s\\InprocServer32"), lpOleStr);
			RegDeleteKey(HKEY_CLASSES_ROOT, tszKey);
			wsprintf(tszKey, TEXT("CLSID\\%s"), lpOleStr);
			RegDeleteKey(HKEY_CLASSES_ROOT, tszKey);
			CoTaskMemFree(lpOleStr);
			lpOleStr = NULL;
		}
	}
	if (RegOpenKeyW(HKEY_LOCAL_MACHINE, 
		L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved", &hKey) == ERROR_SUCCESS)
	{
		if (StringFromCLSID(c_clsmap[0].rclsid, &lpOleStr) == S_OK)
		{
			RegDeleteValueW(hKey, lpOleStr);
			CoTaskMemFree(lpOleStr);
			lpOleStr = NULL;
		}
		RegCloseKey(hKey);
	}
	RegDeleteKeyW(HKEY_CLASSES_ROOT, L"piffile\\shellex\\PropertySheetHandlers\\PifProps");
	return S_OK;
}

BOOL WINAPI DllMain(HANDLE hDll, DWORD dwReason, LPVOID lpReserved)
{
    switch(dwReason) {
    case DLL_PROCESS_ATTACH:
        _ProcessAttach(hDll);
        break;

    case DLL_PROCESS_DETACH:
        _ProcessDetach(lpReserved != NULL);
        break;

    case DLL_THREAD_DETACH:
        _ThreadDetach();
        break;

    default:
        break;
    }

    return TRUE;
}

