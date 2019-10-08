#ifndef __WINDDI_XP_H__
#define __WINDDI_XP_H__
#ifdef USERMODE_DRIVER

#define EngAllocMem(flags, cj, tag) ((PVOID) GlobalAlloc(((flags) & FL_ZERO_MEMORY) ? GPTR : GMEM_FIXED, cj))
#define EngFreeMem(p)               GlobalFree((HGLOBAL) (p))
#define EngAllocUserMem(cj, tag)    ((PVOID) GlobalAlloc(GMEM_FIXED,cj))
#define EngFreeUserMem(p)           GlobalFree((HGLOBAL) (p))
#define EngAllocPrivateUserMem( psl, cj, tag) ((PVOID) GlobalAlloc(GMEM_FIXED,cj))
#define EngFreePrivateUserMem( psl, p)        GlobalFree((HGLOBAL) (p))
#define EngMulDiv                   MulDiv

#endif
#endif
