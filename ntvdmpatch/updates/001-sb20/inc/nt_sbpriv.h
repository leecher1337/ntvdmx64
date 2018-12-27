#include "nt_sb.h"

#if DBG
IMPORT int DebugLevel;

    #define SBDEBUG_CMD		1		// Debug commands (Read/Write)
    #define SBDEBUG_ERR		2		// Debug errors
    #define SBDEBUG_FUN		4		// Debug function calls
    #define SBDEBUG_INFO	8		// Debug informative messages
    #define SBDEBUG_PORT	16		// Debug port read/write


    #define dprintf( _x_ )                          DbgOut _x_
    #define dprintf1( _x_ ) if (DebugLevel & SBDEBUG_CMD)  DbgOut _x_
    #define dprintf2( _x_ ) if (DebugLevel & SBDEBUG_ERR)  DbgOut _x_
    #define dprintf3( _x_ ) if (DebugLevel & SBDEBUG_FUN)  DbgOut _x_
    #define dprintf4( _x_ ) if (DebugLevel & SBDEBUG_INFO) DbgOut _x_
	#define dprintf5( _x_ ) if (DebugLevel & SBDEBUG_PORT) DbgOut _x_

	GLOBAL VOID DbgOut(LPSTR lpszFormat, ...);

#else

    #define dprintf(x)
    #define dprintf1(x)
    #define dprintf2(x)
    #define dprintf3(x)
    #define dprintf4(x)
	#define dprintf5(x)
    #define DbgOut(x)

#endif // DBG 


/*
 * Mode of soundcard emulator 
 */
#define SBMODE_CYCLE		0x01
#define SBMODE_HICYCLE		0x02
#define SBMODE_AUTOINIT		0x04
#define SBMODE_HIAUTOINIT	0x08
#define SBMODE_MIDI			0x10
#define SBMODE_MIXER		0x20
#define SBMODE_FM			0x40


/* 
 * The well-known common port addresses 
 */
#define SB_BASE_PORT		0x220	// default DB base port
#define	MPU_BASE_PORT		0x330	// default MPU401 base port
#define OPL_BASE			0x388	// OPL base port
#define OPL_DATA			0x389	// OPL data port

/*
 * WinMM Prototypes
 */
typedef WINMMAPI MMRESULT (*tSetVolumeProc)(HWAVEOUT hwo, DWORD dwVolume);
typedef WINMMAPI MMRESULT (*tGetVolumeProc)(HWAVEOUT hwo, LPDWORD pdwVolume);
typedef WINMMAPI UINT     (*tGetNumDevsProc)(VOID);
typedef WINMMAPI MMRESULT (*tGetDevCapsProc)(UINT uDeviceID, LPWAVEOUTCAPS pwoc, UINT cbwoc);
typedef WINMMAPI MMRESULT (*tOpenProc)(LPHWAVEOUT phwo, UINT uDeviceID, LPWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwCallbackInstance, DWORD fdwOpen);
typedef WINMMAPI MMRESULT (*tPauseProc)(HWAVEOUT hwo);
typedef WINMMAPI MMRESULT (*tRestartProc)(HWAVEOUT hwo);
typedef WINMMAPI MMRESULT (*tResetProc)(HWAVEOUT hwo);
typedef WINMMAPI MMRESULT (*tCloseProc)(HWAVEOUT hwo);
typedef WINMMAPI MMRESULT (*tGetPositionProc)(HWAVEOUT hwo, LPMMTIME pmmt, UINT cbmmt);
typedef WINMMAPI MMRESULT (*tWriteProc)(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
typedef WINMMAPI MMRESULT (*tPrepareHeaderProc)(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
typedef WINMMAPI MMRESULT (*tUnprepareHeaderProc)(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
typedef WINMMAPI MMRESULT (*tSetMidiVolumeProc)(HMIDIOUT hmo, DWORD dwVolume);
typedef WINMMAPI MMRESULT (*tGetMidiVolumeProc)(HMIDIOUT hmo, LPDWORD lpdwVolume);
typedef WINMMAPI UINT     (*tMidiGetNumDevsProc)(VOID);
typedef WINMMAPI MMRESULT (*tMidiGetDevCapsProc)(UINT uDeviceID, LPMIDIOUTCAPS lpMidiOutCaps, UINT cbMidiOutCaps);
typedef WINMMAPI UINT     (*tMidiOpenProc)(LPHMIDIOUT lphmo, UINT uDeviceID, DWORD dwCallback, DWORD dwCallbackInstance, DWORD dwFlags);
typedef WINMMAPI MMRESULT (*tMidiResetProc)(HMIDIOUT hmo);
typedef WINMMAPI MMRESULT (*tMidiCloseProc)(HMIDIOUT hmo);
typedef WINMMAPI MMRESULT (*tMidiLongMsgProc)(HMIDIOUT hmo, LPMIDIHDR lpMidiOutHdr, UINT cbMidiOutHdr);
typedef WINMMAPI MMRESULT (*tMidiShortMsgProc)(HMIDIOUT hmo, DWORD dwMsg);
typedef WINMMAPI MMRESULT (*tMidiPrepareHeaderProc)(HMIDIOUT hmo, LPMIDIHDR lpMidiOutHdr, UINT cbMidiOutHdr);
typedef WINMMAPI MMRESULT (*tMidiUnprepareHeaderProc)(HMIDIOUT hmo, LPMIDIHDR lpMidiOutHdr, UINT cbMidiOutHdr);

/*
 * WinMM functions
 */
tSetVolumeProc SetVolumeProc;
tGetVolumeProc GetVolumeProc;
tGetNumDevsProc GetNumDevsProc;
tGetDevCapsProc GetDevCapsProc;
tOpenProc OpenProc;
tPauseProc PauseProc;
tRestartProc RestartProc;
tResetProc ResetProc;
tCloseProc CloseProc;
tGetPositionProc GetPositionProc;
tWriteProc WriteProc;
tPrepareHeaderProc PrepareHeaderProc;
tUnprepareHeaderProc UnprepareHeaderProc;
tSetMidiVolumeProc SetMidiVolumeProc;
tGetMidiVolumeProc GetMidiVolumeProc;
tMidiGetNumDevsProc MidiGetNumDevsProc;
tMidiGetDevCapsProc MidiGetDevCapsProc;
tMidiOpenProc MidiOpenProc;
tMidiResetProc MidiResetProc;
tMidiCloseProc MidiCloseProc;
tMidiLongMsgProc MidiLongMsgProc;
tMidiShortMsgProc MidiShortMsgProc;
tMidiPrepareHeaderProc MidiPrepareHeaderProc;
tMidiUnprepareHeaderProc MidiUnprepareHeaderProc;

/*
 * Common nt_sb.c functions
 */
VOID ResetAll();
VOID DisplaySbMode(USHORT mode);
VOID DspWrite(BYTE data);
