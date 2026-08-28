/*++

Copyright (c) 2026  leecher@dose.0wnz.at

Module Name:

    whp_api.h

Abstract:

    Dynamic loader for the Windows Hypervisor Platform user-mode APIs.

    We do not link against WinHvPlatform.lib (and we never needed
    WinHvEmulation.lib, because the NTVDM backend handles every exit
    itself).  Linking statically would make NTVDMx64.exe fail to load on
    any OS that does not ship WinHvPlatform.dll -- Windows XP, Windows 7,
    and any modern Windows install where the optional "Windows Hypervisor
    Platform" feature has not been enabled.

    Instead this header declares function-pointer typedefs for every WHP
    routine we use, plus a global g_whp table, and re-#defines the WHv*
    names so call sites stay textually identical to the static-link
    version.  Resolution happens once via whp_api_load(), which is the
    first thing hypervvm_init() does.  If LoadLibrary or any of the
    GetProcAddress lookups fails, whp_api_load() returns FALSE and the
    backend can bail with the standard "WHP not available" panic --
    exactly the same code path as "hypervisor not present" on a CPU
    without VT-x/AMD-V.

Environment:

    User mode only

--*/

#pragma once
#include <windows.h>

/* -------------------------------------------------------------------------
 * 32-bit-on-x64 ABI workaround (lifted from winevdm/whpxvm).
 *
 * The Windows SDK's <WinHvPlatform.h> defines:
 *
 *     typedef VOID *WHV_PARTITION_HANDLE;
 *
 * On a 32-bit build this resolves to a 4-byte type, but the actual 32-bit
 * WinHvPlatform.dll shipped in %SystemRoot%\SysWOW64 was compiled with
 * WHV_PARTITION_HANDLE as a 64-bit value -- the partition is identified
 * by a fixed 64-bit ID that the user-mode shim hands straight to the
 * kernel.  If we call the DLL with a 4-byte handle, we push 4 bytes
 * where it expects 8, mis-align every subsequent stack arg, and crash
 * inside the DLL on the first call.
 *
 * Fix: include WinHvPlatformDefs.h with the typedef hidden under a
 * renamed identifier so the struct/enum definitions still parse, then
 * redefine WHV_PARTITION_HANDLE as UINT64 before pulling in the
 * function prototypes from WinHvPlatform.h.  Every other type
 * (WHV_GUEST_PHYSICAL_ADDRESS, WHV_REGISTER_NAME, etc.) is already a
 * UINT64/enum and needs no fixup.  This compiles to a no-op on a 64-bit
 * build because UINT64 == sizeof(void*) there.
 * ------------------------------------------------------------------------- */
#define WHV_PARTITION_HANDLE WHV_PARTITION_HANDLE_SDK_HIDDEN
#include <WinHvPlatformDefs.h>
#undef  WHV_PARTITION_HANDLE
typedef UINT64 WHV_PARTITION_HANDLE;
#include <WinHvPlatform.h>

/* Trip the build immediately if some other translation unit (or a PCH)
 * pulled in <WinHvPlatform.h> before this header and the SDK's
 * "typedef VOID *WHV_PARTITION_HANDLE" stuck.  If you ever see this
 * fire on a 32-bit build, search the project for stray includes of
 * <WinHvPlatform.h> / <WinHvPlatformDefs.h> and replace them with
 * "whp_api.h". */
typedef char __whp_api_handle_size_check[sizeof(WHV_PARTITION_HANDLE) == 8 ? 1 : -1];

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------
 * Function-pointer typedefs.  Signatures are copied verbatim from the
 * WinHvPlatform.h SDK declarations -- if Microsoft ever changes one we'd
 * notice on the next SDK update because the typedef would no longer match
 * the macro-redirected call.
 * ------------------------------------------------------------------------- */

typedef HRESULT (WINAPI *PFN_WHvGetCapability)(
    WHV_CAPABILITY_CODE CapabilityCode,
    VOID *CapabilityBuffer,
    UINT32 CapabilityBufferSizeInBytes,
    UINT32 *WrittenSizeInBytes);

typedef HRESULT (WINAPI *PFN_WHvCreatePartition)(
    WHV_PARTITION_HANDLE *Partition);

typedef HRESULT (WINAPI *PFN_WHvSetupPartition)(
    WHV_PARTITION_HANDLE Partition);

typedef HRESULT (WINAPI *PFN_WHvDeletePartition)(
    WHV_PARTITION_HANDLE Partition);

typedef HRESULT (WINAPI *PFN_WHvSetPartitionProperty)(
    WHV_PARTITION_HANDLE Partition,
    WHV_PARTITION_PROPERTY_CODE PropertyCode,
    const VOID *PropertyBuffer,
    UINT32 PropertyBufferSizeInBytes);

typedef HRESULT (WINAPI *PFN_WHvCreateVirtualProcessor)(
    WHV_PARTITION_HANDLE Partition,
    UINT32 VpIndex,
    UINT32 Flags);

typedef HRESULT (WINAPI *PFN_WHvDeleteVirtualProcessor)(
    WHV_PARTITION_HANDLE Partition,
    UINT32 VpIndex);

typedef HRESULT (WINAPI *PFN_WHvRunVirtualProcessor)(
    WHV_PARTITION_HANDLE Partition,
    UINT32 VpIndex,
    VOID *ExitContext,
    UINT32 ExitContextSizeInBytes);

typedef HRESULT (WINAPI *PFN_WHvCancelRunVirtualProcessor)(
    WHV_PARTITION_HANDLE Partition,
    UINT32 VpIndex,
    UINT32 Flags);

typedef HRESULT (WINAPI *PFN_WHvGetVirtualProcessorRegisters)(
    WHV_PARTITION_HANDLE Partition,
    UINT32 VpIndex,
    const WHV_REGISTER_NAME *RegisterNames,
    UINT32 RegisterCount,
    WHV_REGISTER_VALUE *RegisterValues);

typedef HRESULT (WINAPI *PFN_WHvSetVirtualProcessorRegisters)(
    WHV_PARTITION_HANDLE Partition,
    UINT32 VpIndex,
    const WHV_REGISTER_NAME *RegisterNames,
    UINT32 RegisterCount,
    const WHV_REGISTER_VALUE *RegisterValues);

typedef HRESULT (WINAPI *PFN_WHvMapGpaRange)(
    WHV_PARTITION_HANDLE Partition,
    VOID *SourceAddress,
    WHV_GUEST_PHYSICAL_ADDRESS GuestAddress,
    UINT64 SizeInBytes,
    WHV_MAP_GPA_RANGE_FLAGS Flags);

typedef HRESULT (WINAPI *PFN_WHvUnmapGpaRange)(
    WHV_PARTITION_HANDLE Partition,
    WHV_GUEST_PHYSICAL_ADDRESS GuestAddress,
    UINT64 SizeInBytes);

typedef HRESULT (WINAPI *PFN_WHvTranslateGva)(
    WHV_PARTITION_HANDLE Partition,
    UINT32 VpIndex,
    WHV_GUEST_VIRTUAL_ADDRESS Gva,
    WHV_TRANSLATE_GVA_FLAGS TranslateFlags,
    WHV_TRANSLATE_GVA_RESULT *TranslationResult,
    WHV_GUEST_PHYSICAL_ADDRESS *Gpa);

typedef HRESULT (WINAPI *PFN_WHvQueryGpaRangeDirtyBitmap)(
    WHV_PARTITION_HANDLE Partition,
    WHV_GUEST_PHYSICAL_ADDRESS GuestAddress,
    UINT64 RangeSizeInBytes,
    UINT64 *Bitmap,
    UINT32 BitmapSizeInBytes);

/* -------------------------------------------------------------------------
 * Resolved-function table.  Populated by whp_api_load(); zeroed otherwise.
 * ------------------------------------------------------------------------- */
typedef struct {
    HMODULE                              hPlatform;
    PFN_WHvGetCapability                 pWHvGetCapability;
    PFN_WHvCreatePartition               pWHvCreatePartition;
    PFN_WHvSetupPartition                pWHvSetupPartition;
    PFN_WHvDeletePartition               pWHvDeletePartition;
    PFN_WHvSetPartitionProperty          pWHvSetPartitionProperty;
    PFN_WHvCreateVirtualProcessor        pWHvCreateVirtualProcessor;
    PFN_WHvDeleteVirtualProcessor        pWHvDeleteVirtualProcessor;
    PFN_WHvRunVirtualProcessor           pWHvRunVirtualProcessor;
    PFN_WHvCancelRunVirtualProcessor     pWHvCancelRunVirtualProcessor;
    PFN_WHvGetVirtualProcessorRegisters  pWHvGetVirtualProcessorRegisters;
    PFN_WHvSetVirtualProcessorRegisters  pWHvSetVirtualProcessorRegisters;
    PFN_WHvMapGpaRange                   pWHvMapGpaRange;
    PFN_WHvUnmapGpaRange                 pWHvUnmapGpaRange;
    PFN_WHvTranslateGva                  pWHvTranslateGva;
    PFN_WHvQueryGpaRangeDirtyBitmap      pWHvQueryGpaRangeDirtyBitmap;
} WHP_API;

extern WHP_API g_whp;

/*
 * Resolve WinHvPlatform.dll and all required entry points.  Idempotent.
 * Returns TRUE on success, FALSE if the DLL is missing (older Windows /
 * WHP feature off) or any required export is absent.  No message box,
 * no panic -- the caller decides how to react.
 */
BOOL whp_api_load(void);

/* Drop the DLL handle and zero the function pointers. */
void whp_api_unload(void);

/*
 * Diagnostic for the otya128 32-bit wrapper.  Returns NULL when its
 * internal Heaven's Gate resolution of the 64-bit WinHvPlatform.dll
 * succeeded for every entry point we need, otherwise a static string
 * naming the 64-bit hmod and listing the missing exports (one per line).
 * If the wrapper doesn't export the diag function (older build), returns
 * NULL and we assume things are fine.
 */
const char* whp_api_wrapper_status(void);

/* -------------------------------------------------------------------------
 * Macro redirection.  Each WHv* function name is rewritten to read from
 * the loaded table, so every call site in monitor.c / sas.c / int.c can
 * stay textually identical to the static-link build.  These macros only
 * affect plain function-call uses; enum tags (WHvX64Register*,
 * WHvRunVpExitReason*, etc.) are untouched.
 *
 * IMPORTANT: this redirection must come AFTER the WinHvPlatform.h
 * prototypes have been parsed (they would otherwise become syntactically
 * malformed: "HRESULT WINAPI g_whp.pWHvCreatePartition(...);").  Because
 * whp_api.h pulls WinHvPlatform.h in first, including whp_api.h is safe
 * from anywhere.
 * ------------------------------------------------------------------------- */
#define WHvGetCapability                  g_whp.pWHvGetCapability
#define WHvCreatePartition                g_whp.pWHvCreatePartition
#define WHvSetupPartition                 g_whp.pWHvSetupPartition
#define WHvDeletePartition                g_whp.pWHvDeletePartition
#define WHvSetPartitionProperty           g_whp.pWHvSetPartitionProperty
#define WHvCreateVirtualProcessor         g_whp.pWHvCreateVirtualProcessor
#define WHvDeleteVirtualProcessor         g_whp.pWHvDeleteVirtualProcessor
#define WHvRunVirtualProcessor            g_whp.pWHvRunVirtualProcessor
#define WHvCancelRunVirtualProcessor      g_whp.pWHvCancelRunVirtualProcessor
#define WHvGetVirtualProcessorRegisters   g_whp.pWHvGetVirtualProcessorRegisters
#define WHvSetVirtualProcessorRegisters   g_whp.pWHvSetVirtualProcessorRegisters
#define WHvMapGpaRange                    g_whp.pWHvMapGpaRange
#define WHvUnmapGpaRange                  g_whp.pWHvUnmapGpaRange
#define WHvTranslateGva                   g_whp.pWHvTranslateGva
#define WHvQueryGpaRangeDirtyBitmap       g_whp.pWHvQueryGpaRangeDirtyBitmap

#ifdef __cplusplus
} /* extern "C" */
#endif
