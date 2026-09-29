/*++
 *
 *  WOW v1.0
 *
 *  Copyright (c) 1991, Microsoft Corporation
 *
 *  WU32COMPAT.H
 *  WOW32 USER32 compatibility shims for Windows 11 22H2+
 *
 *  History:
 *  Created 16-Sep-2026
--*/

#ifdef W10

//
// TRUE once RegisterWowHandlersFallback() has actually run, i.e. the
// real UserRegisterWowHandlers() failed at startup. NOT the same thing
// as #ifdef W10 (an x64 build doesn't imply a broken registration --
// e.g. Windows 7 x64 still has a working one). Anything that needs to
// know whether USER32's own allocations/behavior are trustworthy as-is
// must check this flag at runtime, not just compile under W10.
//
extern BOOL gfW10RegFallback;

//
// Approach (and other 16-bit apps) OWNER-DRAW their title-bar caption -- custom
// buttons (e.g. a Help "?" on specific dialogs) and per-menu help text -- by
// calling GetWindowDC() on the top-level window and painting the non-client
// area after WM_NCACTIVATE/WM_NCPAINT. On Win7 (classic caption) that survives;
// on Win8+ the DWM/uxtheme themed-caption message WM_NCUAHDRAWCAPTION (0x00AE,
// with WM_NCUAHDRAWFRAME 0x00AF) repaints the themed caption OVER the app's
// drawing, wiping it. We mark a window as an owner-draw-caption window when the
// app takes a whole-window DC on it (WU32GetWindowDC), then, ONLY for those
// marked windows on Win8+, suppress the themed over-draw in the WOW window/dialog
// procs so the app's caption survives. Normal (non-owner-draw) windows keep the
// standard themed caption untouched.
//
// Diagnostic for the owner-draw-caption interception (marking + suppression
// conditions). Set 1 to re-arm.
#define WOW_NCDBG 0

#ifndef WM_NCUAHDRAWCAPTION
#define WM_NCUAHDRAWCAPTION  0x00AE   // undocumented: themed caption draw
#endif
#ifndef WM_NCUAHDRAWFRAME
#define WM_NCUAHDRAWFRAME    0x00AF   // undocumented: themed frame draw
#endif

// TRUE on Windows 8 or later (where the themed-caption over-draw happens; Win7
// reports 6.1 even manifest-capped, Win8+ >= 6.2). Cached; see wmdisp32.c.
BOOL W32IsWin8OrLater(void);

// Owner-draw-caption tracking, keyed by the 16-bit HWND (low word) so it is
// immune to full-vs-truncated 32-bit handle resolution: WU32GetWindowDC marks
// the window (parg16->f1), and the WOW window/dialog procs test GETHWND16(hwnd)
// -- both are the same 16-bit value. Cleared on WM_NCDESTROY. See wmdisp32.c.
void W32MarkNcOwnerDraw(WORD hwnd16);
BOOL W32IsNcOwnerDraw(WORD hwnd16);
void W32ClearNcOwnerDraw(WORD hwnd16);

// Win11 fallback cursors/icons built by xxxServerLoadCreateCursorIcon are PRIVATE
// (CreateIconFromResourceEx), so a 16-bit app's DestroyIcon frees them -- but on
// Win7 the equivalent module-loaded cursors/icons are SHARED and DestroyIcon
// leaves them alive, so apps that load once, cache the handle, and reuse it
// across dialogs (destroying it on each close) keep working (e.g. Lotus Approach's
// save-changes dialog icon vanishing on the 2nd invocation). Track the fallback
// handles so WU32DestroyIcon can no-op DestroyIcon on them (shared semantics).
void W32TrackSharedIcon(HICON hIcon);
BOOL W32IsSharedIcon(HICON hIcon);

// The real Win8+ blocker: DWM composites its own caption over whatever the
// 16-bit app paints into the window DC, so the owner-drawn caption never shows.
// Disable DWM non-client rendering (DWMWA_NCRENDERING_POLICY = DWMNCRP_DISABLED)
// on the window so it does its own NC drawing and the app's caption appears.
// Dynamic dwmapi.dll load; Win8+ only; no-op if unavailable. See wmdisp32.c.
void W32DisableDwmNcRendering(HWND hwnd);

//
// Same shape as UserRegisterWowHandlers() itself (apfnWowIn is unused,
// but kept so the two are trivially interchangeable at the call site):
// fills apfnWowOut with local replacements for the entries USER32.DLL
// would normally have supplied, and returns what should be stored in
// gpsi. Call this only after UserRegisterWowHandlers() has failed --
// see wow32.c and wu32compat.c for why it now always does on Windows 11
// 22H2+.
//
ULONG_PTR RegisterWowHandlersFallback(APFNWOWHANDLERSIN apfnWowIn, APFNWOWHANDLERSOUT apfnWowOut);

//
// Looks up the WC data xxxRegisterClassWOWA stashed for a class at
// registration time (hInstance, class name) -- in particular wc.vpszMenu
// (the class's registered menu name, still a raw VDM address/atom, not
// yet resolved) and wc.hMod16 (the class's owning 16-bit module). Used
// by wuwind.c to resolve a top-level window's class menu explicitly when
// hMenu is 0 at CreateWindow time. Returns NULL if the class was never
// registered through us (table full, or registered before this fallback
// engaged).
//
PWC WowFindTrackedClass(HINSTANCE hInstance, LPCTSTR pszClass);

//
// Companion to WowFindTrackedClass: the class's registered menu-name
// string as it existed at RegisterClass time, snapshotted then because
// wc.vpszMenu is only a raw VDM address -- 16-bit apps commonly reuse
// that same scratch buffer for something else by the time a window
// actually gets created, so re-reading it live at CreateWindow time (as
// an earlier version of this fix did) can return stale/unrelated bytes.
// Returns NULL if the class isn't tracked, has no menu, or its menu name
// is a MAKEINTRESOURCE-style integer id (use WowFindTrackedClass's
// wc.vpszMenu directly for that case -- no string, nothing to snapshot).
//
LPCSTR WowFindTrackedClassMenuName(HINSTANCE hInstance, LPCTSTR pszClass);

//
// Recover a leaked 16-bit window class on the Win11 fallback path: unregister
// every tracked registration of this class NAME under its original owning
// instance and return TRUE if any was freed, so RegisterClass can be retried.
// Called by WU32RegisterClass on ERROR_CLASS_ALREADY_EXISTS. See its body in
// wu32compat.c for why matching is by name, not instance.
//
BOOL WowRetryUnregisterClass(LPCSTR pszClass);

//
// Backing store for xxxGetFullUserHandle's FULLHWND32() replacement --
// records a window's real, full 32-bit HWND (the "uniqueness" high word
// included) against its 16-bit-truncated form, since we have no access to
// USER32's own private handle table to reconstruct it later otherwise.
// xxxCsCreateWindowEx/xxxServerCreateDialog call this for windows/dialogs
// created through our own fallback, but that alone misses dialog CHILD
// controls (a listbox, a button, ...) -- those come into existence inside
// the real CreateDialogIndirectParamA/DialogBoxIndirectParamAorW call
// itself, and modal dialogs never even route through our code at all (see
// xxxServerCreateDialog's own comment). WU32GetDlgItem (wusercli.c) is a
// far more reliable place to catch them: any control a 16-bit app ever
// asks about by ID goes through GetDlgItem, and the real, full HWND is
// sitting right there in hand before we truncate it for the 16-bit
// return value. Confirmed live (Write's "Insert Object" listbox):
// SendMessageTimeout (ThunkMsg16's FULLHWND32() call)
// failed with ERROR_INVALID_HANDLE against the zero-extended fallback
// value for exactly this reason.
//
VOID TrackFullHwnd(HWND hwnd32);

#endif /* W10 */
