/* Quickly hacked together totally vandalized version of SoundFX2000 FM emulator */

/****************************************************************************
 * DS.C                                                                     *
 * Application: Part of SBVDD.DLL                                           *
 * Purpose:     Common DirectSound interface                                *
 *                                                                          *
 * Copyright (c)2000 Lawrence Rust                                          *
 * Created by Lawrence Rust, Software Systems Consultants, Cambridge, UK.   *
 * lvr@softsystem.co.uk. Tel/Fax +44 1223 862391                            *
 *                                                                          *
 * Runtime:     Windows NT 4.0 user mode driver                             *
 *                                                                          *
 * Build:       Microsoft Visual C++ 4.2 and NT4.0 DDK                      *
 *              build.bat [checked|free]                                    *
 *__________________________________________________________________________*
 *                                                                          *
 * Revision History:                                                        *
 *                                                                          *
 * No.   Date     By   Reason                                               *
 *--------------------------------------------------------------------------*
 * 100 20 Mar 00  lvr Created                                               *
 * 101 23 Feb 01  lvr More debug output                                     *
 * 102 21 Mar 05  lvr #ifdef DSBCAPS_LOCDEFER
 *__________________________________________________________________________*/

/* Compilation options */
#if !defined OPT_COINIT 
 #define OPT_COINIT 0                   /* !0 to use CoCreateInstance, 0= DirectSoundCreate */
#endif
#if !defined OPT_SET_FORMAT
 #define OPT_SET_FORMAT 1               /* !0 to set the primary sound buffer wave format */
#endif
#ifndef OPT_CATCH_EXCEPTIONS
 #define OPT_CATCH_EXCEPTIONS 1
#endif

/* MSVC level 4 warnings */
#pragma warning ( disable : 4505) /* unreferenced local function has been removed */
#pragma warning ( disable : 4711) /* function selected for automatic inline expansion */


/* Exports */
#if OPT_COINIT
 #define INITGUID
#endif
#include "nt_sbds.h"

/* Imports */
#include <stdlib.h>

// Win32 SDK
#pragma warning ( disable : 4201) /* nonstandard extension used : nameless struct/union */
#pragma warning ( disable : 4214) /* nonstandard extension used : bit field types other than int */
#include <windows.h>
#include <mmsystem.h>
struct IRpcStubBuffer;
#include <dsound.h>
#pragma warning ( default : 4201)
#pragma warning ( default : 4214)

// NT DDK
#ifdef __cplusplus
extern "C" {
#endif
//#include <vddsvc.h>                     // VDD services
#ifdef __cplusplus
}
#endif


typedef HRESULT (WINAPI* DIRECTSOUNDCREATEPROC)(LPGUID, LPDIRECTSOUND*, IUnknown FAR *);
DIRECTSOUNDCREATEPROC DirectSoundCreateProc;
static HANDLE m_hDsound = NULL;


/***************************************************************************
 * Macro definitions
 ***************************************************************************/


/***************************************************************************
 * Data types
 ***************************************************************************/
struct SDs
  {
  //const SSbvdd* pInst;                  // -> emulator context

  LPDIRECTSOUND pDSI;
  LPDIRECTSOUNDBUFFER lpdsbPrimary;
  WAVEFORMATEX wfx;
  };


/***************************************************************************
 * Function prototypes
 ***************************************************************************/


/***************************************************************************
 * Module data
 ***************************************************************************/
static SDs s_ds;


/***************************************************************************
 * Exported functions
 ***************************************************************************/

/*
 * Initialisation
 * Single threaded
 */
SDs* DirectSoundInit( void)
  {
  SDs* pDs; 
  TRACE( 9, ("DirectSoundInit()\n"));

  pDs = &s_ds; 
  pDs->wfx.nChannels = 0;
  pDs->wfx.wBitsPerSample = 8;
  pDs->wfx.nSamplesPerSec = 4000;

#if OPT_COINIT
  {
  HRESULT hr = CoInitializeEx( NULL, COINIT_MULTITHREADED | COINIT_SPEED_OVER_MEMORY);
  if ( FAILED( hr))
    {
    TRACE( 1, ("*** DirectSoundInit() CoInitializeEx failed 0x%08lx\n", hr));
    return NULL;
    }
  }
#endif

  if ((m_hDsound = LoadLibrary("dsound.dll")))
  {
	  if (!(DirectSoundCreateProc = (DIRECTSOUNDCREATEPROC)GetProcAddress(m_hDsound, "DirectSoundCreate")))
		  return NULL;
  }

  return pDs;
  }


/*
 * Finalisation
 * Single threaded
 */
void DirectSoundFinal( register SDs* pDs)
  {
  TRACE( 9, ("DirectSoundFinal()\n"));

  //if ( NULL == pDs)
  //  pDs =&s_ds;
  (void)pDs;

  if (m_hDsound) FreeLibrary(m_hDsound);

#if OPT_COINIT
  CoUninitialize();
#endif
  }


/*
 * Process terminated
 * Release per process resources
 */
void DirectSoundTerminate( register SDs* pDs)
  {
  TRACE( 9, ("DirectSoundTerminate()\n"));
  ASSERT( NULL != pDs);

  if ( NULL != pDs->lpdsbPrimary)
    {
#if OPT_CATCH_EXCEPTIONS
    { unsigned long ulExcept = 0; __try {
#endif

    TRACE( 3, ("DirectSoundTerminate() Releasing primary buffer...\n"));
    IDirectSoundBuffer_Release( pDs->lpdsbPrimary);
    TRACE( 2, ("DirectSoundTerminate() Primary buffer released\n"));

#if OPT_CATCH_EXCEPTIONS
    } __except( ulExcept = GetExceptionCode(), EXCEPTION_EXECUTE_HANDLER) {
      TRACE( -1, ("!!! DirectSoundTerminate() IDirectSoundBuffer_Release exception 0x%lx\n", ulExcept));
    } }
#endif
    pDs->lpdsbPrimary = NULL;
    }

  if ( NULL != pDs->pDSI)
    {
#if OPT_CATCH_EXCEPTIONS
    { unsigned long ulExcept = 0; __try {
#endif

    TRACE( 3, ("DirectSoundTerminate() Unloading DirectSound...\n"));
    IDirectSound_Release( pDs->pDSI);
    TRACE( 2, ("DirectSoundTerminate() DirectSound released\n"));

#if OPT_CATCH_EXCEPTIONS
    } __except( ulExcept = GetExceptionCode(), EXCEPTION_EXECUTE_HANDLER) {
      TRACE( -1, ("!!! DirectSoundTerminate() IDirectSound_Release exception 0x%lx\n", ulExcept));
    } }
#endif
    pDs->pDSI = NULL;
    }
  }


/*
 * Process suspended
 */
extern void DirectSoundSuspend( register SDs* pDs)
  {
  TRACE( 2, ("DirectSoundSuspend()\n"));
  ASSERT( NULL != pDs);

#if OPT_SET_FORMAT
#if OPT_CATCH_EXCEPTIONS
  { unsigned long ulExcept = 0; __try {
#endif

  if ( NULL != pDs->lpdsbPrimary)
    IDirectSoundBuffer_Stop( pDs->lpdsbPrimary);

#if OPT_CATCH_EXCEPTIONS
  } __except( ulExcept = GetExceptionCode(), EXCEPTION_EXECUTE_HANDLER) {
    TRACE( -1, ("!!! DirectSoundSuspend() IDirectSoundBuffer_Stop exception 0x%lx\n", ulExcept));
  } }
#endif
#else
  (void)pDs;
#endif
  }


/*
 * Process resumed
 */
extern void DirectSoundResume( register SDs* pDs)
  {
  TRACE( 2, ("DirectSoundResume()\n"));
  ASSERT( NULL != pDs);

#if OPT_SET_FORMAT
  // Set the primary buffer to play continuously
  if ( NULL != pDs->lpdsbPrimary)
    {
#if OPT_CATCH_EXCEPTIONS
    { unsigned long ulExcept = 0; __try {
#endif

    HRESULT hr = IDirectSoundBuffer_Play( pDs->lpdsbPrimary, 0, 0, DSBPLAY_LOOPING);
    if ( FAILED( hr))
      TRACE( 1, ("*** DirectSoundResume() IDirectSoundBuffer_Play failed 0x%08lx\n", hr));

#if OPT_CATCH_EXCEPTIONS
    } __except( ulExcept = GetExceptionCode(), EXCEPTION_EXECUTE_HANDLER) {
      TRACE( -1, ("!!! DirectSoundResume() IDirectSoundBuffer_Play exception 0x%lx\n", ulExcept));
    } }
#endif
    }
#else
  (void)pDs;
#endif
  }


/****************************************************************************
 * DirectSound support                                                      *
 ****************************************************************************/
HRESULT DirectSoundOpen(
  register SDs* pDs,
  WAVEFORMATEX* pWfx,       // IN:
  LPDIRECTSOUND* ppDS       // OUT:
) {
  int iTries;
  HRESULT hr = S_OK;

  TRACE( 9, ("DirectSoundOpen(%p)\n", pWfx));
  ASSERT( NULL != pDs);
  ASSERT( NULL != ppDS);

  for ( iTries = 0; NULL == pDs->pDSI; ++iTries)
    {
#if OPT_COINIT
    hr = CoCreateInstance(
      &CLSID_DirectSound,
      NULL,
      CLSCTX_INPROC_SERVER,
      &IID_IDirectSound,
      &pDs->pDSI
    );
    if ( FAILED( hr))
      {
      TRACE( 0, ("!!! DirectSoundOpen() CoCreateInstance failed 0x%08lx\n", hr));
      return hr;
      }

    hr = IDirectSound_Initialize( pDs->pDSI, NULL);
#else
    // Initialise DirectSound
    hr = DirectSoundCreateProc( NULL, &pDs->pDSI, NULL);
#endif
    if ( SUCCEEDED( hr))
      {
      HWND hWnd;

      ASSERT( NULL != pDs->pDSI);
      TRACE( 2, ("DirectSoundOpen() DirectSound initialised\n"));

#if DBG
      {
      DSCAPS caps;
      
      caps.dwSize = sizeof( caps);
      ASSERT( SUCCEEDED( IDirectSound_GetCaps( pDs->pDSI, &caps) ));
      if ( caps.dwFlags & DSCAPS_PRIMARYMONO)
        TRACE( 2, ("DirectSound primary buffer supports mono\n"));
      if ( caps.dwFlags & DSCAPS_PRIMARYSTEREO)
        TRACE( 2, ("DirectSound primary buffer supports stereo\n"));
      if ( caps.dwFlags & DSCAPS_PRIMARY8BIT)
        TRACE( 2, ("DirectSound primary buffer supports 8 bit\n"));
      if ( caps.dwFlags & DSCAPS_PRIMARY16BIT)
        TRACE( 2, ("DirectSound primary buffer supports 16 bit\n"));
      if ( caps.dwFlags & DSCAPS_CONTINUOUSRATE)
        TRACE( 2, ("DirectSound primary buffer supports continuous rate\n"));
      if ( caps.dwFlags & DSCAPS_EMULDRIVER)
        TRACE( 2, ("DirectSound is emulated\n"));
      TRACE( 2, ("DirectSound driver is%s certified\n", (caps.dwFlags & DSCAPS_CERTIFIED) ? "" : " not"));
      if ( caps.dwFlags & DSCAPS_SECONDARYMONO)
        TRACE( 2, ("DirectSound secondary buffer supports mono\n"));
      if ( caps.dwFlags & DSCAPS_SECONDARYSTEREO)
        TRACE( 2, ("DirectSound secondary buffer supports stereo\n"));
      if ( caps.dwFlags & DSCAPS_SECONDARY8BIT)
        TRACE( 2, ("DirectSound secondary buffer supports 8 bit\n"));
      if ( caps.dwFlags & DSCAPS_SECONDARY16BIT)
        TRACE( 2, ("DirectSound secondary buffer supports 16 bit\n"));

      TRACE( 2, ("DirectSound dwMinSecondarySampleRate = %lu\n", caps.dwMinSecondarySampleRate));
      TRACE( 2, ("DirectSound dwMaxSecondarySampleRate = %lu\n", caps.dwMaxSecondarySampleRate));
      TRACE( 2, ("DirectSound dwPrimaryBuffers = %lu\n", caps.dwPrimaryBuffers));
      TRACE( 2, ("DirectSound dwMaxHwMixingAllBuffers = %lu\n", caps.dwMaxHwMixingAllBuffers));
      TRACE( 2, ("DirectSound dwMaxHwMixingStaticBuffers = %lu\n", caps.dwMaxHwMixingStaticBuffers));
      TRACE( 2, ("DirectSound dwMaxHwMixingStreamingBuffers = %lu\n", caps.dwMaxHwMixingStreamingBuffers));
      TRACE( 2, ("DirectSound dwFreeHwMixingAllBuffers = %lu\n", caps.dwFreeHwMixingAllBuffers));
      TRACE( 2, ("DirectSound dwFreeHwMixingStaticBuffers = %lu\n", caps.dwFreeHwMixingStaticBuffers));
      TRACE( 2, ("DirectSound dwFreeHwMixingStreamingBuffers = %lu\n", caps.dwFreeHwMixingStreamingBuffers));
      TRACE( 2, ("DirectSound dwMaxHw3DAllBuffers = %lu\n", caps.dwMaxHw3DAllBuffers));
      TRACE( 2, ("DirectSound dwMaxHw3DStaticBuffers = %lu\n", caps.dwMaxHw3DStaticBuffers));
      TRACE( 2, ("DirectSound dwMaxHw3DStreamingBuffers = %lu\n", caps.dwMaxHw3DStreamingBuffers));
      TRACE( 2, ("DirectSound dwFreeHw3DAllBuffers = %lu\n", caps.dwFreeHw3DAllBuffers));
      TRACE( 2, ("DirectSound dwFreeHw3DStaticBuffers = %lu\n", caps.dwFreeHw3DStaticBuffers));
      TRACE( 2, ("DirectSound dwFreeHw3DStreamingBuffers = %lu\n", caps.dwFreeHw3DStreamingBuffers));
      TRACE( 2, ("DirectSound dwTotalHwMemBytes = %lu\n", caps.dwTotalHwMemBytes));
      TRACE( 2, ("DirectSound dwFreeHwMemBytes = %lu\n", caps.dwFreeHwMemBytes));
      TRACE( 2, ("DirectSound dwMaxContigFreeHwMemBytes = %lu\n", caps.dwMaxContigFreeHwMemBytes));
      TRACE( 2, ("DirectSound dwUnlockTransferRateHwBuffers = %lu\n", caps.dwUnlockTransferRateHwBuffers));
      TRACE( 2, ("DirectSound dwPlayCpuOverheadSwBuffers = %lu\n", caps.dwPlayCpuOverheadSwBuffers));
      }
#endif

      // Set cooperative level
      hWnd = GetForegroundWindow();
      if ( NULL == hWnd)
        hWnd = GetDesktopWindow();

#if OPT_SET_FORMAT
 #define COOP_LEVEL DSSCL_EXCLUSIVE //DSSCL_PRIORITY  // Needed to call SetFormat
#else
 #define COOP_LEVEL DSSCL_NORMAL
#endif
      hr = IDirectSound_SetCooperativeLevel( pDs->pDSI, hWnd, COOP_LEVEL);
      if ( FAILED( hr))
        {
        TRACE( 1, ("*** DirectSoundOpen() IDirectSound_SetCooperativeLevel failed 0x%08lx\n", hr));
        return hr;
        }

      break;
      }
    else if ( DSERR_ALLOCATED != hr || iTries >= 15)
      {
      TRACE( 0, ("!!! DirectSoundOpen() DirectSoundCreate failed 0x%08lx\n", hr));
      return E_FAIL;
      }
  
    // device is busy - wait a while and try again
    TRACE( 3, ("DirectSoundOpen() DirectSound is busy.  Awaiting retry timeout\n"));
    Sleep( 300);
    }

  ASSERT( SUCCEEDED( hr));
  ASSERT( NULL != pDs->pDSI);
  *ppDS = pDs->pDSI;

#if OPT_SET_FORMAT
  if ( NULL != pWfx
    && (pWfx->nSamplesPerSec > pDs->wfx.nSamplesPerSec
      || pWfx->wBitsPerSample > pDs->wfx.wBitsPerSample
      || pWfx->nChannels > pDs->wfx.nChannels
    )
  ) {
#if OPT_CATCH_EXCEPTIONS
    { unsigned long ulExcept = 0; __try {
#endif
    WAVEFORMATEX wfx;
    DWORD dwStatus = 0;

    TRACE( 3, ("DirectSoundOpen() Update format to: %d sps, %d bits, %d channels\n",
      pWfx->nSamplesPerSec, pWfx->wBitsPerSample, pWfx->nChannels));

    if ( NULL == pDs->lpdsbPrimary)
      {
      // Obtain the primary buffer
      DSBUFFERDESC dsbdesc;
      RtlZeroMemory( &dsbdesc, sizeof( dsbdesc) );
      dsbdesc.dwSize = sizeof( dsbdesc);
      dsbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
      hr = IDirectSound_CreateSoundBuffer(
        pDs->pDSI,
        &dsbdesc,
        &pDs->lpdsbPrimary,
        NULL
      );
      if ( SUCCEEDED( hr))
        {
        TRACE( 3, ("DirectSoundOpen() Obtained primary buffer\n"));
        ASSERT( NULL != pDs->lpdsbPrimary);
        }
      else
        {
        TRACE( 1, ("*** DirectSoundOpen() IDirectSound_CreateSoundBuffer (primary) failed 0x%08lx\n", hr));
        ASSERT( NULL == pDs->lpdsbPrimary);
        }
#if DBG
      {
      DSBCAPS caps;
      
      caps.dwSize = sizeof( caps);
      ASSERT( SUCCEEDED( IDirectSoundBuffer_GetCaps( pDs->lpdsbPrimary, &caps) ));
      if ( caps.dwFlags & DSBCAPS_PRIMARYBUFFER)
        TRACE( 2, ("DirectSound primary buffer\n"));
      if ( caps.dwFlags & DSBCAPS_STATIC)
        TRACE( 2, ("DirectSound primary buffer is static\n"));
      if ( caps.dwFlags & DSBCAPS_LOCHARDWARE)
        TRACE( 2, ("DirectSound primary buffer hardware mixing\n"));
      if ( caps.dwFlags & DSBCAPS_LOCSOFTWARE)
        TRACE( 2, ("DirectSound primary buffer software mixing\n"));
      if ( caps.dwFlags & DSBCAPS_CTRL3D)
        TRACE( 2, ("DirectSound primary buffer can do 3D\n"));
      if ( caps.dwFlags & DSBCAPS_CTRLFREQUENCY)
        TRACE( 2, ("DirectSound primary buffer has frequency control\n"));
      if ( caps.dwFlags & DSBCAPS_CTRLPAN)
        TRACE( 2, ("DirectSound primary buffer has pan control\n"));
      if ( caps.dwFlags & DSBCAPS_CTRLVOLUME)
        TRACE( 2, ("DirectSound primary buffer has volume control\n"));
      if ( caps.dwFlags & DSBCAPS_CTRLPOSITIONNOTIFY)
        TRACE( 2, ("DirectSound primary buffer has position notify\n"));
      if ( caps.dwFlags & DSBCAPS_STICKYFOCUS)
        TRACE( 2, ("DirectSound primary buffer has sticky focus\n"));
      if ( caps.dwFlags & DSBCAPS_GLOBALFOCUS)
        TRACE( 2, ("DirectSound primary buffer has global focus\n"));
      if ( caps.dwFlags & DSBCAPS_GETCURRENTPOSITION2)
        TRACE( 2, ("DirectSound primary buffer has new play cursor\n"));
      if ( caps.dwFlags & DSBCAPS_MUTE3DATMAXDISTANCE)
        TRACE( 2, ("DirectSound primary buffer mutes at max distance\n"));
#ifdef DSBCAPS_LOCDEFER
      if ( caps.dwFlags & DSBCAPS_LOCDEFER)
        TRACE( 2, ("DirectSound primary buffer can be h/w or s/w at play time\n"));
#endif

      TRACE( 2, ("DirectSound primary buffer dwBufferBytes = %lu\n", caps.dwBufferBytes));
      TRACE( 2, ("DirectSound primary buffer dwUnlockTransferRate = %lu\n", caps.dwUnlockTransferRate));
      TRACE( 2, ("DirectSound primary buffer dwPlayCpuOverhead = %lu\n", caps.dwPlayCpuOverhead));
      }
#endif
      }

    if ( NULL != pDs->lpdsbPrimary)
      {
      // Find superset of current & desired formats
      wfx = *pWfx;
      if ( pDs->wfx.nSamplesPerSec > wfx.nSamplesPerSec)
        wfx.nSamplesPerSec = pDs->wfx.nSamplesPerSec;
      if ( pDs->wfx.wBitsPerSample > wfx.wBitsPerSample)
        wfx.wBitsPerSample = pDs->wfx.wBitsPerSample;
      if ( pDs->wfx.nChannels > wfx.nChannels)
        wfx.nChannels = pDs->wfx.nChannels;

      wfx.nBlockAlign = (WORD)(((wfx.wBitsPerSample + 7) / 8) * wfx.nChannels);
      wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

      // Primary buffer must be stopped to change format
      hr = IDirectSoundBuffer_GetStatus( pDs->lpdsbPrimary, &dwStatus);
      if ( FAILED( hr))
        TRACE( 1, ("*** DirectSoundOpen() IDirectSoundBuffer_GetStatus failed 0x%08lx\n", hr));
      else if ( DSBSTATUS_PLAYING & dwStatus)
        {
        hr = IDirectSoundBuffer_Stop( pDs->lpdsbPrimary);
        if ( FAILED( hr))
          TRACE( 1, ("*** DirectSoundOpen() IDirectSoundBuffer_Stop failed 0x%08lx\n", hr));
        }

      // Set the primary output format
      hr = IDirectSoundBuffer_SetFormat( pDs->lpdsbPrimary, &wfx);
      if ( SUCCEEDED( hr))
        {
        TRACE( 2, ("DirectSoundOpen() Primary output format set to: %d sps, %d bits, %d channels\n",
          wfx.nSamplesPerSec, wfx.wBitsPerSample, wfx.nChannels));
        pDs->wfx = wfx;
        }
      else
        {
        TRACE( 1, ("*** DirectSoundOpen() IDirectSound_SetFormat %d sps, %d bits, %d channels failed 0x%08lx\n",
          wfx.nSamplesPerSec, wfx.wBitsPerSample, wfx.nChannels, hr));
        }

      // Set the primary buffer to play continuously
      hr = IDirectSoundBuffer_Play( pDs->lpdsbPrimary, 0, 0, DSBPLAY_LOOPING);
      if ( FAILED( hr))
        TRACE( 1, ("*** DirectSoundOpen() IDirectSoundBuffer_Play failed 0x%08lx\n", hr));
      }

#if OPT_CATCH_EXCEPTIONS
    } __except( ulExcept = GetExceptionCode(), EXCEPTION_EXECUTE_HANDLER) {
      TRACE( -1, ("!!! DirectSoundOpen() exception 0x%lx\n", ulExcept));
    } }
#endif
    }
#else
  (void)pWfx;
#endif

  return S_OK;
  }

/* End of file */
