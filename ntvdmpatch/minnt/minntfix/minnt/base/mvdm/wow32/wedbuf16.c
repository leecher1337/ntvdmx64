/*++
 *
 *  WOW v1.0
 *
 *  Copyright (c) 1991, Microsoft Corporation
 *
 *  WEDBUF16.C
 *  EM_GETHANDLE / EM_SETHANDLE 16:32 buffer bridging for Windows 11 22H2+
 *
 *  History:
 *  Created 16-Sep-2026
 *
 *  EM_GETHANDLE/EM_SETHANDLE let a 16-bit app take direct ownership of an
 *  edit control's text-storage handle. This only worked because the real
 *  UserRegisterWowHandlers() used to route USER32's edit-control text
 *  allocation (ECNcCreate's LOCALALLOC(LHND, cb, hInstance)) through
 *  pfnLocalAlloc -> W32LocalAlloc -> LocalAlloc16, which performs the
 *  allocation for real inside 16-bit USER.EXE's own segment. The
 *  resulting handle's low word was already a genuine, directly-usable
 *  16-bit local handle by construction, which is why wmsgem.c's
 *  GETHMEM16(h) -- a bare truncating cast, see walias.h -- was safe.
 *
 *  On Windows 11 22H2+, UserRegisterWowHandlers() always fails (see
 *  wu32compat.c), so ECNcCreate's allocation collapses to the ordinary
 *  DispatchLocalAlloc -> ordinary public LocalAlloc() (confirmed by
 *  decompiling both the 32-bit Windows 10 and Windows 11 USER32.DLL --
 *  win10/x86 still calls through pfnLocalAlloc as a live indirect call;
 *  win11/x86 has it inlined to DispatchLocalAlloc, which is just
 *  `return LocalAlloc(0x42u, a1);`). That handle has no relationship to
 *  any 16-bit segment, so truncating it via GETHMEM16 produces garbage
 *  for EM_GETHANDLE, and handing USER32 a MarkWOWProc-style packed value
 *  for EM_SETHANDLE gives USER32 nothing it can LocalLock.
 *
 *  This file bridges the two representations by ownership-transfer copy,
 *  matching the real EM_GETHANDLE/EM_SETHANDLE contract (the receiving
 *  side takes over the memory; there's no expectation of a live shared
 *  handle afterwards -- USER32 frees whatever handle a subsequent
 *  EM_SETHANDLE replaces, same as it always did):
 *
 *  - EM_GETHANDLE: copy the real 32-bit LocalAlloc'd text out into a
 *    freshly W32LocalAlloc'd (i.e. genuinely 16-bit-segment-backed)
 *    block and hand that back. The 32-bit block is left alone --
 *    USER32/ECxxx still owns and paints from it until the next
 *    EM_SETHANDLE or the control is destroyed.
 *
 *  - EM_SETHANDLE: copy the app's real 16-bit buffer into a fresh
 *    ordinary LocalAlloc() block and hand USER32 that instead of a
 *    packed marker, so its own LocalLock/LocalSize calls on it work
 *    normally. USER32's EM_SETHANDLE processing takes over its
 *    lifetime from there, same as for a native 32-bit app.
 *
 *  This also covers DS_LOCALEDIT: that style only controlled whether
 *  USER32 pooled all of a dialog's edit controls into one shared
 *  GetEditDS() segment or gave each its own -- GetEditDS/ReleaseEditDS
 *  are themselves entirely gone from Windows 11's USER32.DLL (confirmed
 *  absent from InternalCreateDialog's body, not just unreachable), but
 *  either way every edit control's ped->hText ends up as an ordinary
 *  LocalAlloc() handle now, which is exactly what this file already
 *  handles uniformly.
 *
 *  Called from wmsgem.c's ThunkEMMsg16 (EM_SETHANDLE) and
 *  UnThunkEMMsg16 (EM_GETHANDLE), guarded by #ifdef W10.
--*/

#include "precomp.h"
#pragma hdrstop

MODNAME(wedbuf16.c);

#ifdef W10

//
// EM_GETHANDLE: h32 is the real 32-bit HLOCAL USER32/ECxxx just returned
// (still owned by USER32). hInst16 is the calling task's own 16-bit
// instance handle (wmsgem.c passes CURRENTPTD()->hInst16 -- pww->hMod16
// is only populated for app-registered/superclassed wndprocs, not
// standard system classes like Edit, so it's not safe to rely on here).
// hInst16 just needs to be non-zero so W32LocalAlloc takes its
// LocalAlloc16 branch (see wcall32.c: LOWORD(hInstance) == 0 routes to
// the ordinary, non-16-bit-backed LocalAlloc instead). Returns a
// genuine 16-bit local handle (packed low word = handle, high word =
// DS, matching LocalAlloc16's convention) suitable for GETHMEM16's
// truncation, or 0 on failure.
//
HANDLE WowSyncEditHandleTo16(HANDLE h32, HAND16 hInst16)
{
    LPSTR pSrc;
    LPSTR pDst;
    DWORD cb;
    HANDLE h16;
    HANDLE hInstance = (HANDLE)(ULONG_PTR)hInst16;

    if (!h32 || !hInst16) {
        return 0;
    }

    pSrc = LocalLock(h32);
    if (!pSrc) {
        return 0;
    }

    cb = LocalSize(h32);

    h16 = W32LocalAlloc(LMEM_MOVEABLE, cb, hInstance);
    if (h16) {
        pDst = W32LocalLock(h16, hInstance);
        if (pDst) {
            RtlCopyMemory(pDst, pSrc, cb);
            W32LocalUnlock(h16, hInstance);
        } else {
            W32LocalFree(h16, hInstance);
            h16 = 0;
        }
    }

    LocalUnlock(h32);

    return h16;
}

//
// EM_SETHANDLE: h16 is the app's own real 16-bit local handle (already
// unmarked -- wmsgem.c passes the raw wParam through). hInst16 is the
// calling task's own 16-bit instance handle (see WowSyncEditHandleTo16
// above), needed by W32LocalLock/W32LocalSize the same way. Returns a
// real, ordinary 32-bit HLOCAL with the content copied in, ready to
// hand to the actual EM_SETHANDLE message so USER32's own
// LocalLock/LocalSize calls on it work normally, or 0 on failure.
//
HANDLE WowSyncEditHandleTo32(HAND16 h16, HAND16 hInst16)
{
    LPSTR pSrc;
    LPSTR pDst;
    DWORD cb;
    HANDLE h32;
    HANDLE hMem16 = (HANDLE)(ULONG_PTR)h16;
    HANDLE hInstance = (HANDLE)(ULONG_PTR)hInst16;

    if (!h16 || !hInst16) {
        return 0;
    }

    pSrc = W32LocalLock(hMem16, hInstance);
    if (!pSrc) {
        return 0;
    }

    cb = W32LocalSize(hMem16, hInstance);

    h32 = LocalAlloc(LMEM_MOVEABLE | LMEM_ZEROINIT, cb);
    if (h32) {
        pDst = LocalLock(h32);
        if (pDst) {
            RtlCopyMemory(pDst, pSrc, cb);
            LocalUnlock(h32);
        } else {
            LocalFree(h32);
            h32 = 0;
        }
    }

    W32LocalUnlock(hMem16, hInstance);

    return h32;
}

#endif /* W10 */
