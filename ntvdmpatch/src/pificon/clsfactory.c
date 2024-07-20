/* Project: pificon
 * Module : clsfactory
 * Author : leecher@dose.0wnz.at
 * Descr. : We need to reconstruct PIF file parser for icons on x64
 *          explorer. This adds a custom COM component that parses the .PIF
 *          files as a shell extension to show the correct icon in explorer.
 *
 *          This module represents the IClassFactory COM component that is 
 *          needed for a Shell extension.
 * Changes: 17.07.2024  - Created
 */
#include <windows.h>
#include <objbase.h>
#include "pificon.h"

extern const CLSID CLSID_PifFile;
long DllRefCount = 0;

HRESULT STDMETHODCALLTYPE ClassFactory_QueryInterface(IClassFactory *this, REFIID riid, void **ppvObject);
ULONG STDMETHODCALLTYPE ClassFactory_AddRef(IClassFactory *this);
ULONG STDMETHODCALLTYPE ClassFactory_Release(IClassFactory *this);
HRESULT STDMETHODCALLTYPE ClassFactory_CreateInstance(IClassFactory *this, IUnknown *pUnkOuter, REFIID riid, void **ppvObject);
HRESULT STDMETHODCALLTYPE ClassFactory_LockServer(IClassFactory *this, BOOL fLock);

IClassFactoryVtbl classFactoryVtbl = 
{
    ClassFactory_QueryInterface,
    ClassFactory_AddRef,
    ClassFactory_Release,
    ClassFactory_CreateInstance,
    ClassFactory_LockServer
};

typedef struct 
{
    IClassFactoryVtbl *lpVtbl;
    LONG refCount;
} ClassFactory;

HRESULT STDMETHODCALLTYPE ClassFactory_QueryInterface(IClassFactory *this, REFIID riid, void **ppvObject) 
{
    if (IsEqualIID(riid, &IID_IUnknown) || IsEqualIID(riid, &IID_IClassFactory)) 
	{
        *ppvObject = this;
        ClassFactory_AddRef(this);
        return S_OK;
    }
    *ppvObject = NULL;
    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE ClassFactory_AddRef(IClassFactory *this) 
{
	InterlockedIncrement(&DllRefCount);
    return InterlockedIncrement(&((ClassFactory *)this)->refCount);
}

ULONG STDMETHODCALLTYPE ClassFactory_Release(IClassFactory *this) 
{
    LONG refCount = InterlockedDecrement(&((ClassFactory *)this)->refCount);
	InterlockedDecrement(&DllRefCount);
    if (refCount == 0)
        HeapFree(GetProcessHeap(), 0, this);

    return refCount;
}

HRESULT STDMETHODCALLTYPE ClassFactory_CreateInstance(IClassFactory *this, IUnknown *pUnkOuter, REFIID riid, void **ppvObject) 
{
    if (pUnkOuter != NULL)
        return CLASS_E_NOAGGREGATION;

    return CPifFile_CreateInstance(riid, ppvObject);
}

HRESULT STDMETHODCALLTYPE ClassFactory_LockServer(IClassFactory *this, BOOL fLock) 
{
    return S_OK;
}

HRESULT ClassFactoryInst_CreateInstance(REFIID riid, void **ppv) 
{
	HRESULT hr;
    ClassFactory *pClassFactory = (ClassFactory *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(ClassFactory));
    if (!pClassFactory)
        return E_OUTOFMEMORY;

    pClassFactory->lpVtbl = &classFactoryVtbl;
    pClassFactory->refCount = 1;
    hr = pClassFactory->lpVtbl->QueryInterface((IClassFactory *)pClassFactory, riid, ppv);
    pClassFactory->lpVtbl->Release((IClassFactory *)pClassFactory);
    return hr;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void **ppv) 
{
    if (IsEqualCLSID(rclsid, &CLSID_PifFile))
        return ClassFactoryInst_CreateInstance(riid, ppv);

    return CLASS_E_CLASSNOTAVAILABLE;
}

HRESULT STDAPICALLTYPE DllCanUnloadNow ()
{
	return DllRefCount == 0?S_OK:S_FALSE;
}
