/* Quickly hacked together totally vandalized version of SoundFX2000 FM emulator */

#define OPT_ADLIB_EMULATOR 1
#define COMPILATION_REQUIRES(e) extern char _dummy[(e) ? 1 : -1] 
#define TRACE(x,y)
#define ASSERT(x) 

/****************************************************************************
 * FM.C                                                                     *
 * Application: Part of SBVDD.DLL                                           *
 * Purpose:     Sound blaster OPL2/3 FM emulation                           *
 *                                                                          *
 * Copyright (c)1998 Lawrence Rust                                          *
 * Created by Lawrence Rust, Software Systems Consultants, Cambridge, UK.   *
 * lvr@softsystem.co.uk. Tel/Fax +44 1223 862391                            *
 *                                                                          *
 * Runtime:     Windows NT 4.0 user mode driver                             *
 *__________________________________________________________________________*
 *                                                                          *
 * Revision History:                                                        *
 *                                                                          *
 * No.   Date     By   Reason                                               *
 *--------------------------------------------------------------------------*
 * 100 02 Oct 98  lvr  Created                                              *
 * 101 27 Oct 98  lvr  Add caching and timer emulation                      *
 * 102 03 Nov 98  lvr  Add OPT_WARN_CANT_OPEN                               *
 * 103 11 May 99  lvr  Force OPL2 mode in FmTerminate                       *
 * 104 11 Jun 99  lvr  Use portio device to read/write FM h/w               *
 * 105 07 Feb 00  lvr  Better OPL3 reset                                    *
 *     09 Feb 00  lvr  Started Adlib emulator                               *
 * 200 28 Mar 00  lvr  V 2.0 release                                        *
 * 201 01 Apr 00  lvr  Added max/min buffer threholds in registry           *
 *     15 Sep 00  lvr  VC6/SDK July00 compatibiliyy                         *
 * 202 23 Feb 01  lvr  Trap DirectSound exceptions                          *
 *                     Claim the synth critical section in EmulatorStop     *
 * 204 19 Jul 03  lvr  Increased envelope eventQ to 512 (was 32)            *
 *__________________________________________________________________________*/
#define VERSION 204

/* Compilation options */
//#ifndef OPT_WARN_CANT_OPEN
// #define OPT_WARN_CANT_OPEN 2           /* !0 to warn user device open failed */
//#endif
#ifndef OPT_WRITE_CACHE
 #define OPT_WRITE_CACHE 64             /* No. FM reg. writes cached */
#endif
#if OPT_WRITE_CACHE && !defined OPT_CACHE_TIMEOUT
 #define OPT_CACHE_TIMEOUT 16           /* Cache write back timeout ms */
#endif

#ifndef OPT_EMULATE_STATUS
 #define OPT_EMULATE_STATUS 1           /* !0 to emulate status register */
#endif

//#undef OPT_ADLIB_EMULATOR
#if !defined OPT_ADLIB_EMULATOR && OPT_EMULATE_STATUS
 #define OPT_ADLIB_EMULATOR 0           /* !0 to enable Ablib emulator */
#endif

#ifndef OPT_CATCH_EXCEPTIONS
 #define OPT_CATCH_EXCEPTIONS 1
#endif

#if OPT_ADLIB_EMULATOR && !defined OPT_ENV_EVENTS
#define OPT_ENV_EVENTS 512              /* No. envelope events buffered */
#endif


/* MSVC level 4 warnings */
#pragma warning ( disable : 4505) /* unreferenced local function has been removed */
#pragma warning ( disable : 4711)/* function selected for automatic inline expansion */
#pragma warning ( disable : 4201) /* nonstandard extension used : nameless struct/union */
#pragma warning ( disable : 4214) /* nonstandard extension used : bit field types other than int */


/* Exports */
#include "nt_sbfmemu.h"

/* Imports */
#include <limits.h>
#include <stdlib.h> // rand & RAND_MAX

// Win32 SDK
#include <windows.h>
#pragma warning ( disable : 4201) /* nonstandard extension used : nameless struct/union */
#pragma warning ( disable : 4214) /* nonstandard extension used : bit field types other than int */
#include "nt_sb.h"                   // timeSetEvent

//#pragma warning ( default : 4201)
//#pragma warning ( default : 4214)

// Application
#if OPT_ADLIB_EMULATOR
#include "nt_sbds.h"
#endif

/***************************************************************************
 * Macro definitions
 ***************************************************************************/

#ifndef lengthof
#define lengthof( a) (sizeof(a)/sizeof(a[0]))
#endif

// OPL2/3 registers
enum
  {
  kRegTest      = 0x01,
  kRegTimer1    = 0x02,
  kRegTimer2    = 0x03,
  kRegStatus    = 0x04,
  kRegNts       = 0x08,
  kRegMult      = 0x20, // 18 slots
  kRegLevel     = 0x40, // 18 slots
  kRegAttack    = 0x60, // 18 slots
  kRegSustain   = 0x80, // 18 slots
  kRegFnum      = 0xa0, // 9 channels
  kRegBlock     = 0xb0, // 9 channels
  kRegRhythm    = 0xbd,
  kRegFb        = 0xc0, // 9 channels
  kRegWs        = 0xe0, // 9 channels
  // OPL3...
  kRegConnSel   = 0x104,
  kRegNew       = 0x105
  };

#if OPT_ADLIB_EMULATOR
// Buffer format
#define kBitsPerChannel 8U              // Output format
#define kChannels 2U
//#define OPT_FIXED_SAMPLE_RATE 8192U     // Power of 2 for efficient arithmetic

enum
  {
  kAdlibFreq = (unsigned)(14.32e6/288), // The Adlib's internal sample rate 

  // Buffer capacity & thresholds
  kBufferMaxMs          = 750,
  kBytesPerSample       = kChannels * ((kBitsPerChannel + 7) / 8),
#if OPT_FIXED_SAMPLE_RATE
  kOutputBufferSize     = (kBufferMaxMs * kBytesPerSample * OPT_FIXED_SAMPLE_RATE) / 1000,
#else
  kDefaultSampleRate    = 8192,
#endif

  kSilence              = kBitsPerChannel <= 8 ? 0x80 : 0x0000,
  kWaveMax              = 0x7fff,

  kWaveElements         = 1024,         // No. entries in wave lookup table
  kAdsrStep             = 16,           // No. sample clocks between envelope updates
  kLfoStep              = 64            // No. sample clocks between LFO updates
  };

#if OPT_FIXED_SAMPLE_RATE
 #define SAMPLE_RATE(p) OPT_FIXED_SAMPLE_RATE
#else
 #define SAMPLE_RATE(p) ((p)->nSamplesPerSec)
#endif

#define PHASE_MAX(p) ((unsigned long)(kWaveElements * SAMPLE_RATE(p)))

// log2(n) in 4.12 fixed point for envelope generator attenuation
enum
  {
  kMaxdB    = 0xffff,   // = 96dB - 1bit
  k93dB     = 0xf800,   // = 93dB, max sustain attenuation
  k6dB      = 0x1000,   // = 6dB (2.0)
  k3dB      = k6dB / 2, // = 3dB
  k1dB      = k3dB / 3, // = 1dB
  k4dB8     = (unsigned)(4.8 * k1dB)
  };

// ADSR time constants in secs for 6dB (50%) change at minimum rate
// These were determined by 'scope measurements on an AWE32
enum
  {
  kAttackTime   = 1600, // ms
  kDecayTime    = 3200, // ms
  kReleaseTime  = 3200 // ms
  };

#endif // OPT_ADLIB_EMULATOR


/***************************************************************************
 * Data types
 ***************************************************************************/

// synth.h
 typedef struct {
     unsigned short IoPort;
     unsigned short PortData;
 } SYNTH_DATA, *PSYNTH_DATA;

// Adlib device access mode
typedef enum EAdlibMode
  {
  kModeIndirect,                        // Use adlib.mid device
  kModeDirect                           // Use direct portio driver
#if OPT_ADLIB_EMULATOR
  ,kModeEmulate                         // S/w emulation
#endif
  } EAdlibMode;

typedef union USynthData
  {
  SYNTH_DATA adlib;
  } USynthData;
COMPILATION_REQUIRES( sizeof( USynthData) == sizeof( SYNTH_DATA));


#if OPT_ADLIB_EMULATOR
/* ADSR phase */
typedef enum EAdsrState
  {
  kAdsrRelease = 0,
  kAdsrAttack,
  kAdsrDecay,
  kAdsrSustain
  } EAdsrState;

/* ADSR event */
typedef struct SAdsrEvent
  {
  DWORD dwTicksRemaining;
  BOOL bKon;
  } SAdsrEvent;

/* Operator slot state */
typedef struct SOperator
  {
  unsigned uChannel;                    // Controlling channel
  long lModulation;                     // Phase modulation
  // Rhythm mode
  BOOL bNoise;                          // White noise enable
  BOOL bBeat;                           // Rhythm percussive attack phase
  // Envelope generation
  EAdsrState eAdsr;                     // ADSR state
  long lAdsrCount;                      // ADSR cycle counter
  int iAttenuation;                     // log2( ADSR attenuation), 4.12 fixed point
  BOOL bKon;                            // Most recent KON event
  unsigned uNextEvent;                  // Head of queue envelope event queue
  SAdsrEvent eventArray[ OPT_ENV_EVENTS]; // Envelope event queue
  } SOperator;

/* Channel state */
typedef struct SChannel
  {
  short nType;                          // No. operators, 4=4op, 2=2op, 1=rhythm
  unsigned long ulPhase;                // Oscillator phase counter
  unsigned long ulPhaseIncr;            // Oscillator phase increment
  unsigned long ulVibratoPhase;         // Oscillator w/vibrato phase counter
  unsigned long ulVibratoIncr;          // Oscillator phase increment
  int iKslAttenuation;                  // log2( ADSR attenuation), 4.12 fixed point
  } SChannel;
#endif // OPT_ADLIB_EMULATOR

/* FM synthesizer context */
struct SFm
  {
  SDs* pDs;                  // -> directsound handle

  EAdlibMode eMode;                     // Device access mode
  HANDLE hFm;                           // Device driver handle

  WORD wAddr;                           // Synth address register
  BYTE ubControlReg;                    // Control register cache
  BYTE ubStatus;                        // Status register cache

#if OPT_EMULATE_STATUS
  unsigned uTimer1, uTimer2;            // Timer1/2 timeouts
#else
  int iStatusValid;                     // Status register cache valid
#endif

#if OPT_WRITE_CACHE
  CRITICAL_SECTION csCache;             // Mutex for cache access
  unsigned uTimeout;                    // Cache write back timeout in ms
  MMRESULT volatile hTimer;
  unsigned volatile uCacheIndex;
  USynthData aSynth[ 2 * OPT_WRITE_CACHE]; // Write back register cache
#endif

#if OPT_ADLIB_EMULATOR
 #if !OPT_FIXED_SAMPLE_RATE
  WORD nSamplesPerSec;
 #endif
  LPDIRECTSOUNDBUFFER pdsBuffer;        // DirectSound o/p buffer
  DWORD dwBufferSize;                   // O/p buffer size
  DWORD dwBufferLoMs, dwBufferHiMs;     // O/p buffer lo/hi threholds in ms
  DWORD dwBufferLoWater,dwBufferHiWater;
  DWORD dwWritten;                      // O/p buffer write position
  HANDLE hOutputThread;                 // Output thread handle
  HANDLE hSynthEvent;                   // Auto-reset event to signal output thread
  MMRESULT hEmulationTimer;             // Repetive timer to signal synthEvent

  CRITICAL_SECTION csSynth;             // Mutex for synth data access
  DWORD dwTicksPerInterval;             // Emulator clock ticks per timer tick
  unsigned long ulTicks;                // Emulator clock tick count
  DWORD dwLastUpdate;                   // Time of last update

  unsigned long ulVibratoPhase;         // Vibrato LFO phase
  int iVibrato;
  unsigned long ulTremoloPhase;         // Tremolo LFO phase
  int iTremoloAttenuation;

  SChannel chnl[ 18];                   // Channel settings
  SOperator op[ 36];                    // Operator slot settings
  BYTE aubRegister[ 512];               // Synth register cache
#endif
  };


/***************************************************************************
 * Function prototypes
 ***************************************************************************/

static BOOL FmOpen( register SFm* pFm);
static void FmReset( register SFm* const);
#if OPT_WRITE_CACHE
static void CALLBACK WriteBackTimer( UINT, UINT, DWORD, DWORD, DWORD);
static void FlushCache( register SFm* pFm);
#endif
static BOOL FmDeviceWrite(
  register SFm* const pFm,
  const USynthData* pSynthData,
  DWORD dwCount
);
#if !OPT_EMULATE_STATUS
static BOOL FmDeviceStatus(
  register SFm* const pFm,
  BYTE* pubStatus
);
#endif

#if OPT_ADLIB_EMULATOR
// FM emulator
static void EmulatorStop( register SFm* const);
static BOOL EmulatorStart( register SFm* const);

static DWORD WINAPI EmulatorThread( LPVOID);
static DWORD __fastcall EmulatorOutput( register SFm* const, DWORD dwTicks);

static void __fastcall EmulatorEndUpdate( register SFm* const);
static void __fastcall EmulatorWrite( register SFm* const, unsigned uIndex, BYTE const ubData);
static void __fastcall EmulatorClockTick( register SFm* const, unsigned char aucOutputBuffer[]);

static void __fastcall EmulatorEnvelope( register SFm* const);
static void __fastcall EmulatorKonEvent( register SFm* const, unsigned uSlot, BOOL bKon);
#endif


/***************************************************************************
 * Module data
 ***************************************************************************/
static SFm s_fm;

#if OPT_ADLIB_EMULATOR
// Sine wave lookup table
extern const signed short s_knWaveArray1[ kWaveElements];

// Lookup table for 'Total Level' power to output level multiplier
enum { kTwoScale = 0x8000 };
extern const unsigned short s_knPowerOf2Array[ 4096];
COMPILATION_REQUIRES( k6dB == lengthof( s_knPowerOf2Array));

// 4 operator channel usage
static const struct SConnection
  {
  unsigned char chnl1;
  unsigned char chnl2;
  }
s_ConnectionArray[] = {
  {  0,  3 },
  {  1,  4 },
  {  2,  5 },
  {  9, 12 },
  { 10, 13 },
  { 11, 14 }
};
COMPILATION_REQUIRES( lengthof( s_ConnectionArray) == 6 );

// 2operator channel to slot lookup table
static const struct S2Op
  {
  unsigned slot[ 2];
  }
s_2OpArray[] = {
  {  0,  3 },
  {  1,  4 },
  {  2,  5 },
  {  6,  9 },
  {  7, 10 },
  {  8, 11 },
  { 12, 15 },
  { 13, 16 },
  { 14, 17 },
  // OPL3 operators...
  { 18, 21 },
  { 19, 22 },
  { 20, 23 },
  { 24, 27 },
  { 25, 28 },
  { 26, 29 },
  { 30, 33 },
  { 31, 34 },
  { 32, 35 }
  };
COMPILATION_REQUIRES( 18 == lengthof( s_2OpArray) );

// 4operator channel no. to slot lookup table
static const struct S4Op
  {
  unsigned slot[ 4];
  }
s_4OpArray[] = {
  {  0,  3,  6,  9 },
  {  1,  4,  7, 10 },
  {  2,  5,  8, 11 },
  { 18, 21, 24, 27 },
  { 19, 22, 25, 28 },
  { 20, 23, 26, 29 }
  };
COMPILATION_REQUIRES( lengthof( s_4OpArray) == lengthof( s_ConnectionArray) );

// Channel register offsets from 0xa0, 0xb0 & 0xc0
static const WORD s_unChannelArray[] = {
  0x000,
  0x001,
  0x002,
  0x003,
  0x004,
  0x005,
  0x006,
  0x007,
  0x008,
  // OPL3 registers...
  0x100,
  0x101,
  0x102,
  0x103,
  0x104,
  0x105,
  0x106,
  0x107,
  0x108
  };
COMPILATION_REQUIRES( lengthof( s_2OpArray) == lengthof( s_unChannelArray) );

// Lookup table for operator slot no. to register offset
// Regs 0x20, 0x40, 0x60, 0x80 & 0xe0
static const int s_iSlotArray[] = {
  0x000, 0x001, 0x002, 0x003, 0x004, 0x005,
  0x008, 0x009, 0x00a, 0x00b, 0x00c, 0x00d,
  0x010, 0x011, 0x012, 0x013, 0x014, 0x015,
  // OPL3 registers...
  0x100, 0x101, 0x102, 0x103, 0x104, 0x105,
  0x108, 0x109, 0x10a, 0x10b, 0x10c, 0x10d,
  0x110, 0x111, 0x112, 0x113, 0x114, 0x115
  };
COMPILATION_REQUIRES( 36 == lengthof( s_iSlotArray) );

// Lookup table for register offset to operator slot no.
// Regs 0x20, 0x40, 0x60, 0x80 & 0xe0
static const int s_iReg2SlotArray[] = {
   0,  1,  2, 3,  4,  5,  -1, -1,
   6,  7,  8,  9, 10, 11, -1, -1,
  12, 13, 14, 15, 16, 17, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1
  };
COMPILATION_REQUIRES( 0x20 == lengthof( s_iReg2SlotArray) );

// MULT value to freq multiplier lookup
static const unsigned s_auMultiplier[] = {
  1, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 20, 24, 24, 30, 30
};
COMPILATION_REQUIRES( lengthof( s_auMultiplier) == 16);

#endif // OPT_ADLIB_EMULATOR


/***************************************************************************
 * OPL2/3 FM emulation
 ***************************************************************************/

/*
 * Initialise the emulator
 */
extern SFm* FmInit( SDs* pDs)
  {
  DWORD dwValue;
  SFm* pFm;
  TRACE( 9, ("FmInit()\n"));

  pFm = &s_fm;

  // Save emulator context ptr
  pFm->pDs = pDs;

  // Init data
  pFm->hFm = NULL;
#if OPT_ADLIB_EMULATOR
  pFm->pdsBuffer = NULL;
  pFm->hOutputThread = NULL;
  pFm->hSynthEvent = NULL;
  pFm->hEmulationTimer = 0;
  FmSetEmulationMode( pFm, kDefaultSampleRate);
#else
  pFm->eMode = kModeIndirect; // Default to using NT adlib driver
#endif

#if OPT_ADLIB_EMULATOR
//  if ( ReadRegValue( "SoundBufferLo", REG_DWORD, sizeof( dwValue), &dwValue)
//    && dwValue
//  ) {
//    pFm->dwBufferLoMs = (unsigned)dwValue;
//    }
//  else
    {
    pFm->dwBufferLoMs = OPT_BUFFER_LO_THRESHOLD;
    }
  TRACE( 2, ("FmInit() Buffer lo %u ms\n", pFm->dwBufferLoMs));

/*
  if ( ReadRegValue( "SoundBufferHi", REG_DWORD, sizeof( dwValue), &dwValue)
    && dwValue > pFm->dwBufferLoMs
  ) {
    pFm->dwBufferHiMs = (unsigned)dwValue;
    }
  else */if ( pFm->dwBufferLoMs < OPT_BUFFER_HI_THRESHOLD)
    {
    pFm->dwBufferHiMs = OPT_BUFFER_HI_THRESHOLD;
    }
  else
    {
    pFm->dwBufferHiMs = pFm->dwBufferLoMs + (OPT_BUFFER_HI_THRESHOLD - OPT_BUFFER_LO_THRESHOLD);
    }
  TRACE( 2, ("FmInit() Buffer hi %u ms\n", pFm->dwBufferHiMs));
#endif

#if !OPT_ADLIB_EMULATOR
#ifndef OPT_FIXED_SAMPLE_RATE
  if ( ReadRegValue( "AdlibEmulation", REG_DWORD, sizeof( dwValue), &dwValue)
    && dwValue
  ) 
  {
#endif
 #if OPT_FIXED_SAMPLE_RATE
    FmSetEmulationMode( pFm, OPT_FIXED_SAMPLE_RATE);
 #else
    if ( 1 == dwValue)
      dwValue = 2;
    FmSetEmulationMode( pFm, (WORD)dwValue);
 #endif
    }
#endif

#if OPT_WRITE_CACHE
  InitializeCriticalSection( &pFm->csCache);

  // Set default cache timeout
  //if ( ReadRegValue( "AdlibFlush", REG_DWORD, sizeof( dwValue), &dwValue))
   // pFm->uTimeout = (unsigned)dwValue;
  //else
    pFm->uTimeout = OPT_CACHE_TIMEOUT;
  TRACE( 2, ("FmInit() cache timeout %u ms\n", pFm->uTimeout));
#endif

#if OPT_ADLIB_EMULATOR
  // Setup a critical section to guard the synth data from 
  // concurrent foreground & background output thread access
  InitializeCriticalSection( &pFm->csSynth);

  // Create the auto reset output event
  pFm->hSynthEvent = CreateEvent( NULL, FALSE, FALSE, NULL);
  if ( NULL == pFm->hSynthEvent)
    {
    TRACE( 0, ("!!! FmInit() Can't create output event, error %u\n",
      GetLastError()));
    return NULL;
    }
#endif

  pFm->ubControlReg = 0x00;
  pFm->ubStatus = 0x00;

  return pFm;
  }


/*
 * Finalise the FM emulator
 * Single threaded
 */
extern void FmFinal( register SFm* pFm)
  {
  TRACE( 9, ("FmFinal()\n"));

  if ( NULL == pFm)
    pFm =&s_fm;

#if OPT_WRITE_CACHE
  // BUGBUG timeKillEvent hangs 'cos we're single threaded
  pFm->hTimer = 0;
#endif
#if OPT_ADLIB_EMULATOR
  // BUGBUG timeKillEvent hangs 'cos we're single threaded
  pFm->hEmulationTimer = 0;
#endif
  // Release per process resources
  FmTerminate( pFm);

  // Release global resources
#if OPT_ADLIB_EMULATOR
#if OPT_CATCH_EXCEPTIONS // BUGBUG: With DirectSound 4.07.00.0700 (DirectX 7 & Win2k) randomly get exceptions
  { unsigned long ulExcept = 0; __try {
#endif
  if ( NULL != pFm->hSynthEvent)
    {
    // This will wake the output thread with an error
    CloseHandle( pFm->hSynthEvent);
    pFm->hSynthEvent = NULL;
    }

  // Kill the output thread
  if ( NULL != pFm->hOutputThread
    && INVALID_HANDLE_VALUE != pFm->hOutputThread
  ) {
    // This should normally timeout as we're single threaded
    if ( WAIT_TIMEOUT == WaitForSingleObject( pFm->hOutputThread, 100))
      TerminateThread( pFm->hOutputThread, 1);

    CloseHandle( pFm->hOutputThread);
    pFm->hOutputThread = NULL;
    }

  DeleteCriticalSection( &pFm->csSynth);
#if OPT_CATCH_EXCEPTIONS
  } __except( ulExcept = GetExceptionCode(), EXCEPTION_EXECUTE_HANDLER) {
    TRACE( -1, (">>> FmFinal() exception 0x%lx\n", ulExcept));
  } }
#endif
#endif

#if OPT_WRITE_CACHE
  DeleteCriticalSection( &pFm->csCache);
#endif
  }


/*
 * Process terminated
 */
extern void FmTerminate( register SFm* pFm)
  {
  TRACE( 9, ("FmTerminate()\n"));

#if OPT_WRITE_CACHE
  if ( pFm->hTimer)
    {
    timeKillEventProc( pFm->hTimer);
    pFm->hTimer = 0;
    }
  if ( pFm->uCacheIndex)
    FlushCache( pFm);
#endif

  if ( NULL != pFm->hFm)
    FmReset( pFm);

  // Release the Adlib device
#if OPT_ADLIB_EMULATOR
  if ( kModeEmulate == pFm->eMode)
    {
    if ( NULL != pFm->hFm)
      {
      EmulatorStop( pFm);
      pFm->hFm = NULL;
      }
    }
  else
#endif
  if ( NULL != pFm->hFm && INVALID_HANDLE_VALUE != pFm->hFm)
    {
    TRACE( 3, ("FmTerminate() closing device...\n"));
    (void)CloseHandle( pFm->hFm);
    pFm->hFm = NULL;
    TRACE( 2, ("FmTerminate() closed\n"));
    }
  }


/*
 * Process suspended
 */
extern void FmSuspend( register SFm* pFm)
  {
  TRACE( 2, ("FmSuspend()\n"));

#if OPT_ADLIB_EMULATOR
  if ( NULL != pFm->pdsBuffer)
    IDirectSoundBuffer_Stop( pFm->pdsBuffer);
#else
  (void)pFm;
#endif
  }


/*
 * Process resumed
 */
extern void FmResume( register SFm* pFm)
  {
  TRACE( 2, ("FmResume()\n"));

#if OPT_ADLIB_EMULATOR
  if ( NULL != pFm->pdsBuffer)
    IDirectSoundBuffer_Play( pFm->pdsBuffer, 0, 0, DSBPLAY_LOOPING);
#else
  (void)pFm;
#endif
  }


/*
 * Set the cache size
 */
extern void FmSetCacheTimeout(
  register SFm* pFm,
  unsigned uTimeout
) {
  TRACE( 9, ("FmSetCacheTimeout(%p,%u)\n", pFm, uTimeout));
  ASSERT( NULL != pFm);

#if OPT_WRITE_CACHE
  pFm->uTimeout = uTimeout;
#else
  (void)pFm;
  (void)uTimeout;
#endif
  }


/*
 * Set the FM emulation mode
 */
extern void FmSetEmulationMode(
  register SFm* pFm,
  WORD wParam
) {
  TRACE( 9, ("FmSetEmulationMode(%p,%u)\n", pFm, wParam));
  ASSERT( NULL != pFm);

  switch ( wParam)
    {
  case 0:
    pFm->eMode = kModeIndirect;
    break;

  case 1:
    pFm->eMode = kModeDirect;
    break;

  default:
#if OPT_ADLIB_EMULATOR
    pFm->eMode = kModeEmulate;
 #if OPT_FIXED_SAMPLE_RATE
    pFm->dwBufferSize = kOutputBufferSize;
    pFm->dwBufferLoWater = ((pFm->dwBufferLoMs + pFm->uTimeout) * kBytesPerSample * SAMPLE_RATE( pFm)) / 1000;
    pFm->dwBufferHiWater = ((pFm->dwBufferHiMs + pFm->uTimeout) * kBytesPerSample * SAMPLE_RATE( pFm)) / 1000;
 #else
    if ( 2 == wParam)
      {
      // Leave rate at default or that set in registry
      //pFm->nSamplesPerSec = kDefaultSampleRate;
      }
    else if ( wParam < 5000)
      {
      pFm->nSamplesPerSec = kDefaultSampleRate;
      TRACE( 2, ("FmSetEmulationMode() %u sps clamped to %u\n",
        wParam, pFm->nSamplesPerSec));
      }
    else if ( wParam > 44100)
      {
      pFm->nSamplesPerSec = 44100;
      TRACE( 2, ("FmSetEmulationMode() %u sps clamped to %u\n",
        wParam, pFm->nSamplesPerSec));
      }
    else
      pFm->nSamplesPerSec = wParam;

    pFm->dwBufferSize = (kBufferMaxMs * kBytesPerSample * pFm->nSamplesPerSec) / 1000;
    pFm->dwBufferSize += (4 * sizeof(DWORD) - 1) & -(long)pFm->dwBufferSize; // Round up to DWORDs

    pFm->dwBufferLoWater = ((pFm->dwBufferLoMs + pFm->uTimeout) * kBytesPerSample * pFm->nSamplesPerSec) / 1000;
    pFm->dwBufferHiWater = ((pFm->dwBufferHiMs + pFm->uTimeout) * kBytesPerSample * pFm->nSamplesPerSec) / 1000;
 #endif
#else
    TRACE( 2, ("FmSetEmulationMode() Emulator not enabled, selecting Adlib device\n"));
    pFm->eMode = kModeIndirect;
#endif
    break;
    }
  }


/*
 * Write the synthesizer index address register
 */
extern void __fastcall FmAddress( register SFm* pFm, WORD wAddr)
  {
  TRACE( 4, ("FmAddress(0x%03x)\n", wAddr));
  ASSERT( NULL != pFm);
  ASSERT( wAddr < 0x200);

  // Early SB's have OPL2
  //if ( DspGetType( pFm->pInst->pDsp) < 4)
  //  wAddr &= 0xff;

  pFm->wAddr = wAddr;
  }


/*
 * Read the synthesizer status
 */
extern BYTE __fastcall FmStatus( register SFm* pFm)
  {
  TRACE( 9, ("FmStatus()\n"));
  ASSERT( NULL != pFm);

#if OPT_EMULATE_STATUS
  //if ( DspGetType( pFm->pInst->pDsp) >= 4)
    {
    // Later SBPro2, SB16 & later use YMF262(OPL3)
    pFm->ubStatus &= 0xf0;
    }
  //else
    //{
    // Older models including early SBPro2 use YM3812(OPL2)
    //pFm->ubStatus &= 0xf0;
    //pFm->ubStatus |= 0x06;
    //}

  // Update timer 1
  if ( (pFm->ubControlReg & 1)
    && pFm->uTimer1
    && 0 == --pFm->uTimer1
  ) {
    pFm->ubStatus |= 0x40;
    }

  // Update timer 2
  if ( (pFm->ubControlReg & 2)
   && pFm->uTimer2
   && 0 == --pFm->uTimer2
  ) {
    pFm->ubStatus |= 0x20;
    }

  // Assert IRQ flag if either timer flag set
  if ( pFm->ubStatus & 0x60)
    pFm->ubStatus |= 0x80;

  return pFm->ubStatus;
#else
  // Read cached status if poss
  if ( pFm->iStatusValid > 0)
    {
    // Unconditionally valid
    TRACE( 4, ("FmStatus() unconditionally valid\n"));
    return pFm->ubStatus;
    }
  if ( pFm->iStatusValid < 0)
    {
    // Age the cache
    TRACE( 4, ("FmStatus() cache age %d\n", pFm->iStatusValid));
    pFm->iStatusValid++;
    return pFm->ubStatus;
    }

  // Open the device if not already
  if ( NULL == pFm->hFm)
    FmOpen( pFm);

#if OPT_WRITE_CACHE
  // Write the cache to device - leave timer running
  EnterCriticalSection( &pFm->csCache);
  if ( pFm->uCacheIndex)
    FlushCache( pFm);
  LeaveCriticalSection( &pFm->csCache);
#endif

  // Refresh the status register cache
  if ( FmDeviceStatus( pFm, &pFm->ubStatus))
    {
    TRACE( 3, ("FmStatus() driver status 0x%02x\n", pFm->ubStatus));

    // Indicate status cache conditionally valid if any timer started
    if ( !(pFm->ubControlReg & 3))
      {
      // Timers not enabled
      pFm->iStatusValid = 1;              // Unconditionally valid
      }
    else
      {
      pFm->iStatusValid = -5;             // Valid for -n status reads
      }
    }

  return pFm->ubStatus;
#endif
  }


/*
 * Write synthesizer register
 */
extern void __fastcall FmWrite(
  register SFm* pFm,
  BOOL bOPL3,
  BYTE ubData
) {
#if OPT_WRITE_CACHE
  unsigned uIndex;
#endif

  TRACE( 4, ("FmWrite() 0x%02x to register 0x%03x\n", ubData, pFm->wAddr));
  ASSERT( NULL != pFm);

  // Open the device if not already
  if ( NULL == pFm->hFm)
    FmOpen( pFm);

  //if ( DspGetType( pFm->pInst->pDsp) < 4)
    //bOPL3 = FALSE;

  if ( 4 == pFm->wAddr)
    {
    // Control register
    TRACE( 2, ("FmWrite() reset/mask/start 0x%02x\n", ubData));
    pFm->ubControlReg = ubData;

#if OPT_EMULATE_STATUS
    if (ubData & 0x80)
      {
      // IRQ reset
      pFm->ubStatus &= ~0x80;
      }
    else
      {
      // Timer 1
      if (ubData & 0x40)
        pFm->ubStatus &= ~0x40;         // Timer1 status reset
      else if ( ubData & 1)
        pFm->uTimer1 = 1;               // Start timer1 x80us
        //pFm->ubStatus |= 0x40;        // Immediate expiry

      // Timer 2
      if ( ubData & 0x20)
        pFm->ubStatus &= ~0x20;         // Timer1 status reset
      else if ( ubData & 2)
        pFm->uTimer2 = 1;               // Start timer2 x320us
        //pFm->ubStatus |= 0x20;        // Immediate expiry
      }

    // Assert IRQ flag if any timer flag set
    if ( pFm->ubStatus & 0x60)
      {
      pFm->ubStatus |= 0x80;
      }
    else
      {
      // No unmasked timer flags so clear IRQ flag
      pFm->ubStatus &= ~0x80;
      }

#if OPT_WRITE_CACHE
    // Write thru to device if caching disabled
    if ( pFm->uTimeout)
      return;
#endif
#else
    // Invalidate the status cache
    pFm->iStatusValid = 0;
#endif
    }

#if OPT_ADLIB_EMULATOR
  if ( kModeEmulate == pFm->eMode)
    {
    // Update the register cache
    EmulatorWrite( pFm, pFm->wAddr, ubData);
    return;
    }
#endif // OPT_ADLIB_EMULATOR
  }


/*
 * Set volume
 */
extern BOOL FmSetVolume(
  register SFm* pFm, 
  BYTE ubLeftVolume,
  BYTE ubRightVolume
) {
  TRACE( 3, ("FmSetVolume(%u,%u) \n", ubLeftVolume, ubRightVolume));
  return FALSE;
  }


/***************************************************************************
 * Internal functions
 ***************************************************************************/

/*
 * Open the FM device
 */
static BOOL FmOpen( register SFm* pFm)
  {
  TRACE( 9, ("FmOpen()\n"));

  if ( INVALID_HANDLE_VALUE == pFm->hFm)
    return FALSE;
  if ( NULL != pFm->hFm)
    return TRUE;

  // open the FM device
  for ( ;;)
    {
      pFm->hFm = INVALID_HANDLE_VALUE;
      if ( EmulatorStart( pFm))
        break;
      TRACE( 1, ("*** FmOpen() Can't start the OPL3 emulator\n"));
    }


  ASSERT( NULL != pFm->hFm);
  FmReset( pFm);

  return TRUE;
  }


/*
 * Reset the synthesiser to power on state
 */
static void FmReset(
  register SFm* const pFm
) {
  static USynthData synth[] = {
    // Reset rhythm
    { 0x388, kRegRhythm}, { 0x389, 0x00},
    { 0x388, kRegRhythm}, { 0x389, 0x20},
    { 0x388, kRegFnum+6}, { 0x389, 0x00},
    { 0x388, kRegBlock+6}, { 0x389, 0x0a},
    { 0x388, kRegFnum+7}, { 0x389, 0x0b},
    { 0x388, kRegBlock+7}, { 0x389, 0x0a},
    { 0x388, kRegFnum+8}, { 0x389, 0x57},
    { 0x388, kRegBlock+8}, { 0x389, 0x09},

    // OPL3 mode
    { 0x38a, 0x05}, { 0x38b, 0x01},
    { 0x38a, 0x04}, { 0x38b, 0x3f},

    { 0x388, kRegTest}, { 0x389, 0x20},
    { 0x388, kRegNts}, { 0x389, 0x00},

    // Reset rhythm again
    { 0x388, kRegRhythm}, { 0x389, 0x20},
    { 0x388, kRegFnum+6}, { 0x389, 0x00},
    { 0x388, kRegBlock+6}, { 0x389, 0x0a},
    { 0x388, kRegFnum+7}, { 0x389, 0x0b},
    { 0x388, kRegBlock+7}, { 0x389, 0x0a},
    { 0x388, kRegFnum+8}, { 0x389, 0x57},
    { 0x388, kRegBlock+8}, { 0x389, 0x09},

    // All channels off
    { 0x388, kRegRhythm}, { 0x389, 0x00},
    { 0x388, kRegBlock}, { 0x389, 0x00},
    { 0x388, kRegBlock+1}, { 0x389, 0x00},
    { 0x388, kRegBlock+2}, { 0x389, 0x00},
    { 0x388, kRegBlock+3}, { 0x389, 0x00},
    { 0x388, kRegBlock+4}, { 0x389, 0x00},
    { 0x388, kRegBlock+5}, { 0x389, 0x00},
    { 0x388, kRegBlock+6}, { 0x389, 0x00},
    { 0x388, kRegBlock+7}, { 0x389, 0x00},
    { 0x388, kRegBlock+8}, { 0x389, 0x00},

    { 0x388, kRegFb}, { 0x389, 0x00},
    { 0x388, kRegFb+1}, { 0x389, 0x00},
    { 0x388, kRegFb+2}, { 0x389, 0x00},
    { 0x388, kRegFb+3}, { 0x389, 0x00},
    { 0x388, kRegFb+4}, { 0x389, 0x00},
    { 0x388, kRegFb+5}, { 0x389, 0x00},
    { 0x388, kRegFb+6}, { 0x389, 0x00},
    { 0x388, kRegFb+7}, { 0x389, 0x00},
    { 0x388, kRegFb+8}, { 0x389, 0x00},

    { 0x388, kRegWs}, { 0x389, 0x00},
    { 0x388, kRegWs+1}, { 0x389, 0x00},
    { 0x388, kRegWs+2}, { 0x389, 0x00},
    { 0x388, kRegWs+3}, { 0x389, 0x00},
    { 0x388, kRegWs+4}, { 0x389, 0x00},
    { 0x388, kRegWs+5}, { 0x389, 0x00},
    { 0x388, kRegWs+8}, { 0x389, 0x00},
    { 0x388, kRegWs+9}, { 0x389, 0x00},
    { 0x388, kRegWs+0xa}, { 0x389, 0x00},
    { 0x388, kRegWs+0xb}, { 0x389, 0x00},
    { 0x388, kRegWs+0xc}, { 0x389, 0x00},
    { 0x388, kRegWs+0xd}, { 0x389, 0x00},
    { 0x388, kRegWs+0x10}, { 0x389, 0x00},
    { 0x388, kRegWs+0x11}, { 0x389, 0x00},
    { 0x388, kRegWs+0x12}, { 0x389, 0x00},
    { 0x388, kRegWs+0x13}, { 0x389, 0x00},
    { 0x388, kRegWs+0x14}, { 0x389, 0x00},
    { 0x388, kRegWs+0x15}, { 0x389, 0x00},

    // OPL3 channels
    { 0x38a, kRegRhythm}, { 0x38b, 0x00},
    { 0x38a, kRegBlock}, { 0x38b, 0x00},
    { 0x38a, kRegBlock+1}, { 0x38b, 0x00},
    { 0x38a, kRegBlock+2}, { 0x38b, 0x00},
    { 0x38a, kRegBlock+3}, { 0x38b, 0x00},
    { 0x38a, kRegBlock+4}, { 0x38b, 0x00},
    { 0x38a, kRegBlock+5}, { 0x38b, 0x00},
    { 0x38a, kRegBlock+6}, { 0x38b, 0x00},
    { 0x38a, kRegBlock+7}, { 0x38b, 0x00},
    { 0x38a, kRegBlock+8}, { 0x38b, 0x00},

    { 0x38a, kRegFb}, { 0x38b, 0x00},
    { 0x38a, 0xc1}, { 0x38b, 0x00},
    { 0x38a, 0xc2}, { 0x38b, 0x00},
    { 0x38a, 0xc3}, { 0x38b, 0x00},
    { 0x38a, 0xc4}, { 0x38b, 0x00},
    { 0x38a, 0xc5}, { 0x38b, 0x00},
    { 0x38a, 0xc6}, { 0x38b, 0x00},
    { 0x38a, 0xc7}, { 0x38b, 0x00},
    { 0x38a, 0xc8}, { 0x38b, 0x00},

    { 0x38a, kRegWs}, { 0x38b, 0x00},
    { 0x38a, 0xe1}, { 0x38b, 0x00},
    { 0x38a, 0xe2}, { 0x38b, 0x00},
    { 0x38a, 0xe3}, { 0x38b, 0x00},
    { 0x38a, 0xe4}, { 0x38b, 0x00},
    { 0x38a, 0xe5}, { 0x38b, 0x00},
    { 0x38a, 0xe8}, { 0x38b, 0x00},
    { 0x38a, 0xe9}, { 0x38b, 0x00},
    { 0x38a, 0xea}, { 0x38b, 0x00},
    { 0x38a, 0xeb}, { 0x38b, 0x00},
    { 0x38a, 0xec}, { 0x38b, 0x00},
    { 0x38a, 0xed}, { 0x38b, 0x00},
    { 0x38a, 0xf0}, { 0x38b, 0x00},
    { 0x38a, 0xf1}, { 0x38b, 0x00},
    { 0x38a, 0xf2}, { 0x38b, 0x00},
    { 0x38a, 0xf3}, { 0x38b, 0x00},
    { 0x38a, 0xf4}, { 0x38b, 0x00},
    { 0x38a, 0xf5}, { 0x38b, 0x00},

    // Ensure in OPL2 mode
    { 0x38a, 0x04}, { 0x38b, 0x00},
    { 0x38a, 0x05}, { 0x38b, 0x00}
  };

  TRACE( 9, ("FmReset(%p)\n",pFm));
  FmDeviceWrite( pFm, &synth[0], sizeof( synth));
  }


/*
 * Write to FM device
 */
static BOOL FmDeviceWrite(
  register SFm* const pFm,
  const USynthData* pSynthData,
  DWORD dwCount
) {
  TRACE( 9, ("FmDeviceWrite(%p,%p,%u)\n",pFm,pSynthData,dwCount));
  ASSERT( NULL != pSynthData);
  ASSERT( dwCount);

    {
    unsigned uIndex;

    if ( NULL == pFm->hFm)
      return FALSE;

    // Unpack the register writes
    uIndex = 0;
    for ( ; dwCount; dwCount -= sizeof( *pSynthData), ++pSynthData)
      {
      switch ( pSynthData->adlib.IoPort)
        {
      case 0x388: // Address
        uIndex = pSynthData->adlib.PortData;
        break;
      case 0x38a: // OPL3 Address
        uIndex = 0x100 + pSynthData->adlib.PortData;
        break;
      case 0x389: // Data
      case 0x38b: // OPL3 data
        // Write to emulator
        EmulatorWrite( pFm, uIndex, (BYTE)pSynthData->adlib.PortData);
        break;
      default:
        ASSERT( !"FmDeviceWrite to invalid port");
        return FALSE;
        }
      }
    return TRUE;
    }
  }



#if OPT_WRITE_CACHE
/*
 * Cache flush timer
 */
static void CALLBACK WriteBackTimer(
  UINT uID,
  UINT uMsg,
  DWORD dwUser,
  DWORD dw1,
  DWORD dw2
) {
  SFm* pFm = (SFm*)dwUser;
  (void)uID;
  (void)uMsg;
  (void)dw1;
  (void)dw2;

  TRACE( 4, ("WriteBackTimer()\n"));
  ASSERT( NULL != pFm);

  EnterCriticalSection( &pFm->csCache);

  pFm->hTimer = 0;
  if ( pFm->uCacheIndex)
    FlushCache( pFm);

  LeaveCriticalSection( &pFm->csCache);
  }


/*
 * Write the OPL register cache to the device
 * MUST hold csCache before calling
 */
static void FlushCache( register SFm* pFm)
  {
  DWORD dwCount;

  TRACE( 9, ("FlushCache()\n"));
  ASSERT( pFm->uCacheIndex);

  TRACE( 3, ("FlushCache() %u entries\n", pFm->uCacheIndex));

  dwCount = pFm->uCacheIndex * sizeof( pFm->aSynth[0]);
  pFm->uCacheIndex = 0;
  FmDeviceWrite( pFm, &pFm->aSynth[0], dwCount);
  }
#endif // OPT_WRITE_CACHE


#if OPT_ADLIB_EMULATOR
/***************************************************************************
 * The OPL2/3 emulator
 ***************************************************************************/

/*
 * Initialise the emulator
 */
BOOL EmulatorStart( register SFm* const pFm)
  {
  TRACE( 9, ("EmulatorStart()\n"));
  ASSERT( NULL != pFm);

  // Create the synthesiser thread
  if ( NULL == pFm->hOutputThread)
    {
    DWORD dwThreadId;
    pFm->hOutputThread = CreateThread(
      NULL,
      0,                    // Stack size
      EmulatorThread,       // Function
      pFm,                  // Thread arg
      0,                    // Creation flags
      &dwThreadId
    );
    if ( NULL == pFm->hOutputThread)
      {
      TRACE( 0, ("!!! EmulatorStart() Can't create synthesiser thread, error %u\n",
        GetLastError()));
      return FALSE;
      }
    TRACE( 3, ("EmulatorStart() Created OLP3 emulator thread\n"));

    // Adjust priority
    ASSERT( SetThreadPriority(
      pFm->hOutputThread,
      //THREAD_PRIORITY_ABOVE_NORMAL
      THREAD_PRIORITY_HIGHEST
    ));
    }

  while ( NULL == pFm->pdsBuffer)
    {
    WAVEFORMATEX wfx;
    HRESULT hr;
    LPDIRECTSOUND pDS;

    // Open DirectSound & set output format
    RtlZeroMemory( &wfx, sizeof( wfx));
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = kChannels;
    wfx.nSamplesPerSec = SAMPLE_RATE( pFm);
    wfx.wBitsPerSample = kBitsPerChannel;
    wfx.nBlockAlign = (WORD)(((wfx.wBitsPerSample + 7) / 8) * wfx.nChannels);
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

    hr = DirectSoundOpen( pFm->pDs, &wfx, &pDS);
    if ( SUCCEEDED( hr))
      {
      unsigned uChannel;
      DSBUFFERDESC dsbdesc;
      DWORD dwInterval;

      // Create a secondary buffer
      RtlZeroMemory( &dsbdesc, sizeof( dsbdesc) );
      dsbdesc.dwSize = sizeof( dsbdesc);
      dsbdesc.dwFlags = 0
        | (1 * DSBCAPS_GETCURRENTPOSITION2)
        | (1 * DSBCAPS_GLOBALFOCUS)
        | (0 * DSBCAPS_LOCSOFTWARE)
      ;
      dsbdesc.dwBufferBytes = pFm->dwBufferSize;
      dsbdesc.lpwfxFormat = &wfx;
      hr = IDirectSound_CreateSoundBuffer(
        pDS,
        &dsbdesc,
        &pFm->pdsBuffer,
        NULL
      );
      if ( SUCCEEDED( hr))
        {
        ASSERT( NULL != pFm->pdsBuffer);
        TRACE( 2, ("EmulatorStart() Created %lu byte secondary buffer 0x%p, lo water %lu, hi water %lu\n",
          pFm->dwBufferSize, pFm->pdsBuffer, pFm->dwBufferLoWater, pFm->dwBufferHiWater));

        // Set no. emulator clock ticks per output interval
        dwInterval = pFm->uTimeout;
        if ( dwInterval < 1)
          dwInterval = 1;
        pFm->dwTicksPerInterval = (SAMPLE_RATE( pFm) * dwInterval + 999) / /*1000*/ 950;

        TRACE( 3, ("EmulatorStart() Creating %lu ms output timer @ %lu ticks\n",
          dwInterval, pFm->dwTicksPerInterval));
        pFm->hEmulationTimer = timeSetEventProc(
          dwInterval,
          0,
          (LPTIMECALLBACK)(DWORD)pFm->hSynthEvent,
          (DWORD)pFm,
          TIME_PERIODIC | TIME_CALLBACK_EVENT_SET
        );
        if ( 0 == pFm->hEmulationTimer)
          TRACE( 0, ("!!! EmulatorStart() timeSetEvent failed %#lx\n", GetLastError() ));

        // Initialise the emulator
        EnterCriticalSection( &pFm->csSynth);

        // Reset all regs, operators & channels
        RtlZeroMemory( pFm->aubRegister, sizeof( pFm->aubRegister) );
        RtlZeroMemory( &pFm->op[0], sizeof( pFm->op) );
        RtlZeroMemory( &pFm->chnl[0], sizeof( pFm->chnl) );

        // Set all channels to 2operator mode
        for ( uChannel = 0; uChannel < lengthof( pFm->chnl); ++uChannel)
          {
          unsigned uOp;
          pFm->chnl[ uChannel].nType = 2; // 2operator
          pFm->chnl[ uChannel].ulPhaseIncr = ULONG_MAX;

          // Init all operators
          for ( uOp = 0; uOp < 2; ++uOp)
            {
            unsigned uSlot;
            ASSERT( uChannel < lengthof( s_2OpArray));
            uSlot = s_2OpArray[ uChannel].slot[ uOp];
            ASSERT( uSlot < lengthof( pFm->op));
            pFm->op[ uSlot].uChannel = uChannel;
            pFm->op[ uSlot].iAttenuation = kMaxdB;
            pFm->op[ uSlot].eAdsr = kAdsrRelease;
            pFm->op[ uSlot].bNoise = FALSE;
            }
          }

        // Prime the output buffer
        EmulatorOutput( pFm, pFm->dwTicksPerInterval);

        // Start playback
        hr = IDirectSoundBuffer_Play( pFm->pdsBuffer, 0, 0, DSBPLAY_LOOPING);
        if ( FAILED( hr))
          TRACE( 0, ("!!! EmulatorStart() IDirectSoundBuffer_Play failed 0x%08lx\n", hr));
        else
          TRACE( 3, ("EmulatorStart() Playback started\n"));

        LeaveCriticalSection( &pFm->csSynth);
        break;
        }

      TRACE( 1, ("*** EmulatorStart() IDirectSound_CreateSoundBuffer %d sps, %d bits, %d channels failed 0x%08lx\n",
        wfx.nSamplesPerSec, wfx.wBitsPerSample, wfx.nChannels, hr));
      }
      return FALSE;                     // Continue without music
    }

  return TRUE;
  }


/*
 * Finalise the emulator
 */
void EmulatorStop( register SFm* const pFm)
  {
  TRACE( 9, ("EmulatorStop()\n"));
  ASSERT( NULL != pFm);

  // Claim the Synth to exclude the output thread
  EnterCriticalSection( &pFm->csSynth);

  if ( NULL != pFm->pdsBuffer)
    {
#if OPT_CATCH_EXCEPTIONS // BUGBUG: With DirectSound 4.07.00.0700 (DirectX 7 & Win2k) randomly get exceptions
    { unsigned long ulExcept = 0; __try {
#endif
    TRACE( 3, ("EmulatorStop() Stopping playback...\n"));
    IDirectSoundBuffer_Stop( pFm->pdsBuffer);
    TRACE( 2, ("EmulatorStop() Playback stopped\n"));

    TRACE( 3, ("EmulatorStop() Releasing buffer 0x%p...\n", pFm->pdsBuffer));
    IDirectSoundBuffer_Release( pFm->pdsBuffer);
    TRACE( 2, ("EmulatorStop() Buffer released\n"));
#if OPT_CATCH_EXCEPTIONS
    } __except( ulExcept = GetExceptionCode(), EXCEPTION_EXECUTE_HANDLER) {
      TRACE( -1, (">>> EmulatorStop() IDirectSound exception 0x%lx\n", ulExcept));
    } }
#endif

    pFm->pdsBuffer = NULL;
    }

  if ( pFm->hEmulationTimer)
    {
    timeKillEventProc( pFm->hEmulationTimer);
    pFm->hEmulationTimer = 0;
    }

  // Release the Synth
  LeaveCriticalSection( &pFm->csSynth);
  }


/*
 * Emulator register write
 */
void __fastcall EmulatorWrite(
  register SFm* const pFm,
  unsigned uIndex,          // IN: Register index (0..0x1ff), 0x100+ are OPL3
  BYTE const ubData
) {
  unsigned uChannel;

  TRACE( 9, ("EmulatorWrite(0x%p,%u,%u)\n", pFm,uIndex,ubData));
  ASSERT( NULL != pFm);
  ASSERT( uIndex < lengthof( pFm->aubRegister));

  // Claim the Synth to exclude the output thread
  EnterCriticalSection( &pFm->csSynth);

  TRACE( 3, ("EmulatorWrite() reg %#x data %#x)\n", uIndex,ubData));
  pFm->aubRegister[ uIndex] = ubData;

  // Check if any recalc'n needed
  switch ( 0xf0 & uIndex)
    {
  case 0x00:
    if ( 0x08 == uIndex)
      {
      if ( 0x80 & ubData)
        TRACE( 1, ("*** EmulatorWrite() CSW enabled, data %02x\n", ubData));
      else
        TRACE( 2, ("EmulatorWrite() NTS(d5)= %02x\n", ubData));
      break;
      }

    if ( kRegConnSel == uIndex
      || kRegNew == uIndex
    ) {
      TRACE( 2, ("EmulatorWrite() OPL3 reg 0x%03x, data %02x\n", uIndex, ubData));

      // Determine operators per channel
      for ( uChannel = 0; uChannel < lengthof( s_ConnectionArray); ++uChannel)
        {
        unsigned uPrimary, uSecondary;
        uPrimary = s_ConnectionArray[ uChannel].chnl1;
        uSecondary = s_ConnectionArray[ uChannel].chnl2;

        // Mark osc. phase for re-calc
        ASSERT( lengthof( pFm->chnl) > uPrimary);
        pFm->chnl[ uPrimary].ulPhaseIncr = ULONG_MAX;
        ASSERT( lengthof( pFm->chnl) > uSecondary);
        pFm->chnl[ uSecondary].ulPhaseIncr = ULONG_MAX;

        if ( (1 & pFm->aubRegister[ kRegNew])
          && ((1 << uChannel) & pFm->aubRegister[ kRegConnSel])
        ) {
          // This is configured as 4op
          TRACE( 3, ("EmulatorWrite() 4op channel %u\n", 1+uChannel));
          pFm->chnl[ uPrimary].nType = 4;
          pFm->chnl[ uSecondary].nType = 4;
    
          // Assign slots to channels
          ASSERT( lengthof( s_2OpArray) > uPrimary);
          pFm->op[ s_2OpArray[ uPrimary].slot[ 0] ].uChannel = uPrimary;
          pFm->op[ s_2OpArray[ uPrimary].slot[ 1] ].uChannel = uPrimary;
          ASSERT( lengthof( s_2OpArray) > uSecondary);
          pFm->op[ s_2OpArray[ uSecondary].slot[ 0] ].uChannel = uPrimary;
          pFm->op[ s_2OpArray[ uSecondary].slot[ 1] ].uChannel = uPrimary;
          }
        else
          {
          // This is configured as 2op
          COMPILATION_REQUIRES( 2 == lengthof(s_2OpArray[ uChannel].slot));
          TRACE( 3, ("EmulatorWrite() 2op channels %u & %u\n", 1+uPrimary, 1+uSecondary));
          pFm->chnl[ uPrimary].nType = 2;
          pFm->chnl[ uSecondary].nType = 2;
    
          // Assign slots to channels
          ASSERT( lengthof( s_2OpArray) > uPrimary);
          pFm->op[ s_2OpArray[ uPrimary].slot[ 0] ].uChannel = uPrimary;
          pFm->op[ s_2OpArray[ uPrimary].slot[ 1] ].uChannel = uPrimary;
          ASSERT( lengthof( s_2OpArray) > uSecondary);
          pFm->op[ s_2OpArray[ uSecondary].slot[ 0] ].uChannel = uSecondary;
          pFm->op[ s_2OpArray[ uSecondary].slot[ 1] ].uChannel = uSecondary;
          }
        }
      }
    break;

  case kRegBlock:
    if ( kRegRhythm != uIndex)
      {
      // Change of fnum, block & KON
      // Find channel
      uChannel = uIndex & 0x0f;
      if ( uChannel < 9)
        {
        BOOL bKon;
        if ( uIndex & 0x100)
          uChannel += 9; // OPL3

        // Handle KON/off events
        bKon = 0x20 & ubData;
        if ( 4 == pFm->chnl[ uChannel].nType)
          {
          // Convert channel no. to 4op index
          uChannel = uIndex & 0x0f;
          uChannel %= 3;
          if ( uIndex & 0x100)
            uChannel += 3; // OPL3
          ASSERT( lengthof( s_4OpArray) > uChannel);
          EmulatorKonEvent( pFm, s_4OpArray[ uChannel].slot[ 0], bKon);
          EmulatorKonEvent( pFm, s_4OpArray[ uChannel].slot[ 1], bKon);
          EmulatorKonEvent( pFm, s_4OpArray[ uChannel].slot[ 2], bKon);
          EmulatorKonEvent( pFm, s_4OpArray[ uChannel].slot[ 3], bKon);
          }
        else
          {
          ASSERT( lengthof( s_2OpArray) > uChannel);
          EmulatorKonEvent( pFm, s_2OpArray[ uChannel].slot[ 0], bKon);
          EmulatorKonEvent( pFm, s_2OpArray[ uChannel].slot[ 1], bKon);
          }
        }
      }
    else
      {
      // Test if (de)selecting rhythm sounds
      BOOL bNoise = 0x20 & ubData;
      TRACE( 3, ("EmulatorWrite() Rhythm ctrl= 0x%02x\n", ubData));

      // Channels 7,8,9 are rhythm sounds
      // Enable white noise on selected slots
      //pFm->op[ s_2OpArray[ 6].slot[ 0] ].bNoise = bNoise; // BD
      //pFm->op[ s_2OpArray[ 6].slot[ 1] ].bNoise = bNoise; // BD
      pFm->op[ s_2OpArray[ 7].slot[ 0] ].bNoise = bNoise; // HH
      pFm->op[ s_2OpArray[ 7].slot[ 1] ].bNoise = bNoise; // SD
      //pFm->op[ s_2OpArray[ 8].slot[ 0] ].bNoise = bNoise; // TOM
      pFm->op[ s_2OpArray[ 8].slot[ 1] ].bNoise = bNoise; // TC

      if ( bNoise)
        {
        // Handle rhythm KON/off events
        EmulatorKonEvent( pFm, 12, ubData & 0x10); // BD
        EmulatorKonEvent( pFm, 15, ubData & 0x10); // BD
        EmulatorKonEvent( pFm, 13, ubData & 0x01); // HH
        EmulatorKonEvent( pFm, 16, ubData & 0x08); // SD
        EmulatorKonEvent( pFm, 14, ubData & 0x04); // TOM
        EmulatorKonEvent( pFm, 17, ubData & 0x02); // TC
        }
      break;
      }
    // Fall thru

  case kRegFnum: // Channel fnum
    // Convert reg index to channel no.
    uChannel = uIndex & 0x0f;
    if ( uChannel < 9)
      {
      if ( uIndex & 0x100)
        uChannel += 9; // OPL3

      // Indicate osc must be recalc'd
      ASSERT( uChannel < lengthof( pFm->chnl)); 
      pFm->chnl[ uChannel].ulPhaseIncr = ULONG_MAX;
      }
    break;

  case kRegFb: // Channel CNT change so reset slot modulation
    // Convert reg index to channel no.
    uChannel = uIndex & 0x0f;
    if ( uChannel < 9)
      {
      if ( uIndex & 0x100)
        uChannel += 9; // OPL3

      ASSERT( uChannel < lengthof( s_2OpArray)); 
      pFm->op[ s_2OpArray[ uChannel].slot[ 0]].lModulation = 0;
      pFm->op[ s_2OpArray[ uChannel].slot[ 1]].lModulation = 0;
      }
    break;
    }

  // Release the Synth
  LeaveCriticalSection( &pFm->csSynth);
  }


/*
 * This is the synthesiser output thread
 * It runs periodically to keep the output buffer full
 */
static DWORD WINAPI EmulatorThread( LPVOID pvContext)
  {
  register SFm* const pFm = (SFm*)pvContext;

  TRACE( 9, ("EmulatorThread(0x%p)\n", pvContext));
  ASSERT( NULL != pFm);

  for (;;)
    {
    DWORD dwResult;

    // Wait for an event
    TRACE( 3, ("EmulatorThread() waiting...\n"));
    dwResult = WaitForSingleObject( pFm->hSynthEvent, INFINITE);
    if ( WAIT_OBJECT_0 != dwResult && WAIT_TIMEOUT != dwResult)
      {
      TRACE( 1, ("*** EmulatorThread() WaitForSingleObject returned %u, error %u\n",
        dwResult, GetLastError()));
      break;
      }

    // Claim the Synth to exclude the foreground VDM
    TRACE( 3, ("EmulatorThread() got event, claiming Synth...\n"));
    EnterCriticalSection( &pFm->csSynth);
    TRACE( 3, ("EmulatorThread() claimed Synth\n"));

    // Refill the output buffer
    if ( NULL != pFm->pdsBuffer)
      {
#if OPT_CATCH_EXCEPTIONS
      { unsigned long ulExcept = 0; __try {
#endif
      EmulatorOutput( pFm, pFm->dwTicksPerInterval);
#if OPT_CATCH_EXCEPTIONS
      } __except( ulExcept = GetExceptionCode(), EXCEPTION_EXECUTE_HANDLER) {
        TRACE( -1, (">>> EmulatorThread() uncaught exception 0x%08lx\n", ulExcept));
      } }
#endif
      }

    // Release the Synth
    LeaveCriticalSection( &pFm->csSynth);
    }

  TRACE( 1, ("EmulatorThread() exit\n"));
  return 0;
  }


/*
 * Topup the output buffer
 */
DWORD __fastcall EmulatorOutput(        // Returns no. bytes written
  register SFm* const pFm,
  DWORD dwTicks                         // IN: Hint - no. emulator clock ticks
) {
  HRESULT hr;
  DWORD dwPlay, dwWrite, dwBytes, dwBytes1, dwBytes2;
  LPVOID pvBlock1, pvBlock2;
  BYTE* lpc;
  DWORD dwCurrent;

  TRACE( 9, ("EmulatorOutput(0x%p,%lu)\n", pFm, dwTicks));
  ASSERT( NULL != pFm);
  ASSERT( NULL != pFm->pdsBuffer);
  ASSERT( dwTicks);

  // Calc. amount of space in the output buffer
  hr = IDirectSoundBuffer_GetCurrentPosition(
    pFm->pdsBuffer,
    &dwPlay,
    &dwWrite
  );
  if ( FAILED( hr))
    {
    TRACE( 0, ("!!! EmulatorOutput() IDirectSoundBuffer_GetCurrentPosition failed 0x%08lx\n", hr));
    return 0;
    }

  // Calc. bytes in hand
  if ( pFm->dwWritten >= dwWrite && (pFm->dwWritten < dwPlay || dwPlay <= dwWrite))
    dwBytes2 = pFm->dwWritten - dwWrite;
  else if ( pFm->dwWritten < dwWrite && pFm->dwWritten < dwPlay && dwPlay <= dwWrite)
    dwBytes2 = pFm->dwBufferSize - (dwWrite - pFm->dwWritten);
  else
    {
    TRACE( 2, ("EmulatorOutput() underrun: play=%lu, write=%lu, current=%lu\n",
      dwPlay, dwWrite, pFm->dwWritten));

    // Increase the buffer thresholds
    dwBytes2 = pFm->dwBufferSize / 64; // ~10ms
    if ( pFm->dwBufferHiWater < pFm->dwBufferSize / 2)
      {
      pFm->dwBufferHiWater += dwBytes2;
      pFm->dwBufferLoWater += dwBytes2;
      }

    // Set write position to low water
    dwBytes2 = pFm->dwBufferLoWater;
    pFm->dwWritten = dwWrite + dwBytes2;
    if ( pFm->dwWritten >= pFm->dwBufferSize)
      pFm->dwWritten -= pFm->dwBufferSize;
    }

  // Calc. buffer size available
  if ( pFm->dwWritten < dwPlay)
    dwBytes = dwPlay - pFm->dwWritten;
  else
    dwBytes = pFm->dwBufferSize - (pFm->dwWritten - dwPlay);
  ASSERT( dwBytes <= pFm->dwBufferSize);
  if ( dwBytes > kBytesPerSample)
    dwBytes -= kBytesPerSample;
  else
    dwBytes = 0;
  TRACE( 3, ("EmulatorOutput() Play=%lu, Write=%lu, Current=%lu\n",
    dwPlay, dwWrite, pFm->dwWritten));
  TRACE( 3, ("EmulatorOutput() %lu ticks requested, %lu in hand, space for %lu\n",
    dwTicks, dwBytes2 / kBytesPerSample, dwBytes / kBytesPerSample));

  // Simple PID loop to sync emulator clock rate with output
  if ( dwBytes2 >= pFm->dwBufferHiWater)
    {
    TRACE( 3, ("EmulatorOutput() slow down\n"));

    //return 0; // Just skip this burst
    dwTicks /= 2;

    //if ( pFm->dwTicksPerInterval > 1)
    //  --(pFm->dwTicksPerInterval);
    }
  else if ( dwBytes2 < pFm->dwBufferLoWater)
    {
    TRACE( 2, ("EmulatorOutput() speed up: play=%lu, write=%lu, current=%lu, %lu pending, %lu available\n",
      dwPlay, dwWrite, pFm->dwWritten, dwBytes2, dwBytes));

    dwTicks += dwTicks;
    //++(pFm->dwTicksPerInterval);
    }

  // Only request sufficient bytes for the no. ticks requested
  dwBytes1 = dwTicks * kBytesPerSample;
  if ( dwBytes > dwBytes1)
    dwBytes = dwBytes1;

  // Round down to whole no. samples
  dwBytes &= ~(kBytesPerSample - 1);
  if ( dwBytes < kBytesPerSample)
    {
    TRACE( 2, ("EmulatorOutput() buffer full: play=%lu, write=%lu, current=%lu, %lu pending, %lu available\n",
      dwPlay, dwWrite, pFm->dwWritten, dwBytes2, dwBytes));
    return 0;
    }
  ASSERT( 0 == (dwBytes % kBytesPerSample));

  // Get a buffer write pointer
  hr = IDirectSoundBuffer_Lock(
    pFm->pdsBuffer,
    pFm->dwWritten,
    dwBytes,
    &pvBlock1, &dwBytes1,
    &pvBlock2, &dwBytes2,
    0
  );
  if ( FAILED( hr))
    {
    TRACE( 1, ("*** EmulatorOutput() IDirectSoundBuffer_Lock failed 0x%08lx, bytes=%lu\n",
      hr, dwBytes));
    return 0;
    }
  ASSERT( dwBytes == dwBytes1 + dwBytes2);
  ASSERT( NULL != pvBlock1);
  ASSERT( 0 != dwBytes1);
  ASSERT( 0 == dwBytes2 || NULL != pvBlock2);

  // Simulate FM sample clock ticks until request fulfilled
  lpc = (PUCHAR)pvBlock1;
  dwCurrent = dwBytes1;
  while ( dwCurrent >= kBytesPerSample)
    {
    EmulatorClockTick( pFm, lpc);
    lpc += kBytesPerSample;
    dwCurrent -= kBytesPerSample;
    }
  if ( 0 == dwCurrent)
    {
    // Continue with second fragment
    dwCurrent = dwBytes2;
    lpc = (PUCHAR)pvBlock2;
    }
  else
    {
    // 1st buffer wasn't a whole no. samples
    dwCurrent = kBytesPerSample - dwCurrent;
    if ( dwBytes2 >= kBytesPerSample + dwCurrent)
      {
      // Skip the misaligned sample
      TRACE( 2, ("EmulatorOutput() IDirectSoundBuffer_Lock skipping over misaligned area 1\n"));
      lpc = (PUCHAR)pvBlock2 + dwCurrent;
      dwCurrent = dwBytes2 - dwCurrent;
      }
    else
      {
      // Ignore unfilled area at end
      dwCurrent = 0;
      }
    }

  while ( dwCurrent >= kBytesPerSample)
    {
    EmulatorClockTick( pFm, lpc);
    lpc += kBytesPerSample;
    dwCurrent -= kBytesPerSample;
    }
  if ( dwCurrent)
    TRACE( 2, ("EmulatorOutput() IDirectSoundBuffer_Lock area 2 not aligned\n"));
  EmulatorEndUpdate( pFm);

  // Update our write cursor
  pFm->dwWritten += dwBytes;
  if ( pFm->dwWritten >= pFm->dwBufferSize)
    pFm->dwWritten -= pFm->dwBufferSize;

  // Release the buffer
  hr = IDirectSoundBuffer_Unlock(
    pFm->pdsBuffer,
    pvBlock1, dwBytes1,
    pvBlock2, dwBytes2
  );
  ASSERT( SUCCEEDED( hr));

  TRACE( 3, ("EmulatorOutput() Written %lu samples\n", dwBytes / kBytesPerSample));

  return dwBytes;
  }


/*
 * Operators in parallel
 */
__inline int AdditiveSynthesis(
  register int const iCarrier,
  register int const iModulation
) {
#if 1
  return iCarrier + iModulation;
#else
  // Amplitude modulation
  // Op = 0.5 * (carrier + kdAmDepth * modulator * carrier)
  //    = (carrier * (kWaveMax + kdAmDepth * modulator)) / (2 * kWaveMax)
  return (int)((iCarrier * ((long)kWaveMax + iModulation)) / (2 * (int)kWaveMax));
#endif
  }


/*
 * Convert modulator amplitude to FM phase
 */
__inline long PhaseModulation(
  register SFm* const pFm,
  int const iModulation  // +/- kWaveMax
) {
  (void)pFm;
  // No allowance for 1/fm
  #define kdFmDepth 2 // 4Pi
  #define OPT_FB_MOD 8 // slot1 FB is >> OPT_FB_MOD - 7
  return ( ((long)SAMPLE_RATE( pFm) * iModulation) / kWaveMax)
    * ((long)(kdFmDepth * kWaveElements));
  }


/*
 * Generate basic rhythm
 */
__inline int RhythmBeat(
  register SFm* const pFm,
  size_t phase
) {
  // Channel 8 (HH/SD) sets pulse width
  BYTE ubFreq = pFm->aubRegister[ kRegFnum + 7];
  if ( ubFreq < 2)
    return 0;

  // TODO
  (void)phase;
  return 0;
  }


/*
 * Calc. operator slot output
 * NB This is the biggest CPU time consumer
 */
__inline int OperatorOutput(
  register SFm* const pFm,
  unsigned const uSlot
) {
  int const iSlotRegBase = s_iSlotArray[ uSlot];
  SOperator* const pOp = &pFm->op[ uSlot];
  SChannel* const pChnl = &pFm->chnl[ pOp->uChannel];
  register int iOutput;
  register int iAttenuation;
  register long lPhase;
  unsigned uMult;
  size_t phase;

  ASSERT( lengthof( s_iSlotArray) > uSlot);
  ASSERT( lengthof( pFm->op) > uSlot);
  ASSERT( lengthof( pFm->chnl) > pOp->uChannel);

  // Calc. total level attenuation
  uMult = pFm->aubRegister[ kRegLevel + iSlotRegBase];
  iAttenuation = (uMult & 0x3f) << 9; // TotalLevel 4.12 FP log2 dB

  // Add envelope attenuation
  iAttenuation += pOp->iAttenuation;

  // Add KSL attenuation
  switch ( uMult >> 6)
    {
  case 0: // No change
    break;
  case 2: // 3dB/oct
    iAttenuation += pChnl->iKslAttenuation / 2;
    break;
  case 1: // 1.5dB/oct
    iAttenuation += pChnl->iKslAttenuation / 4;
    break;
  case 3: // 6dB/oct
    iAttenuation += pChnl->iKslAttenuation;
    break;
    }

  // Add tremolo
  uMult = pFm->aubRegister[ kRegMult + iSlotRegBase];
  if ( 0x80 & uMult)
    iAttenuation += pFm->iTremoloAttenuation;

  if ( iAttenuation >= kMaxdB )
    return 0;

  // Calc. waveform phase from channel osc.
  lPhase = (long)((0x40 & uMult) ? pChnl->ulVibratoPhase : pChnl->ulPhase); // Vibrato
  lPhase *= s_auMultiplier[ uMult & 0xf];

  // Include phase modulation
  lPhase += pOp->lModulation;

  lPhase %= PHASE_MAX( pFm);
  if ( lPhase < 0)
    lPhase += PHASE_MAX( pFm);
  phase = (size_t)(lPhase / SAMPLE_RATE( pFm));
  ASSERT( kWaveElements > phase);

  // Read sample from waveform table
  if ( !pOp->bNoise)
    {
    unsigned uWs;
    if ( 0x20 & pFm->aubRegister[ kRegTest]) // WSE
      uWs = pFm->aubRegister[ kRegWs + iSlotRegBase] & 0x3;
    else if ( 1 & pFm->aubRegister[ kRegNew])
      uWs = pFm->aubRegister[ kRegWs + iSlotRegBase] & 0x7;
    else
      uWs = 0;

    switch ( uWs)
      {
    default:
      // Sine wave
      iOutput = s_knWaveArray1[ phase];
      break;
    case 1:
      // half sine
      iOutput = s_knWaveArray1[ phase];
      if ( iOutput < 0)
        iOutput = 0;
      break;
    case 2:
      // abs sine
      iOutput = s_knWaveArray1[ phase];
      if ( iOutput < 0)
        iOutput = -iOutput;
      break;
    case 3:
      // half abs
      phase %= kWaveElements / 2;
      if ( phase < kWaveElements / 4)
        iOutput = 0;
      else
        iOutput = -s_knWaveArray1[ phase];
      break;
    case 4:
      // half double
      if ( phase >= kWaveElements / 2)
        iOutput = 0;
      else if ( phase >= kWaveElements / 8)
        iOutput = s_knWaveArray1[ 2 * phase - kWaveElements / 4 ];
      else
        iOutput = s_knWaveArray1[ 2 * phase + (3 * kWaveElements) / 4 ];
      break;
    case 5:
      // half abs double
      if ( phase >= kWaveElements / 2)
        iOutput = 0;
      else if ( phase >= kWaveElements / 8)
        {
        iOutput = s_knWaveArray1[ 2 * phase - kWaveElements / 4 ];
        if ( iOutput < 0)
          iOutput = -iOutput;
        }
      else
        iOutput = s_knWaveArray1[ 2 * phase + (3 * kWaveElements) / 4 ];
      break;
    case 6:
      // Square wave
      iOutput = (phase < kWaveElements / 2) ? kWaveMax : -(int)kWaveMax;
      break;
    case 7:
      // Exponential decaying ramp
      if ( phase > kWaveElements / 2)
        lPhase = kWaveElements - phase;
      else
        lPhase = phase;
      lPhase = ((kMaxdB +1) * lPhase) / (kWaveElements / 2);
      iOutput = (int)((kWaveMax * (long)s_knPowerOf2Array[ lPhase % k6dB]) / kTwoScale);
      iOutput >>= lPhase / k6dB;
      if ( phase > kWaveElements / 2)
        iOutput = -iOutput;
      ASSERT( iOutput <= kWaveMax && iOutput >= -kWaveMax);
      break;
      }
    }
  // White noise source
  else switch ( uSlot)
    {
  default:
    ASSERT( !"Unexpected rhythm slot");
  case 16: // Snare drum
    iOutput = (rand() & 1) ? (kWaveMax/4) : -((int)kWaveMax/4);
    iOutput += ((phase % (kWaveElements / 2)) > kWaveElements / 4) ? (kWaveMax/2) : -((int)kWaveMax/2);
    break;
  case 13: // HH
    iOutput = (rand() & 1) ? (kWaveMax/4) : -((int)kWaveMax/4);
    iOutput += RhythmBeat( pFm, phase);
    break;
  case 17: // TC
    iOutput = RhythmBeat( pFm, phase);
    break;
    }

  // Add percussive attack
  if ( pOp->bBeat)
    {
    if ( phase >= (3 * kWaveElements) / 4)
      {
      pOp->bBeat = FALSE;
      }
    else if ( phase > kWaveElements / 2)
      {
      iOutput -= kWaveMax;
      }
    }

  // Apply attenuation
  if ( iOutput)
    {
    // Mantissa
    iOutput = ((long)iOutput * (int)s_knPowerOf2Array[ iAttenuation % k6dB]) / kTwoScale;
    // Exponent
    iOutput >>= iAttenuation / k6dB;
    }

  return iOutput;
  }


/*
 * Calc channel oscillator phase increment per sample clock
 */
__inline unsigned long FreqToPhase(
  register SFm* const pFm,
  unsigned const uChannel,
  int* const piKslAttenuation
) {
  unsigned const uChannelRegBase = s_unChannelArray[ uChannel];
  unsigned uFnum, uBlock;
  int iKsl;
      
  uBlock = pFm->aubRegister[ kRegBlock + uChannelRegBase];
  uFnum = (unsigned)pFm->aubRegister[ kRegFnum + uChannelRegBase] + ((uBlock & 0x03) << 8);
  uBlock = (uBlock >> 2) & 0x07;

  // Include KSL attenuation
  iKsl = (int)(uFnum / 64);
  if ( iKsl > (16 >> uBlock))
    *piKslAttenuation = iKsl * (k6dB >> (7 - uBlock));
  else
    *piKslAttenuation = 0;

  //freq = (uFnum * kAdlibFreq * s_auMultiplier[ uMult]) / (0x100000L >> uBlock);
  //phaseIncr = (unsigned long)(kWaveElements * freq);
  return (((unsigned long)uFnum * kAdlibFreq) / ((0x200000L / kWaveElements) >> uBlock)) % PHASE_MAX( pFm);
  }


/*
 * Emulate an FM synth clock tick
 */
void __fastcall EmulatorClockTick(
  register SFm* const pFm,
  unsigned char aucOutputBuffer[]
) {
  unsigned uChannel, uSlot, uIndex;
  int iLeft, iRight, iOutput;
  unsigned char ucMode, ucFeedback;
  int iOutputArray[ 4];
  SChannel* pChnl;

  TRACE( 9, ("EmulatorClockTick(0x%p)\n", pFm));
  ASSERT( NULL != pFm);
  ASSERT( NULL != aucOutputBuffer);

  // Run the envelope generator at a reduced rate to minimise overhead
  if ( 0 == (pFm->ulTicks++ % kAdsrStep))
    {
    // Update all the operator envelopes
    EmulatorEnvelope( pFm);
    }

  // Init output data
  iOutput = iLeft = iRight = 0;

  // Process 4op channels
  if ( 1 & pFm->aubRegister[ kRegNew])
    {
    // OPL3 mode so process all 4op channels
    for ( uChannel = 0; uChannel < lengthof( s_ConnectionArray); ++uChannel)
      {
      // Is this a 4op channel?
      unsigned uPrimary = s_ConnectionArray[ uChannel].chnl1;
      pChnl = &pFm->chnl[ uPrimary];
      if ( 4 == pChnl->nType)
        {
        // Yes it is...
        enum { kOperators = 4};
        unsigned char ucMode2;
        unsigned uSecondary = s_ConnectionArray[ uChannel].chnl2;
        ASSERT( 4 == pFm->chnl[ uSecondary].nType); // Both channels must be 4op
        
        // Test if muted
        ucMode = pFm->aubRegister[ kRegFb + s_unChannelArray[ uPrimary] ];
        if ( !(0x30 & ucMode))  // NB ignore CHC&D settings
          continue;

        // Re-calc oscillator phase increment if needed
        if ( ULONG_MAX == pChnl->ulPhaseIncr)
          {
          pChnl->ulPhaseIncr = FreqToPhase( pFm, uPrimary, &pChnl->iKslAttenuation);
          pChnl->ulVibratoIncr = pChnl->ulPhaseIncr;
          }

        // Bump oscillator phase counter
        if ( (pChnl->ulPhase += pChnl->ulPhaseIncr) >= PHASE_MAX( pFm))
          pChnl->ulPhase -= PHASE_MAX( pFm);
        if ( (pChnl->ulVibratoPhase += pChnl->ulVibratoIncr) >= PHASE_MAX( pFm))
          pChnl->ulVibratoPhase -= PHASE_MAX( pFm);

        // Calc. operator outputs
        for ( uIndex = 0; uIndex < kOperators; ++uIndex)
          {
          uSlot = s_4OpArray[ uChannel].slot[ uIndex];
          iOutputArray[ uIndex] = OperatorOutput( pFm, uSlot);
          }

        // Apply FM feedback to slot1
        uSlot = s_4OpArray[ uChannel].slot[ 0];
        ucFeedback = (unsigned char)((ucMode >> 1) & 0x7); // FB
        if ( !ucFeedback)
          pFm->op[ uSlot].lModulation = 0;
        else
          {
          // 7= 4PI, 6= 2PI, 5= PI etc
          iOutput = iOutputArray[ 0] >> (OPT_FB_MOD - ucFeedback);
          pFm->op[ uSlot].lModulation = PhaseModulation( pFm, iOutput);
          }

        // Determine operator inter-connection
        ucMode2 = pFm->aubRegister[ kRegFb + s_unChannelArray[ uSecondary] ];
        uIndex = ((ucMode2 & 1) << 1) | (ucMode & 1);
        switch ( uIndex)
          {
        default:
          ASSERT( !"Invalid 4op CNT");
        case 0:
          // 1 x 2 x 3 x 4
          uSlot = s_4OpArray[ uChannel].slot[ 1];
          pFm->op[ uSlot].lModulation = PhaseModulation( pFm, iOutputArray[ 0]);

          uSlot = s_4OpArray[ uChannel].slot[ 2];
          pFm->op[ uSlot].lModulation = PhaseModulation( pFm, iOutputArray[ 1]);

          uSlot = s_4OpArray[ uChannel].slot[ 3];
          pFm->op[ uSlot].lModulation = PhaseModulation( pFm, iOutputArray[ 2]);

          iOutput = iOutputArray[ 3];
          break;

        case 1:
          // 1 x 2 + 3 x 4
          uSlot = s_4OpArray[ uChannel].slot[ 1];
          pFm->op[ uSlot].lModulation = PhaseModulation( pFm, iOutputArray[ 0]);

          uSlot = s_4OpArray[ uChannel].slot[ 3];
          pFm->op[ uSlot].lModulation = PhaseModulation( pFm, iOutputArray[ 2]);

          iOutput = AdditiveSynthesis( iOutputArray[ 1], iOutputArray[ 3]);
          break;

        case 2:
          // 1 + 2 x 3 x 4
          uSlot = s_4OpArray[ uChannel].slot[ 2];
          pFm->op[ uSlot].lModulation = PhaseModulation( pFm, iOutputArray[ 1]);

          uSlot = s_4OpArray[ uChannel].slot[ 3];
          pFm->op[ uSlot].lModulation = PhaseModulation( pFm, iOutputArray[ 2]);

          iOutput = AdditiveSynthesis( iOutputArray[ 0], iOutputArray[ 3]);
          break;

        case 3:
          // 1 + 2 x 3 + 4
          uSlot = s_4OpArray[ uChannel].slot[ 2];
          pFm->op[ uSlot].lModulation = PhaseModulation( pFm, iOutputArray[ 1]);

          iOutput = AdditiveSynthesis( iOutputArray[ 0], iOutputArray[ 2]);
          iOutput = AdditiveSynthesis( iOutput, iOutputArray[ 3]);
          break;
          }

        // Sum the channel outputs
        if ( 0x20 & ucMode)
          iRight += iOutput;
        if ( 0x10 & ucMode)
          iLeft += iOutput;
        }
      }
    }

  // Process 2op channels
  // NEW enables OPL3 channels
  uChannel = (1 & pFm->aubRegister[ kRegNew]) ? lengthof( pFm->chnl) : lengthof( pFm->chnl) / 2;
  while ( uChannel-- > 0)
    {
    enum { kModulator, kCarrier, kOperators};
    
    // Is this a 2op channel?
    ASSERT( uChannel < lengthof( pFm->chnl));
    pChnl = &pFm->chnl[ uChannel];
    if ( 2 != pChnl->nType)
      continue; // No

    // Test if muted
    ucMode = pFm->aubRegister[ kRegFb + s_unChannelArray[ uChannel] ];
    if ( (1 & pFm->aubRegister[ kRegNew])
      && 0 == (0x30 & ucMode)           // NB ignore CHC&D settings
    ) {
      continue;
      }

    // Re-calc oscillator phase increment if needed
    if ( ULONG_MAX == pChnl->ulPhaseIncr)
      {
      pChnl->ulPhaseIncr = FreqToPhase( pFm, uChannel, &pChnl->iKslAttenuation);
      pChnl->ulVibratoIncr = pChnl->ulPhaseIncr;
      }

    // Bump oscillator phase counter
    if ( (pChnl->ulPhase += pChnl->ulPhaseIncr) >= PHASE_MAX( pFm))
      pChnl->ulPhase -= PHASE_MAX( pFm);
    if ( (pChnl->ulVibratoPhase += pChnl->ulVibratoIncr) >= PHASE_MAX( pFm))
      pChnl->ulVibratoPhase -= PHASE_MAX( pFm);

    // Calc. operator outputs
    for ( uIndex = 0; uIndex < kOperators; ++uIndex)
      {
      uSlot = s_2OpArray[ uChannel].slot[ uIndex];
      iOutputArray[ uIndex] = OperatorOutput( pFm, uSlot);
      }

    // Apply FM feedback to slot1
    uSlot = s_2OpArray[ uChannel].slot[ 0];
    ucFeedback = (unsigned char)((ucMode >> 1) & 0x7); // FB
    if ( !ucFeedback)
      pFm->op[ uSlot].lModulation = 0;
    else
      {
      // 7= 4PI, 6= 2PI, 5= PI etc
      iOutput = iOutputArray[ 0] >> (OPT_FB_MOD - ucFeedback);
      pFm->op[ uSlot].lModulation = PhaseModulation( pFm, iOutput);
      }

    // Select output mode
    switch ( uChannel)
      {
    case 6: // BD
    case 7: // HH & SD
    case 8: // TOM & TC
      if ( (0x20 & pFm->aubRegister[ kRegRhythm]))
        {
        // Rhythm mode
        if ( 6 != uChannel)
          {
          iOutput = iOutputArray[ kCarrier] + iOutputArray[ kModulator];
          }
        else
          {
          // Bass drum
          if ( ucMode & 1)              // CNT = 1
            {
            iOutput = iOutputArray[ kCarrier]; // Modulator has no o/p
            }
          else
            {
            // FM
            uSlot = s_2OpArray[ uChannel].slot[ kCarrier];
            pFm->op[ uSlot].lModulation = PhaseModulation( pFm, iOutputArray[ kModulator]);
            iOutput = iOutputArray[ kCarrier];
            }
          }
        break;
        }
      // Fall thru

    default:
      if ( ucMode & 1)                  // CNT = 1
        {
        iOutput = AdditiveSynthesis( iOutputArray[ kCarrier], iOutputArray[ kModulator]);
        }
      else
        {
        // FM
        uSlot = s_2OpArray[ uChannel].slot[ kCarrier];
        pFm->op[ uSlot].lModulation = PhaseModulation( pFm, iOutputArray[ kModulator]);
        iOutput = iOutputArray[ kCarrier];
        }
      break;
      }
    
    // Sum the channel outputs
    if ( !(1 & pFm->aubRegister[ kRegNew]))
      {
      iRight += iOutput;
      iLeft += iOutput;
      }
    else
      {
      if (0x20 & ucMode)
        iRight += iOutput;
      if ( 0x10 & ucMode)
        iLeft += iOutput;
      }
    }

  // Attenuate to prevent clipping & saturate
  iLeft /= 4;
  if ( iLeft > kWaveMax)
    iLeft = kWaveMax;
  else if ( iLeft < -kWaveMax)
    iLeft = -kWaveMax;

  iRight /= 4;
  if ( iRight > kWaveMax)
    iRight = kWaveMax;
  else if ( iRight < -kWaveMax)
    iRight = -kWaveMax;

  // Return the new output
#if kBitsPerChannel > 8
  ((short*)aucOutputBuffer)[ 0] = (short)iLeft;
  #if kChannels > 1
    ((short*)aucOutputBuffer)[ 1] = (short)iRight;
  #endif
#else
  aucOutputBuffer[ 0] = (unsigned char)(kSilence + (iLeft >> 8));
  #if kChannels > 1
    aucOutputBuffer[ 1] = (unsigned char)(kSilence + (iRight >> 8));
  #endif
#endif

  // Update LFO's at a reduced rate to minimise overhead
  if ( 0 == (pFm->ulTicks % kLfoStep))
    {
    // Update vibrato 6.1Hz osc
    pFm->ulVibratoPhase += (unsigned long)(kLfoStep * kWaveElements * 6.1);
    pFm->ulVibratoPhase %= PHASE_MAX( pFm);
    // Vibrato depth
    iOutput = (0x40 & pFm->aubRegister[ kRegRhythm]) ? 18 : 9; // 14% or 7% pitch bend
    uIndex = (size_t)(pFm->ulVibratoPhase / SAMPLE_RATE( pFm));
    iOutput = 1 + kWaveMax + (iOutput * s_knWaveArray1[ uIndex]) / 2048;

    uChannel = (1 & pFm->aubRegister[ kRegNew]) ? lengthof( pFm->chnl) : lengthof( pFm->chnl) / 2;
    while ( uChannel-- > 0)
      {
      // Re-calc oscillator phase increment
      pChnl = &pFm->chnl[ uChannel];
      pChnl->ulVibratoIncr = (unsigned long)(((__int64)iOutput * pChnl->ulPhaseIncr) / (1 + kWaveMax));
      }
  
    // Update tremolo 3.7Hz osc
    pFm->ulTremoloPhase += (unsigned long)(kLfoStep * kWaveElements * 3.7);
    pFm->ulTremoloPhase %= PHASE_MAX( pFm);
    // Tremolo depth: 
    iOutput = (0x80 & pFm->aubRegister[ kRegRhythm]) ? (k4dB8 / 2) : (k1dB / 2);
    uIndex = (size_t)(pFm->ulTremoloPhase / SAMPLE_RATE( pFm));
    pFm->iTremoloAttenuation = iOutput + (iOutput * s_knWaveArray1[ uIndex]) / (1 + kWaveMax);
    }
  }


/*
 * Process a change in KON for a given slot
 */
void __fastcall EmulatorKonEvent(
  register SFm* const pFm,
  unsigned uSlot,
  BOOL bKon
) {
  SOperator* const pOp = &pFm->op[ uSlot];
  unsigned uCount, uIndex;
  DWORD dwTicks;

  TRACE( 9, ("EmulatorKeyOn(0x%p,%u,%u)\n", pFm,uSlot,bKon));
  ASSERT( uSlot < lengthof( pFm->op));

  // Check for KON change
  bKon = (0 != bKon);
  if ( pOp->bKon == bKon)
    return;
  pOp->bKon = bKon;

  // Calc elapsed time from last envelope update
  dwTicks = 1 + (SAMPLE_RATE( pFm) * (timeGetTimeProc() - pFm->dwLastUpdate)) / (1000 * kAdsrStep);

  // Append to end of the event queue
  ASSERT( lengthof( pOp->eventArray) > pOp->uNextEvent);
  uIndex = pOp->uNextEvent;
  for ( uCount = 0; uCount < lengthof( pOp->eventArray); ++uCount)
    {
    // Entries are timed relative to previous
    DWORD dwTicksRemaining = pOp->eventArray[ uIndex].dwTicksRemaining;
    if ( 0 == dwTicksRemaining)
      {
      // Event is unused
      pOp->eventArray[ uIndex].dwTicksRemaining = dwTicks;
      pOp->eventArray[ uIndex].bKon = bKon;
      return;
      }

    // Deduct cycles remaining
    if ( dwTicksRemaining < dwTicks)
      dwTicks -= dwTicksRemaining;
    else
      dwTicks = 1;

    // Try next      
    if ( lengthof( pOp->eventArray) == ++uIndex)
      uIndex = 0;
    }
    
  TRACE( 1, ("*** EmulatorKonEvent overflow\n"));
  
  // Overwrite the last entry
  if ( uIndex)
    --uIndex;
  else
    uIndex = lengthof( pOp->eventArray) - 1;
  pOp->eventArray[ uIndex].bKon = bKon;
  }


/*
 * Update all slot envelopes
 */
void __fastcall EmulatorEnvelope(
  register SFm* const pFm
) {
  unsigned uSlot;

  TRACE( 9, ("EmulatorEnvelope(0x%p)\n", pFm));
  ASSERT( NULL != pFm);

  uSlot = (1 & pFm->aubRegister[ kRegNew]) ? lengthof( pFm->op)  : lengthof( pFm->op) / 2;
  while ( uSlot-- > 0)
    {
    register SOperator* const pOp = &pFm->op[ uSlot];
    int iChannelRegBase, iSlotRegBase;
    unsigned uRof, uRate;
    unsigned char ucMode;
    ldiv_t div;
    long lSamples;
    SAdsrEvent* pEvent;

    ASSERT( uSlot < lengthof( s_iSlotArray));
    iSlotRegBase = s_iSlotArray[ uSlot];
    ASSERT( pOp->uChannel < lengthof( s_unChannelArray));
    iChannelRegBase = s_unChannelArray[ pOp->uChannel];

    // Calc. key scale no. from BLOCK & FNUM MS 2bits
    ucMode = pFm->aubRegister[ kRegBlock + iChannelRegBase];
    uRof = (ucMode & 0x1e) >> 1; // Ignore FNUM MSB-1

    // Calc rate offset from key scale    
    if ( !(pFm->aubRegister[ kRegMult + iSlotRegBase] & 0x10))
      uRof /= 4; // KSR=0: rof= key scale / 4

    // Check for a queued KON event
    ASSERT( lengthof( pOp->eventArray) > pOp->uNextEvent);
    pEvent = &pOp->eventArray[ pOp->uNextEvent];
    if ( pEvent->dwTicksRemaining > 0
      && 0 == --(pEvent->dwTicksRemaining)
    ) {
      // Process the event
      if ( pEvent->bKon)
        {
        // Key on
        switch ( pOp->eAdsr)
          {
        case kAdsrRelease:
          pOp->eAdsr = kAdsrAttack;
          pOp->lAdsrCount = 0;
          pOp->bBeat = (0x20 & pFm->aubRegister[ kRegRhythm]); // Start rhythm beat
          break;
        default:
          break;
          }
        }
      else
        {
        // Key off
        switch ( pOp->eAdsr)
          {
        case kAdsrAttack:
        case kAdsrDecay:
        case kAdsrSustain:
          pOp->eAdsr = kAdsrRelease;
          pOp->lAdsrCount = 0;
          break;
        case kAdsrRelease:
          break;
          }
        }

      // Update queue head index
      if ( ++(pOp->uNextEvent) >= lengthof( pOp->eventArray))
        pOp->uNextEvent = 0;
      }

    // Update the envelope generator
    switch ( pOp->eAdsr)
      {
    case kAdsrAttack:
      uRate = pFm->aubRegister[ kRegAttack + iSlotRegBase] >> 4;
      if ( uRate)
        {
        uRate = k6dB * uRate + (k6dB / 4) * uRof;
        lSamples = (((2 * kAttackTime * (long)SAMPLE_RATE( pFm)) / (1000 * kAdsrStep))
          * (long)s_knPowerOf2Array[ uRate % k6dB]) / kTwoScale;
        lSamples >>= uRate / k6dB;
        if ( lSamples)
          {
          pOp->lAdsrCount += (k3dB / 2) + ((2784 * pOp->iAttenuation) >> 10); // * e
          div = ldiv( pOp->lAdsrCount, lSamples);
          pOp->lAdsrCount = div.rem;
          pOp->iAttenuation -= div.quot;
          }
        else
          pOp->iAttenuation = 0;
        }
      if ( pOp->iAttenuation <= 0)
        {
        // Enter decay state
        pOp->iAttenuation = 0;
        pOp->eAdsr = kAdsrDecay;
        pOp->lAdsrCount = 0;
        }
      break;

    case kAdsrDecay:
      {
      int iSustain;
      uRate = pFm->aubRegister[ kRegAttack + iSlotRegBase] & 0x0f;
      if ( uRate)
        {
        uRate = k6dB * uRate + (k6dB / 4) * uRof;
        lSamples = (((2 * kDecayTime * (long)SAMPLE_RATE( pFm)) / (1000 * kAdsrStep))
          * (long)s_knPowerOf2Array[ uRate % k6dB]) / kTwoScale;
        lSamples >>= uRate / k6dB;
        if ( lSamples)
          {
          pOp->lAdsrCount += k6dB;
          div = ldiv( pOp->lAdsrCount, lSamples);
          pOp->lAdsrCount = div.rem;
          pOp->iAttenuation += div.quot;
          if ( pOp->iAttenuation > kMaxdB)
            pOp->iAttenuation = kMaxdB;
          }
        else
          pOp->iAttenuation = kMaxdB;
        }

      iSustain = pFm->aubRegister[ kRegSustain + iSlotRegBase] & 0xf0;
      if ( iSustain == 0xf0)
        iSustain = k93dB;
      else
        iSustain <<= 7; // 4.12 FP log2 dB

      if ( pOp->iAttenuation >= iSustain)
        {
        // Enter sustain state
        pOp->iAttenuation = iSustain;
        pOp->eAdsr = kAdsrSustain;
        pOp->lAdsrCount = 0;
        }
      }
      break;

    case kAdsrSustain:
      if ( pFm->aubRegister[ kRegMult + iSlotRegBase] & 0x20) // EGT
        break; // Sustained

      // Fall thru to release but maintain state as sustain

    case kAdsrRelease:
      if ( pOp->iAttenuation < kMaxdB)
        {
        uRate = pFm->aubRegister[ kRegSustain + iSlotRegBase] & 0x0f;
        if ( uRate)
          {
          uRate = k6dB * uRate + (k6dB / 4) * uRof;
          lSamples = (((2 * kReleaseTime * (long)SAMPLE_RATE( pFm)) / (1000 * kAdsrStep))
            * (long)s_knPowerOf2Array[ uRate % k6dB]) / kTwoScale;
          lSamples >>= uRate / k6dB;
          if ( lSamples)
            {
            pOp->lAdsrCount += k6dB;
            div = ldiv( pOp->lAdsrCount, lSamples);
            pOp->lAdsrCount = div.rem;
            pOp->iAttenuation += div.quot;
            if ( pOp->iAttenuation > kMaxdB)
              pOp->iAttenuation = kMaxdB;
            }
          else
            pOp->iAttenuation = kMaxdB;
          }
        }
      break;
      }
    }
  }


/*
 * End of emulator update period.
 * Start a new envelope timing period
 */
void __fastcall EmulatorEndUpdate(
  register SFm* const pFm
) {
  unsigned uSlot;

  TRACE( 9, ("EmulatorEndUpdate(0x%p)\n", pFm));
  ASSERT( NULL != pFm);

  // Mark all pending KON events for immediate action
  uSlot = (1 & pFm->aubRegister[ kRegNew]) ? lengthof( pFm->op)  : lengthof( pFm->op) / 2;
  while ( uSlot-- > 0)
    {
    SOperator* const pOp = &pFm->op[ uSlot];
    unsigned uIndex;

    ASSERT( lengthof( pOp->eventArray) > pOp->uNextEvent);
    uIndex = pOp->uNextEvent;
    {
    unsigned u;
    for ( u = 0; u < lengthof( pOp->eventArray); ++u)
      {
      if ( 0 == pOp->eventArray[ uIndex].dwTicksRemaining)
        break;
      pOp->eventArray[ uIndex].dwTicksRemaining = 1;
      if ( lengthof( pOp->eventArray) == ++uIndex)
        uIndex = 0;
      }
    }
    }

  pFm->dwLastUpdate = timeGetTimeProc();
  }

#endif // OPT_ADLIB_EMULATOR

/* End of file */
