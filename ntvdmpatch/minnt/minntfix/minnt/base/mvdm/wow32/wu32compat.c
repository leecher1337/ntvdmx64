/*++
 *
 *  WOW v1.0
 *
 *  Copyright (c) 1991, Microsoft Corporation
 *
 *  WU32COMPAT.C
 *  WOW32 USER32 compatibility shims for Windows 11 22H2+
 *
 *  History:
 *  Created 16-Sep-2026
 *
 *  Starting with some Windows 11 22H2 build, USER32.DLL's
 *  UserRegisterWowHandlers() was replaced with:
 *
 *      mov     rax, 0FFFFFFFFC0000022h   ; STATUS_ACCESS_DENIED
 *      retn
 *
 *  Nothing else in USER32.DLL ever takes the address of the ~20 functions
 *  that the real UserRegisterWowHandlers() body used to wire into
 *  apfnWowOut, so the linker's own dead-code elimination quietly stripped
 *  all of them along with it: RegisterClassWOWA, CsCreateWindowEx,
 *  InternalCreateDialog, WowServerLoadCreateCursorIcon/Menu,
 *  DirectedYield, GetFullUserHandle, WowGetDefWindowProcBits, FillWindow,
 *  WOWFindWindow, WOWLoadBitmapA, WOWCleanup/ModuleUnload,
 *  GetClassWOWWords, RegisterUserHungAppHandlers, GetMenuIndex,
 *  YieldTask, WaitForMsgAndEvent. This was not a deliberate
 *  function-by-function purge -- it's fallout from a single function
 *  body swap plus ordinary whole-program dead-code elimination.
 *
 *  The good news, confirmed by diffing a still-working Windows 10 22H2
 *  USER32.DLL against the Windows 11 one: the underlying win32k.sys
 *  syscalls behind most of these (NtUserRegisterClassExWOW,
 *  NtUserFillWindow, ...) are still imported and alive today. What's
 *  gone is only the usermode glue in USER32.DLL that used to marshal
 *  into them through the now-dead registration handshake. Only a
 *  handful of the syscalls themselves (NtUserGetMenuIndex,
 *  NtUserGetWOWClass, NtUserWaitForMsgAndEvent, NtUserWOWCleanup,
 *  NtUserYieldTask) were actually removed.
 *
 *  This file fills in local replacements for the pfnOut table so WOW32
 *  doesn't call through NULL. Where practical we just forward straight
 *  to the ordinary public Win32 API (RegisterClassA, CreateWindowExA)
 *  instead of reimplementing USER32's private wire format:
 *
 *  - The version-compat clamping these entry points used to provide
 *    (old-app style-bit masking, driven by RtlGetExpWinVer) is still
 *    applied by USER32 internally, unconditionally, as long as we pass
 *    through the real hInstance -- confirmed by decompiling the surviving
 *    RegisterClassExWOWA/W internals in Windows 11's USER32.DLL.
 *
 *  - 16-bit wndproc/dlgproc dispatch is already handled independently of
 *    any of this by our own W32Win16WndDispProc hook (wuclass.c), which
 *    substitutes an ordinary callable proc at class-registration time
 *    rather than relying on USER32 calling back into pfnWowWndProcEx.
 *    Neither Windows 10 nor Windows 11 USER32.DLL ever actually calls
 *    pfnWowWndProcEx/pfnWowDlgProcEx from its own code (confirmed by
 *    decompiling both), so the private tagWOWCLS data these entry points
 *    used to forward has nothing left on the other end to feed.
 *
 *  Call RegisterWowHandlersFallback() only after UserRegisterWowHandlers()
 *  has failed -- see wow32.c.
--*/

#include "precomp.h"
#pragma hdrstop

MODNAME(wu32compat.c);

#ifdef W10

//
// xxxGetFullUserHandle's backing store -- see that function further down
// for why this exists. Windows this build creates itself (via
// xxxCsCreateWindowEx / xxxServerCreateDialog below) get tracked here,
// full 32-bit HWND alongside the 16-bit truncated form 16-bit callers
// actually pass back in. A ring buffer rather than a table keyed purely
// by hwnd16: hwnd16 values get reused as windows come and go over a
// session, so a plain "first match wins" lookup would eventually return
// a stale, already-destroyed window's full handle instead of the
// current one -- searching newest-first and just letting old entries
// get overwritten as the ring wraps keeps the lookup naturally
// preferring whichever window is actually still alive.
//
#define MAX_TRACKED_HWNDS 1024

typedef struct _HWNDTRACK {
    BOOL  fInUse;
    HAND16 hwnd16;
    HWND  hwnd32;
} HWNDTRACK;

static HWNDTRACK s_aHwndTrack[MAX_TRACKED_HWNDS];
static int       s_iHwndTrackNext = 0;

VOID TrackFullHwnd(HWND hwnd32)
{
    int    i;
    HAND16 h16;

    if (!hwnd32) {
        return;
    }

    //
    // Reject an already-THIN handle (high word == 0). A genuine full USER
    // handle always carries a non-zero "uniqueness" high word; a value with
    // a zero high word is the very truncated form this table exists to
    // repair. Storing one is worse than not tracking at all: IsWindow()
    // accepts a thin handle as a live window, so a thin entry would win the
    // newest-first lookup in FindTrackedFullHwnd and hand back exactly the
    // useless truncated handle we were trying to avoid. This is not
    // hypothetical -- WM_DRAWITEM for comdlg32's owner-draw dir list arrives
    // here with hwndItem alternately full (e.g. 0x002F0BCE, from comdlg32)
    // and thin (0x00000BCE, from the 16-bit app re-dispatching its own
    // owner-draw paint, its 16-bit handle FULLHWND32'd back before it was
    // ever tracked full); tracking the thin one poisoned the lookup and the
    // dir-list CallWindowProc(prevProc) still got a thin hwnd -> LB_ERR.
    //
    if (HIWORD((ULONG_PTR)hwnd32) == 0) {
        return;
    }

    h16 = (HAND16)(ULONG_PTR)hwnd32;    // matches USER16()'s (HAND16)h32 truncation

    //
    // Dedup: if this exact (hwnd16,hwnd32) pair is already tracked, don't
    // burn another ring slot. Frequent repeat callers -- WM_DRAWITEM as an
    // owner-draw control paints (wmdisp32.c), WindowFromPoint hit-testing
    // (wuwind.c) -- would otherwise flood the ring and evict other windows'
    // still-needed full handles (the very regression the newest-first
    // lookup exists to avoid). Scan newest-first, matching the lookup
    // order, so a control that keeps repainting dedups on the first compare.
    //
    for (i = 1; i <= MAX_TRACKED_HWNDS; i++) {
        int idx = (s_iHwndTrackNext - i + MAX_TRACKED_HWNDS * 2) % MAX_TRACKED_HWNDS;

        if (!s_aHwndTrack[idx].fInUse) {
            continue;
        }
        if (s_aHwndTrack[idx].hwnd16 == h16 && s_aHwndTrack[idx].hwnd32 == hwnd32) {
            return;                     // already tracked -- nothing to do
        }
    }

    i = s_iHwndTrackNext;
    s_aHwndTrack[i].fInUse = TRUE;
    s_aHwndTrack[i].hwnd16 = h16;
    s_aHwndTrack[i].hwnd32 = hwnd32;
    s_iHwndTrackNext = (i + 1) % MAX_TRACKED_HWNDS;
}

//
// Matches PFNCSCREATEWINDOWEX10's calling convention (wuwind.c) -- the
// one x64CreateWindowEx() actually calls through on Windows 10/11
// (iHasZbid > 0). Band is declared DWORD rather than enum ZBID (private
// to wuwind.c): same size, same calling convention, and we never look
// at its value.
//
static HWND WINAPI xxxCsCreateWindowEx(
    DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle,
    int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu,
    HANDLE hInstance, LPVOID lpParam, DWORD Band, DWORD Flags)
{
    HWND hwnd;

    UNREFERENCED_PARAMETER(Band);
    UNREFERENCED_PARAMETER(Flags);   // wow32 only ever passes CW_FLAGS_ANSI

    hwnd = CreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle,
                            X, Y, nWidth, nHeight, hWndParent, hMenu,
                            (HINSTANCE)hInstance, lpParam);
    TrackFullHwnd(hwnd);
    return hwnd;
}

//
// GetClassWOWWords(hInstance, pszClass) (walias.c's FindClass16) is
// documented as "a pointer to the WOW Class structure in the server's
// window class structure ... similar to GetClassLong(hwnd32,
// GCL_WOWWORDS), but ... we have the class name and instance handle"
// instead of an hwnd. Real USER32 stores this as 2 DWORDs tucked into
// the class's own extra bytes; we don't have access to that private
// storage (or a live hwnd to read it back from at lookup time -- that's
// the whole reason this entry point exists), so we keep our own side
// table instead, filled in by xxxRegisterClassWOWA below and consumed
// by xxxGetClassWOWWords / xxxWOWModuleUnload / xxxWOWCleanup further
// down. This does NOT touch the class-extra-byte scheme
// W32Win16WndDispProc/GCL_MY_WNDPROC already relies on (wuclass.c) --
// it's a fully independent, wu32compat-local table.
//
// All of RegisterClass/GetClassWOWWords/ModuleUnload/Cleanup are only
// ever reached from 16-bit API thunks, which by construction run one at
// a time under WOW's own Win16 lock -- no extra synchronization added
// here.
//
#define MAX_TRACKED_WOWCLASSES 256

typedef struct _WOWCLASSTRACK {
    BOOL      fInUse;
    HINSTANCE hInstance;
    HAND16    htask16;         // 16-bit task that registered it -- for per-task cleanup (see UnregisterTrackedClassesByTask)
    CHAR      szClassName[64];
    CHAR      szMenuName[64];  // snapshot of wc.vpszMenu's string content -- see xxxRegisterClassWOWA
    WC        wc;
} WOWCLASSTRACK;

static WOWCLASSTRACK s_aClassTrack[MAX_TRACKED_WOWCLASSES];

//
// lpWndClass is really an LPWNDCLASSA (wuclass.c passes &t1, a WNDCLASS
// whose lpfnWndProc has already been replaced with our own
// W32Win16WndDispProc and whose cbClsExtra has already been bumped by
// GWL_OFFSET before this is ever called -- see WU32RegisterClass).
// pdwWOWstuff is really a WC* -- stashed into our own side table (see
// above) rather than fed to USER32, which has nothing left listening
// for it (see file header comment).
//
static ATOM WINAPI xxxRegisterClassWOWA(PVOID lpWndClass, LPDWORD pdwWOWstuff)
{
    LPWNDCLASSA pwc = (LPWNDCLASSA)lpWndClass;
    ATOM atom = RegisterClassA(pwc);
    int i;

    if (atom && pdwWOWstuff) {
        for (i = 0; i < MAX_TRACKED_WOWCLASSES; i++) {
            if (!s_aClassTrack[i].fInUse) {
                s_aClassTrack[i].fInUse    = TRUE;
                s_aClassTrack[i].hInstance = pwc->hInstance;
                s_aClassTrack[i].htask16   = CURRENTPTD()->htask16;
                lstrcpynA(s_aClassTrack[i].szClassName, pwc->lpszClassName,
                          sizeof(s_aClassTrack[i].szClassName));
                s_aClassTrack[i].wc = *(PWC)pdwWOWstuff;
                //
                // wc.vpszMenu (just copied above) is a raw VDM address, not
                // a snapshot -- fine for the MAKEINTRESOURCE-style integer
                // case (HIWORD==0, nothing to read), but a real string case
                // needs its content captured NOW. pwc->lpszMenuName at this
                // point is the already-flattened, currently-valid copy
                // WU32RegisterClass (wuclass.c) fetched via GETPSZPTR right
                // before calling us -- exactly the bytes the app just set up
                // for this RegisterClass call. Snapshotting it here is what
                // makes it safe to read back later: 16-bit apps routinely
                // reuse the same scratch buffer wc.vpszMenu pointed at for
                // something else entirely by the time a window actually
                // gets created (confirmed live: PaintBrush's "pbParent"
                // class registers with menu name "PBrush2", but by
                // CreateWindow time that same VDM address holds "w" --
                // reused for an unrelated FindResource lookup in between).
                // x64ResolveClassMenu (wuwind.c) reads this snapshot
                // instead of re-fetching from VDM memory live.
                //
                if (HIWORD(((PWC)pdwWOWstuff)->vpszMenu) != 0) {
                    lstrcpynA(s_aClassTrack[i].szMenuName, pwc->lpszMenuName,
                              sizeof(s_aClassTrack[i].szMenuName));
                } else {
                    s_aClassTrack[i].szMenuName[0] = '\0';
                }
                break;
            }
        }
        // Table full: this class just won't be found by
        // xxxGetClassWOWWords/cleaned up by xxxWOWModuleUnload later --
        // degrades gracefully in both (see those functions), so we don't
        // fail the registration itself over it.
    }

    return atom;
}

//
// Shared lookup used by xxxGetClassWOWWords below and by
// WowFindTrackedClass (wu32compat.h), the latter used by wuwind.c to
// resolve a class's registered 16-bit menu name at CreateWindow time --
// see the comment there for why.
//
static PWC FindTrackedClassWC(HINSTANCE hInstance, LPCTSTR pszClass)
{
    int i;

    for (i = 0; i < MAX_TRACKED_WOWCLASSES; i++) {
        if (s_aClassTrack[i].fInUse &&
            s_aClassTrack[i].hInstance == hInstance &&
            lstrcmpiA(s_aClassTrack[i].szClassName, pszClass) == 0) {
            return &s_aClassTrack[i].wc;
        }
    }

    return NULL;
}

PWC WowFindTrackedClass(HINSTANCE hInstance, LPCTSTR pszClass)
{
    return FindTrackedClassWC(hInstance, pszClass);
}

//
// Returns the menu-name string snapshotted at registration time (see
// xxxRegisterClassWOWA) for a tracked class, or NULL if the class isn't
// tracked, has no menu, or its menu name is a MAKEINTRESOURCE-style
// integer id rather than a string (x64ResolveClassMenu, wuwind.c, already
// has the raw wc.vpszMenu value for that case via WowFindTrackedClass).
//
LPCSTR WowFindTrackedClassMenuName(HINSTANCE hInstance, LPCTSTR pszClass)
{
    int i;

    for (i = 0; i < MAX_TRACKED_WOWCLASSES; i++) {
        if (s_aClassTrack[i].fInUse &&
            s_aClassTrack[i].hInstance == hInstance &&
            lstrcmpiA(s_aClassTrack[i].szClassName, pszClass) == 0) {
            return s_aClassTrack[i].szMenuName[0] ? s_aClassTrack[i].szMenuName : NULL;
        }
    }

    return NULL;
}

//
// See xxxRegisterClassWOWA above for what this reads back and why.
//
// PFNGETCLASSWOWWORDS declares a LONG (32-bit) return for what's really
// a pointer -- a leftover from the original 32-bit-only ABI, unchanged
// by this port (FindClass16 in walias.c casts the result straight to
// PWC regardless). That's a pre-existing contract limitation, not
// something introduced here: whatever we return, the caller's call
// site -- typed through this same LONG-returning field -- only reads
// back the low 32 bits. s_aClassTrack is a small fixed-size static in
// this module's own data section, so in practice its address should
// stay well within reach, but this is worth knowing if lookups ever
// come back wrong after an ASLR-heavy relocation.
//
static LONG WINAPI xxxGetClassWOWWords(HINSTANCE hInstance, LPCTSTR pszClass)
{
    PWC pwc = FindTrackedClassWC(hInstance, pszClass);

    // FindClass16 (walias.c) already tolerates and warns on NULL rather
    // than treating it as fatal.
    return pwc ? (LONG)(LONG_PTR)pwc : 0;
}

//
// Real USER32's WOWModuleUnload/WOWCleanup ("UserSrv private api ...
// cleans up any USER objects created by this hModule, most notably
// classes, and subclassed windows" -- wuser.c's ModuleUnload) went
// through NtUserCallOneParam/TwoParam with private SFI__WOWMODULEUNLOAD/
// SFI__WOWCLEANUP opcodes -- and those generic multiplexed syscalls are
// themselves gone from Windows 11's win32u.dll import table (confirmed),
// not just their WOW-specific wrapper, so there's no syscall left to
// forward to either way.
//
// We only track class registrations (see xxxRegisterClassWOWA), not
// subclassed windows or hooks, so this is a partial replacement: it
// unregisters every class this hModule registered through us, but
// anything else the real per-task cleanup used to do is not covered.
//
static VOID UnregisterTrackedClasses(HINSTANCE hInstance)
{
    int i;

    for (i = 0; i < MAX_TRACKED_WOWCLASSES; i++) {
        if (s_aClassTrack[i].fInUse && s_aClassTrack[i].hInstance == hInstance) {
            // Only drop the tracking slot if USER32 actually unregistered the
            // class. If it refused (e.g. a live window of the class still
            // exists), keep the slot so a later re-registration of the same
            // name can still find and retry it (see WowRetryUnregisterClass) --
            // otherwise the class leaks AND we lose the owning-instance record
            // needed to ever unregister it.
            if (UnregisterClassA(s_aClassTrack[i].szClassName, hInstance))
                s_aClassTrack[i].fInUse = FALSE;
        }
    }
}

//
// Per-TASK class cleanup. UnregisterTrackedClasses (above) matches by the
// registering INSTANCE, but a class is frequently registered by a support
// DLL under the DLL's own instance, not the task's -- e.g. Lotus APPROACH's
// "MgrLTS05"/"SmrtIcnLTS05". The per-task teardown (xxxWOWCleanup, fed the
// task's HINSTRES32) therefore never matches those, so they leak across
// sequential app instances on Win11 (real USER32 WOW cleanup, which handled
// this on the owning module's unload, is stubbed here -- see the header note
// above). By the 3rd APPROACH instance the leaked duplicate "MgrLTS05"
// registrations make CreateWindow bind the window to a stale class whose
// 16-bit wndproc (GCL_MY_WNDPROC) is wrong -- it never binds its per-window
// context, and APPROACH GP-faults on teardown. Cleaning up by the task that
// registered the class (recorded at registration time) unregisters those
// DLL-instance classes when the task that pulled them in exits, so the next
// instance starts clean. UnregisterClassA still refuses (and we keep the
// slot) if a live window of the class remains -- e.g. a genuinely shared
// class another task is still using.
//
static VOID UnregisterTrackedClassesByTask(HAND16 htask16)
{
    int i;

    for (i = 0; i < MAX_TRACKED_WOWCLASSES; i++) {
        if (s_aClassTrack[i].fInUse && s_aClassTrack[i].htask16 == htask16) {
            if (UnregisterClassA(s_aClassTrack[i].szClassName,
                                 s_aClassTrack[i].hInstance))
                s_aClassTrack[i].fInUse = FALSE;
        }
    }
}

//
// Recovery for a leaked 16-bit window class on Win11 (see the caller in
// wuclass.c WU32RegisterClass). When RegisterClass fails ERROR_CLASS_ALREADY_
// EXISTS, unregister every tracked registration of this class NAME using the
// original owning instance we recorded at registration time (which is the only
// instance USER32 will accept for UnregisterClass), clearing each slot we
// successfully free. Returns TRUE if at least one was unregistered, so the
// caller can retry RegisterClass. Matching by name (not instance) is deliberate:
// the reloaded module re-registers under a NEW instance, so the stale class's
// owning instance is no longer the caller's.
//
BOOL WowRetryUnregisterClass(LPCSTR pszClass)
{
    int  i;
    BOOL fAny = FALSE;

    for (i = 0; i < MAX_TRACKED_WOWCLASSES; i++) {
        if (s_aClassTrack[i].fInUse &&
            lstrcmpiA(s_aClassTrack[i].szClassName, pszClass) == 0) {
            if (UnregisterClassA(s_aClassTrack[i].szClassName,
                                 s_aClassTrack[i].hInstance)) {
                s_aClassTrack[i].fInUse = FALSE;
                fAny = TRUE;
            }
        }
    }

    return fAny;
}

static BOOL WINAPI xxxWOWModuleUnload(HANDLE hModule)
{
    UnregisterTrackedClasses((HINSTANCE)hModule);
    return TRUE;
}

static BOOL WINAPI xxxWOWCleanup(HANDLE hInstance, DWORD hTaskWow)
{
    UnregisterTrackedClasses((HINSTANCE)hInstance);
    // Also drop classes this task registered under some OTHER instance (a
    // support DLL's), which the by-instance sweep above cannot match -- see
    // UnregisterTrackedClassesByTask. Without this they leak across
    // sequential app instances on Win11 (Lotus APPROACH "MgrLTS05" crash).
    UnregisterTrackedClassesByTask((HAND16)hTaskWow);
    return TRUE;
}

//
// wudlg.c (WU32DialogBoxParam) only reaches this for the non-modal
// (CreateDialog-style) path -- the modal path already goes straight to
// the public DialogBoxIndirectParamAorW and needs nothing from us. cb
// and fFlags (always SCDLG_CLIENT | SCDLG_ANSI | SCDLG_NOREVALIDATE at
// the call site) are USER32-internal bookkeeping with no public
// equivalent to feed; CreateDialogIndirectParamA does the ANSI template
// walk itself from lpDlgTemplate alone.
//
static HWND WINAPI xxxServerCreateDialog(
    HANDLE hmod, LPDLGTEMPLATE lpDlgTemplate, DWORD cb, HWND hwndOwner,
    DLGPROC pfnWndProc, LPARAM dwInitParam, UINT fFlags)
{
    HWND hwnd;

    UNREFERENCED_PARAMETER(cb);
    UNREFERENCED_PARAMETER(fFlags);

    hwnd = CreateDialogIndirectParamA((HINSTANCE)hmod, (LPCDLGTEMPLATEA)lpDlgTemplate,
                                       hwndOwner, pfnWndProc, dwInitParam);
    TrackFullHwnd(hwnd);
    return hwnd;
}

//
// hmod/lpModName/lpName/fClient are only used by wucursor.c's caller for
// its own bookkeeping (SetupResCursorIconAlias) after this returns --
// the actual icon/cursor is built purely from the raw resource bytes
// (pcur/cb), the icon-vs-cursor selector (lpType, passed the same way
// WU32LoadCursor's own fIsCursor check reads it: the low WORD of the
// resource-type value, RT_CURSOR vs RT_ICON), and the resource's own
// version (dwExpWinVer).
//
// dwExpWinVer arrives in the same compact MAKEWORD(minor,major) form as
// every other ExpWinVer value in this codebase (e.g. 0x0300, 0x030A) --
// confirmed from a live log (WU32LoadCursor passing dwExpWinVer=0x0300
// straight through). CreateIconFromResourceEx's dwVer parameter is NOT
// that format -- Microsoft documents exactly two valid values,
// 0x00020000 (Windows 2.0x resources) and 0x00030000 (Windows 3.0+).
// Passing 0x0300 directly made it reject every resource as an
// unrecognized version and return NULL -- this was the actual cause of
// missing cursors/icons, not a graceful degradation. Map the compact
// major-version byte onto the two buckets CreateIconFromResourceEx
// actually understands.
//
// Separately: LoadCursor(NULL, IDC_WAIT)-style *stock system* cursor/
// icon requests have no raw resource bytes at all (hmod and pcur are
// both NULL/0 -- there's nothing to have been extracted from any
// module's resources, because there is no module). Confirmed against
// write16's real source (initwin.c:195): it's the literal first thing
// WinMain does, treats a NULL return as fatal ("We don't even have
// enough memory to tell the user we don't have enough memory" ->
// return FALSE), and the log showed exactly this call
// (LoadCursor(NULL, IDC_WAIT), id 0x7F02) failing and the app exiting
// immediately after. CreateIconFromResourceEx was never going to
// succeed on a NULL pcur regardless of dwVer -- this needs a completely
// different path: WU32LoadCursor passes the stock resource id straight
// through as lpName using the same MAKEINTRESOURCE-disguised-as-integer
// convention the public LoadCursorA/LoadIconA already expect for their
// own stock-resource case, so just ask the OS directly instead of
// trying to parse bytes that don't exist.
//
//
// Set of cursor/icon handles this fallback shim created via
// CreateIconFromResourceEx (PRIVATE handles). On Win7 the equivalent handles are
// module-shared and a 16-bit app's DestroyIcon leaves them alive; here they are
// private, so we record them and let WU32DestroyIcon no-op DestroyIcon on them
// to reproduce the shared-icon lifetime. Serialized by the WOW critical section
// like the rest of these thunks. Bounded; overflow just falls back to real
// destroy (worst case the original bug for that one handle). Not freed here --
// shared icons live until module unload, same as Win7 (acceptable small residue).
//
#define WOW_MAX_SHARED_ICONS 1024
static HICON s_aSharedIcons[WOW_MAX_SHARED_ICONS];
static int   s_cSharedIcons = 0;

void W32TrackSharedIcon(HICON hIcon)
{
    int i;

    if (!hIcon) {
        return;
    }
    for (i = 0; i < s_cSharedIcons; i++) {
        if (s_aSharedIcons[i] == hIcon) {
            return;                     // already tracked
        }
    }
    if (s_cSharedIcons < WOW_MAX_SHARED_ICONS) {
        s_aSharedIcons[s_cSharedIcons++] = hIcon;
    }
}

BOOL W32IsSharedIcon(HICON hIcon)
{
    int i;

    if (!hIcon) {
        return FALSE;
    }
    for (i = 0; i < s_cSharedIcons; i++) {
        if (s_aSharedIcons[i] == hIcon) {
            return TRUE;
        }
    }
    return FALSE;
}

static HCURSOR WINAPI xxxServerLoadCreateCursorIcon(
    HANDLE hmod, LPTSTR lpModName, DWORD dwExpWinVer, LPCTSTR lpName,
    DWORD cb, PVOID pcur, LPTSTR lpType, BOOL fClient)
{
    DWORD dwVer = (HIBYTE(LOWORD(dwExpWinVer)) >= 3) ? 0x00030000 : 0x00020000;
    BOOL  fIsCursor = (WORD)(ULONG_PTR)lpType == (WORD)RT_CURSOR;
    HCURSOR hRet;

    UNREFERENCED_PARAMETER(hmod);
    UNREFERENCED_PARAMETER(lpModName);
    UNREFERENCED_PARAMETER(fClient);

    if (!pcur) {
        // (Obsolete stock OEM cursor ids -- IDC_SIZE 0x7F80 / IDC_ICON 0x7F81 --
        // are remapped up in the WU32LoadCursor thunk so the fix covers the Win7
        // real-registration path too, not just this fallback.) Stock handles are
        // already shared -- no tracking needed.
        return fIsCursor ? LoadCursorA(NULL, lpName)
                          : (HCURSOR)LoadIconA(NULL, lpName);
    }

    // Private handle -- record it so its DestroyIcon becomes a no-op (Win7
    // shared-icon semantics), letting apps reuse a cached, "destroyed" handle.
    hRet = (HCURSOR)CreateIconFromResourceEx((PBYTE)pcur, cb, !fIsCursor,
                                             dwVer, 0, 0, LR_DEFAULTCOLOR);
    W32TrackSharedIcon((HICON)hRet);
    return hRet;
}

//
// hMod/lpName/cb/fCallClient are USER32-internal bookkeeping (cb is
// redundant -- the template is self-describing; fCallClient/lpName feed
// private server-side menu tracking with nothing left to receive it,
// same story as pdwWOWstuff above). LoadMenuIndirectA needs nothing but
// the template itself.
//
static HMENU WINAPI xxxServerLoadCreateMenu(
    HANDLE hMod, LPTSTR lpName, CONST LPMENUTEMPLATE pmt, DWORD cb, BOOL fCallClient)
{
    UNREFERENCED_PARAMETER(hMod);
    UNREFERENCED_PARAMETER(lpName);
    UNREFERENCED_PARAMETER(cb);
    UNREFERENCED_PARAMETER(fCallClient);

    return LoadMenuIndirectA((CONST MENUTEMPLATEA *)pmt);
}

static HWND WINAPI xxxWOWFindWindow(LPCSTR lpClassName, LPCSTR lpWindowName)
{
    return FindWindowA(lpClassName, lpWindowName);
}

//
// hmod/lpName are only informational to the real USER32 entry point
// (resource tracking); the bitmap is built purely from the raw in-memory
// resource bytes, which are laid out exactly like an on-disk .bmp file
// minus the BITMAPFILEHEADER: a BITMAPINFOHEADER, then its color table,
// then the pixel bits -- CreateDIBitmap needs nothing else.
//
static HBITMAP WINAPI xxxWOWLoadBitmapA(HINSTANCE hmod, LPCSTR lpName, LPBYTE pResData, DWORD cbResData)
{
    LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)pResData;
    LPBYTE lpBits;
    DWORD  cColors;
    HDC    hdc;
    HBITMAP hbm;

    UNREFERENCED_PARAMETER(hmod);
    UNREFERENCED_PARAMETER(lpName);
    UNREFERENCED_PARAMETER(cbResData);

    // (Stock OEM bitmaps -- LoadBitmap(NULL, OBM_*), which have no resource
    // bytes -- are served directly in the WU32LoadBitmap thunk so the fix
    // covers the Win7 real-registration path too, not just this fallback.)
    if (!pResData) {
        return NULL;
    }

    cColors = lpbi->biClrUsed ? lpbi->biClrUsed
                               : (lpbi->biBitCount <= 8 ? (1u << lpbi->biBitCount) : 0);
    lpBits  = pResData + lpbi->biSize + cColors * sizeof(RGBQUAD);

    hdc = GetDC(NULL);
    hbm = CreateDIBitmap(hdc, lpbi, CBM_INIT, lpBits, (LPBITMAPINFO)pResData, DIB_RGB_COLORS);
    ReleaseDC(NULL, hdc);

    return hbm;
}

//
// The removed NtUserGetMenuIndex syscall did the same linear search
// server-side; GetSubMenu/GetMenuItemCount are the public equivalent of
// walking the same menu-item list.
//
static DWORD WINAPI xxxGetMenuIndex(HMENU hMenu, HMENU hSubMenu)
{
    int i, cItems = GetMenuItemCount(hMenu);

    for (i = 0; i < cItems; i++) {
        if (GetSubMenu(hMenu, i) == hSubMenu) {
            return (DWORD)i;
        }
    }

    return (DWORD)-1;
}

//
// Real USER32 fills pDefWindowProcBits from its own private
// gSharedInfo.DefWindowMsgs/DefWindowSpecMsgs tables (1 bit = "must go
// to the real 32-bit DefWindowProcA", 0 bit = "16-bit code can handle
// this locally") so 16-bit DefWindowProc can short-circuit messages
// that don't need a round trip -- a pure performance optimization. We
// don't have that private data.
//
// A first version of this returned wMaxDWPMsg = 0 with the buffer left
// zeroed, reasoning that every message ID would then be "out of range"
// and fall through to the real DefWindowProcA by default. That was a
// real, observed bug (startup failure: WOWEXEC's own WM_NCCREATE
// handler returned FALSE, aborting its first CreateWindow) -- we don't
// actually know the 16-bit side's out-of-range fallback behavior
// (we don't have that source), and it apparently defaults the other
// way: msg IDs beyond wMaxDWPMsg get treated as "handle locally"
// (effectively a no-op), starving WM_NCCREATE of real non-client
// processing it needs.
//
// Fixed by not relying on any fallback at all: set every bit within
// the buffer USER.EXE actually gave us, so every message ID it could
// possibly test is explicitly "must go to the real DefWindowProcA",
// with no out-of-range case left for either side to disagree about.
//
static WORD WINAPI xxxWowGetDefWindowProcBits(PBYTE pDefWindowProcBits, WORD cbDefWindowProcBits)
{
    if (pDefWindowProcBits && cbDefWindowProcBits) {
        RtlFillMemory(pDefWindowProcBits, cbDefWindowProcBits, 0xFF);
    }

    return (WORD)(cbDefWindowProcBits * 8 - 1);
}

//
// Public-API equivalent of the documented behavior ("paints a given
// window by using the specified brush", accounting for hwndParent so
// tiled brushes line up across sibling windows): align the brush origin
// to the window's position relative to its parent, then fill the client
// rect. Standard brush-alignment technique, no private entry point
// needed.
//
static VOID WINAPI xxxFillWindow(HWND hwndParent, HWND hwnd, HDC hdc, HANDLE hBrush)
{
    RECT  rc;
    POINT pt = { 0, 0 };

    GetClientRect(hwnd, &rc);
    if (hwndParent) {
        MapWindowPoints(hwnd, hwndParent, &pt, 1);
    }
    SetBrushOrgEx(hdc, -pt.x, -pt.y, NULL);
    FillRect(hdc, &rc, (HBRUSH)hBrush);
}

//
// IS reached -- via the FULLHWND32() macro (walias.h), used directly by
// WU32SetWindowLong's plain-index path (wuwind.c) among others. Real
// USER32 reconstructs the full handle by looking up wHandle's
// "uniqueness" counter in its own private handle table
// (gSharedInfo.aheList), which we don't have access to -- but for
// windows this build created itself, TrackFullHwnd already captured the
// real, full HWND at creation time (see s_aHwndTrack above), so use
// that instead of guessing.
//
// The original MAKELONG(wHandle, 0) zero-extend here (uniqueness bits
// always 0) was flat-out wrong, not just a lower-fidelity fallback:
// confirmed live via a real-window comparison (OLESVR.DLL's own
// "SrvrWndClass" window, hwnd16 0x0930) -- its real, full HWND is
// 0x00110930, and passing the zero-extended 0x00000930 to SetWindowLong
// fails with GetLastError()==ERROR_INVALID_WINDOW_HANDLE (1400).
// GetWindowLong (via wuwind.c's HWND32() macro, a different, also-naive
// sign-extend) tolerates the same incomplete handle on its read path,
// which is why this went unnoticed until a write path hit it -- but
// SetWindowLong validates strictly and rejects it outright, so the
// write silently never happens. That's the direct, confirmed cause of
// the PBRUSHX / OLESVR.DLL GPF: OLESVR.DLL
// stores its own per-window context pointer via
// SetWindowLong(hwnd, 0, ptr) right after creating its window, that
// write was silently failing, and OLESVR.DLL later dereferences the
// still-NULL slot it reads back.
//
static HWND FindTrackedFullHwnd(HAND16 hwnd16)
{
    int i;

    // Search newest-first: i=1 is the most recently written slot
    // ((s_iHwndTrackNext - 1) mod N), i=MAX_TRACKED_HWNDS the oldest --
    // see the s_aHwndTrack comment above for why newest-first matters.
    //
    // The 16-bit handle is only 16 bits, so it is inevitably REUSED across a
    // session: a window is created (tracked), destroyed, and later a brand-new
    // window is handed the same 16-bit handle with a different full HWND. This
    // ring never invalidates a destroyed window's slot, so several tracked
    // entries can share one hwnd16, only one of which (if any) is still a live
    // window. Return the newest match that is STILL A LIVE WINDOW.
    //
    // If NO tracked entry for this handle is live, return NULL so the caller
    // (xxxGetFullUserHandle) falls back to MAKELONG(hwnd16, 0) -- the bare
    // low-word handle, which USER32 on this host resolves to the current live
    // window just fine (verified: IsWindow()==1). Returning a STALE, destroyed
    // full HWND instead is strictly worse than that fallback: it is a dead
    // window (IsWindow()==0), so any SendMessage/CallWindowProc to it silently
    // no-ops. That is exactly what broke WM_SETFONT delivery to the controls of
    // the WinOffice installer's retry dialogs -- the controls resolved to a
    // prior destroyed instance's HWND, never received the app's font, and fell
    // back to the default (heavy) font, while the untracked controls (clean
    // low-word fallback) rendered correctly. A live tracked HWND is still
    // preferred when one exists (e.g. dialogs tracked at WM_INITDIALOG, whose
    // CallWindowProc chain genuinely needs the exact full handle).
    for (i = 1; i <= MAX_TRACKED_HWNDS; i++) {
        int idx = (s_iHwndTrackNext - i + MAX_TRACKED_HWNDS * 2) % MAX_TRACKED_HWNDS;

        if (s_aHwndTrack[idx].fInUse && s_aHwndTrack[idx].hwnd16 == hwnd16 &&
            IsWindow(s_aHwndTrack[idx].hwnd32)) {
            return s_aHwndTrack[idx].hwnd32;         // newest LIVE match wins
        }
    }

    return NULL;   // no live match -> let caller use the low-word fallback
}

//
// REMOVED: an EnumWindows/EnumChildWindows-based last-resort fallback
// used to live here for a 16-bit HWND not found in s_aHwndTrack (a
// window we'd never seen through any of our own proactive tracking
// points). Reverted after a live repro (embedding PBrush from Write)
// produced a real, native access violation in WOW32.DLL itself, and --
// intermittently, i.e. timing-dependent, the signature of a race, not a
// deterministic bug -- a ccpu386/softpc "tried to reallocate real mode
// area" fault (nt_mem.c:1092) in the underlying CPU emulator. Walking
// the entire desktop's window tree via callbacks re-enters win32k.sys
// from wherever FULLHWND32() happens to be called, which can be deep
// inside fragile, mid-instruction VDM/CPU-emulation call contexts --
// exactly the kind of thing that can destabilize the real-mode memory
// manager in a timing-dependent way. Not proven beyond reasonable doubt
// (no symbols were available to resolve the crash address), but severe
// enough, and not load-bearing for anything actually confirmed fixed, to
// pull rather than chase further while it's live. The proactive
// TrackFullHwnd() call sites (xxxCsCreateWindowEx, xxxServerCreateDialog,
// WU32GetDlgItem's, and W32Win16WndDispProc's WM_NCCREATE hook) are
// simple, non-reentrant table writes and stay -- they're what actually
// fixed every confirmed case so far. If another untracked-HWND case
// turns up, prefer adding another proactive TrackFullHwnd() call site
// over reintroducing a whole-desktop enumeration here.
//
static DWORD WINAPI xxxGetFullUserHandle(WORD wHandle)
{
    HWND hwnd = FindTrackedFullHwnd((HAND16)wHandle);

    if (hwnd) {
        return (DWORD)(ULONG_PTR)hwnd;
    }

    // Genuinely doesn't exist as a live window right now (e.g. already
    // destroyed), or not one we've seen through any of our own tracked
    // creation/discovery points -- best effort as before.
    return MAKELONG(wHandle, 0);
}

//
// Let WOW register its own "hung app" recovery UI/handler with USER32
// instead of the generic one -- private hookup, nothing left to
// register into. Windows' generic hang/ghosting detection for wow32's
// own top-level windows (an unresponsive message pump) already applies
// without this, so returning success just means WOW's customized
// handling never fires; it's not a failure callers need to react to.
//
static BOOL WINAPI xxxRegisterUserHungAppHandlers(PFNW32ET pfnW32EndTask, HANDLE hEventWowExec)
{
    UNREFERENCED_PARAMETER(pfnW32EndTask);
    UNREFERENCED_PARAMETER(hEventWowExec);

    return TRUE;
}

//
// aiWowClass indexed by (fnid - FNID_START) -- a fixed, public, stable
// table (standard-control FNID assignments, ntuser/inc/user.h) mapping
// each of the built-in window-manager class ids to its WOWCLASS_*
// equivalent (wudlg.h). Not USER32 state of any kind, just data; copied
// verbatim from the reference UserRegisterWowHandlers's aiClassWow[].
//
static INT s_aiClassWow[] = {
    WOWCLASS_SCROLLBAR,
    WOWCLASS_ICONTITLE,
    WOWCLASS_MENU,
    WOWCLASS_DESKTOP,
    WOWCLASS_WIN16,
    WOWCLASS_WIN16,
    WOWCLASS_SWITCHWND,
    WOWCLASS_BUTTON,
    WOWCLASS_COMBOBOX,
    WOWCLASS_COMBOLBOX,
    WOWCLASS_DIALOG,
    WOWCLASS_EDIT,
    WOWCLASS_LISTBOX,
    WOWCLASS_MDICLIENT,
    WOWCLASS_STATIC,
    WOWCLASS_WIN16,    // 2A9
    WOWCLASS_WIN16,
    WOWCLASS_WIN16,
    WOWCLASS_WIN16,
    WOWCLASS_WIN16,
    WOWCLASS_WIN16,
    WOWCLASS_WIN16,
    WOWCLASS_WIN16,
    WOWCLASS_WIN16,    // 2B1
    WOWCLASS_WIN16,
    WOWCLASS_WIN16,
    WOWCLASS_WIN16,
    WOWCLASS_WIN16,
    WOWCLASS_WIN16,
    WOWCLASS_WIN16,
    WOWCLASS_WIN16
};

//
// gpsi (wow32.c) is NOT dead weight -- WU32NotifyWow's NW_FINALUSERINIT
// handler (wuman.c) relays it straight into 16-bit USER.EXE's own
// wow16gpsi storage cell (usercli.asm) on every process init. We don't
// have the real (private, per-build) SharedInfo struct USER32 normally
// points it at, and nothing in the wow16/user source we have actually
// reads wow16gpsi back out again -- so it may genuinely be inert today.
// But "may be inert" isn't "is inert": returning the sign-extended
// STATUS_ACCESS_DENIED UserRegisterWowHandlers left behind would hand
// 16-bit code a bogus linear address if something *does* read it. Hand
// back a real, owned, zeroed page instead, so a stray read gets zeros
// rather than a fault or garbage. If a 16-bit app misbehaves in a way
// that traces back to this, that's the first thing to revisit.
//
static BYTE s_DummySharedInfo[4096];

//
// TRUE once RegisterWowHandlersFallback() has actually run -- i.e. the
// real UserRegisterWowHandlers() failed (Windows 11 22H2+). #ifdef W10
// alone only means "this is an x64 build"; it does NOT mean registration
// is broken -- e.g. Windows 7 x64 (and, per the user, Windows 10 builds
// before whichever 22H2 update stubbed it) still have a working
// UserRegisterWowHandlers(), so the ORIGINAL pfnLocalAlloc-routed
// behavior is already correct there and must not be second-guessed.
// Code elsewhere (wmsgem.c's EM_GETHANDLE/EM_SETHANDLE, for one) needs
// to check this at runtime, not just #ifdef W10, before assuming
// USER32's own allocations need bridging.
//
BOOL gfW10RegFallback = FALSE;

ULONG_PTR RegisterWowHandlersFallback(APFNWOWHANDLERSIN apfnWowIn, APFNWOWHANDLERSOUT apfnWowOut)
{
    UNREFERENCED_PARAMETER(apfnWowIn);

    gfW10RegFallback = TRUE;

    apfnWowOut->pfnCsCreateWindowEx           = (PFNCSCREATEWINDOWEX)xxxCsCreateWindowEx;
    apfnWowOut->pfnRegisterClassWOWA          = xxxRegisterClassWOWA;
    apfnWowOut->pfnServerCreateDialog         = xxxServerCreateDialog;
    apfnWowOut->pfnServerLoadCreateCursorIcon = xxxServerLoadCreateCursorIcon;
    apfnWowOut->pfnServerLoadCreateMenu       = xxxServerLoadCreateMenu;
    apfnWowOut->pfnWOWFindWindow              = xxxWOWFindWindow;
    apfnWowOut->pfnWOWLoadBitmapA             = xxxWOWLoadBitmapA;
    apfnWowOut->pfnGetMenuIndex               = xxxGetMenuIndex;
    apfnWowOut->pfnWowGetDefWindowProcBits    = xxxWowGetDefWindowProcBits;
    apfnWowOut->pfnFillWindow                 = xxxFillWindow;
    apfnWowOut->pfnGetFullUserHandle          = xxxGetFullUserHandle;
    apfnWowOut->pfnRegisterUserHungAppHandlers = xxxRegisterUserHungAppHandlers;
    apfnWowOut->pfnGetClassWOWWords           = xxxGetClassWOWWords;
    apfnWowOut->pfnWOWCleanup                 = xxxWOWCleanup;
    apfnWowOut->pfnWOWModuleUnload            = xxxWOWModuleUnload;
    apfnWowOut->aiWowClass                    = s_aiClassWow;

    //
    // pfnInitTask is NOT filled in here -- its only call site (wkman.c)
    // is already wrapped in #ifndef W10 ("Unfortunately, no NtUserInitTask
    // support..."), so it's never called in this build regardless of
    // whether registration succeeded.
    //
    // pfnYieldTask / pfnDirectedYield / pfnWowWaitForMsgAndEvent are
    // handled separately in wow32.c (xxxUserYield / xxxDirectedYield /
    // xxxWaitForMsgAndEvent, wtask.c) unconditionally, regardless of
    // whether registration succeeded, so they don't need filling in
    // here either.
    //

    return (ULONG_PTR)s_DummySharedInfo;
}

#endif /* W10 */
