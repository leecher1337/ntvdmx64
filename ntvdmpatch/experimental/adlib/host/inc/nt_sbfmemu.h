/****************************************************************************
 * FM.H                                                                     *
 * Interface to the OPL2/3 FM emulator                                      *
 *                                                                          *
 * Copyright (c)1998 Lawrence Rust                                          *
 * Created by Lawrence Rust, Software Systems Consultants, Cambridge, UK.   *
 * lvr@softsystem.co.uk. Tel/Fax +44 1223 862391                            *
 *__________________________________________________________________________*
 *                                                                          *
 * Revision History:                                                        *
 *                                                                          *
 * No.   Date     By   Reason                                               *
 *--------------------------------------------------------------------------*
 * 100 02 Oct 98  lvr  Created                                              *
 * 101 21 Feb 00  lvr  Added FmSetEmulationMode                             *
 * 102 21 Mar 00  lvr  Added FmSuspend/Resume                               *
 *__________________________________________________________________________*/
#ifndef FM_H
#define FM_H 0x102

#include "nt_sbds.h"

/* Dependencies */
#ifndef REPORT_SB_MODE
#pragma warning ( disable : 4201) /* nonstandard extension used : nameless struct/union */
#pragma warning ( disable : 4214) /* nonstandard extension used : bit field types other than int */
#include <windows.h>
#pragma warning ( default : 4201)
#pragma warning ( default : 4214)
#endif

/* Types */
struct SSbvdd;
typedef struct SFm SFm;

/* Functions */
extern SFm* FmInit( SDs*);
extern void FmFinal( register SFm*);
extern void FmTerminate( register SFm*);

extern BYTE __fastcall FmStatus( register SFm*);
extern void __fastcall FmAddress( register SFm*, WORD);
extern void __fastcall FmWrite( register SFm*, BOOL bOPL3, BYTE);

extern void FmSetCacheTimeout( register SFm*, unsigned);
extern BOOL FmSetVolume(
  register SFm*, 
  BYTE ubLeftVolume,
  BYTE ubRightVolume
);
extern void FmSetEmulationMode( register SFm*, WORD);

extern void FmSuspend( register SFm*);
extern void FmResume( register SFm*);

#endif /* ndef FM_H */
/* End of file */
