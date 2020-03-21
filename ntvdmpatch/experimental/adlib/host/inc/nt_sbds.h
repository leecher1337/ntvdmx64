/****************************************************************************
 * DS.H                                                                     *
 * DirectSound support                                                      *
 *                                                                          *
 * Copyright (c)2000 Lawrence Rust                                          *
 * Created by Lawrence Rust, Software Systems Consultants, Cambridge, UK.   *
 * lvr@softsystem.co.uk. Tel/Fax +44 1223 862391                            *
 *__________________________________________________________________________*
 *                                                                          *
 * Revision History:                                                        *
 *                                                                          *
 * No.   Date     By   Reason                                               *
 *--------------------------------------------------------------------------*
 * 100 20 Mar 00  lvr  Created                                              *
 * 101 20 Feb 01  lvr  Added DIRECTSOUND_VERSION                            *
 *__________________________________________________________________________*/
#ifndef DS_H
#define DS_H 0x100

#ifndef DIRECTSOUND_VERSION
 #define DIRECTSOUND_VERSION  0x0100 /* version 1.0 */
#endif /* !DIRECTSOUND_VERSION */
#ifndef CINTERFACE
 #define CINTERFACE
#endif

/* Dependencies */
#ifndef REPORT_SB_MODE
#pragma warning ( disable : 4201) /* nonstandard extension used : nameless struct/union */
#pragma warning ( disable : 4214) /* nonstandard extension used : bit field types other than int */
#include <windows.h>
#include <mmsystem.h>
struct IRpcStubBuffer;
#include <dsound.h>
#pragma warning ( default : 4201)
#pragma warning ( default : 4214)

#ifndef DSBCAPS_CTRL3D
#define DSBCAPS_CTRL3D              0x00000010
#endif
#ifndef DSBCAPS_CTRLPOSITIONNOTIFY
#define DSBCAPS_CTRLPOSITIONNOTIFY  0x00000100
#endif
#ifndef DSBCAPS_GLOBALFOCUS
#define DSBCAPS_GLOBALFOCUS         0x00008000
#endif
#ifndef DSBCAPS_MUTE3DATMAXDISTANCE
#define DSBCAPS_MUTE3DATMAXDISTANCE 0x00020000
#endif


/* Buffer thresholds */
#ifndef OPT_BUFFER_LO_THRESHOLD
 #define OPT_BUFFER_LO_THRESHOLD 175    /* Lo threshold in ms.  Proportional to output interval */
#endif
#ifndef OPT_BUFFER_HI_THRESHOLD
 #define OPT_BUFFER_HI_THRESHOLD 250    /* Hi threshold in ms */
#endif
#endif

/* Types */
struct SSbvdd;                          /* Declared in sbvdd.h */
typedef struct SDs SDs;                 /* Opaque type */

/* Functions */
extern SDs* DirectSoundInit( void );
extern void DirectSoundFinal( register SDs*);
extern void DirectSoundTerminate( register SDs*);
extern void DirectSoundSuspend( register SDs*);
extern void DirectSoundResume( register SDs*);

#ifndef REPORT_SB_MODE
extern HRESULT DirectSoundOpen(
  register SDs*,                        /* IN: */
  WAVEFORMATEX*,                        /* IN: -> format or NULL */
  LPDIRECTSOUND*                        /* OUT: */
);
#endif

#endif /* ndef DS_H */
/* End of file */
