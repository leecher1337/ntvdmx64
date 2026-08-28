/*++

Copyright (c) 2026  leecher@dose.0wnz.at

Module Name:

    whp_api.c

Abstract:

    Runtime resolver for WinHvPlatform.dll.  All call sites use the
    macros installed by whp_api.h, which rewrite WHv* names to
    g_whp.pWHv* function-pointer reads.  This file owns the storage for
    that table and the LoadLibrary / GetProcAddress logic.

    Whp_api_load() is idempotent: hypervvm_init() calls it once, and a
    second call (from a paranoid caller probing for WHP support) just
    re-returns TRUE.  Whp_api_unload() is provided for symmetry but
    NTVDM never tears the backend down -- the process exits instead.

Environment:

    User mode only

--*/

#include "whp_api.h"

WHP_API g_whp = { 0 };

/*
 * Resolve one export.  Sets *out to the address (cast to PFN_T) and
 * returns FALSE if the symbol is missing -- in which case whp_api_load()
 * tears the partial load down and reports failure.
 */
#define WHP_RESOLVE(name)                                                    \
    do {                                                                     \
        FARPROC _p = GetProcAddress(g_whp.hPlatform, #name);                 \
        if (!_p) { whp_api_unload(); return FALSE; }                         \
        g_whp.p##name = (PFN_##name)_p;                                      \
    } while (0)

/* otya128's wrapper exposes a non-standard diagnostic function that
 * returns a string describing which 64-bit exports it failed to resolve
 * via Heaven's Gate (or NULL when everything resolved).  We query it
 * right after loading the wrapper so we can panic with the specific
 * missing-export list instead of crashing later with a far-call to
 * RIP=0 in the 64-bit half. */
typedef const char* (__cdecl *PFN_whp32_load_status)(void);

const char* whp_api_wrapper_status(void)
{
    PFN_whp32_load_status f;
    if (!g_whp.hPlatform) return "wrapper DLL not loaded";
    f = (PFN_whp32_load_status)GetProcAddress(g_whp.hPlatform,
                                              "whp32_load_status");
    if (!f) return NULL;  /* older wrapper without diag -- assume OK */
    return f();
}

BOOL whp_api_load(void)
{
    if (g_whp.hPlatform)
        return TRUE;

    g_whp.hPlatform = LoadLibraryA("WinHvPlatform.dll");
    if (!g_whp.hPlatform)
        return FALSE;

    WHP_RESOLVE(WHvGetCapability);
    WHP_RESOLVE(WHvCreatePartition);
    WHP_RESOLVE(WHvSetupPartition);
    WHP_RESOLVE(WHvDeletePartition);
    WHP_RESOLVE(WHvSetPartitionProperty);
    WHP_RESOLVE(WHvCreateVirtualProcessor);
    WHP_RESOLVE(WHvDeleteVirtualProcessor);
    WHP_RESOLVE(WHvRunVirtualProcessor);
    WHP_RESOLVE(WHvCancelRunVirtualProcessor);
    WHP_RESOLVE(WHvGetVirtualProcessorRegisters);
    WHP_RESOLVE(WHvSetVirtualProcessorRegisters);
    WHP_RESOLVE(WHvMapGpaRange);
    WHP_RESOLVE(WHvUnmapGpaRange);
    WHP_RESOLVE(WHvTranslateGva);
    WHP_RESOLVE(WHvQueryGpaRangeDirtyBitmap);

    return TRUE;
}

void whp_api_unload(void)
{
    HMODULE h = g_whp.hPlatform;
    memset(&g_whp, 0, sizeof(g_whp));
    if (h) FreeLibrary(h);
}

#undef WHP_RESOLVE
