/*
 * Code for the lousy Soundblaster 2.0 emulation
 * of NTVDM starting with Windows XP
 *
 * This is more meant for completeness to reconstruct
 * Windows XP NTVDM than it is useful, better use
 * SoundFX 2000:
 *
 * http://www.softsystem.co.uk/products/soundfx.htm
 */

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <mmsystem.h>
#include <vdm.h>
#include "insignia.h"
#include "host_def.h"
#include "xt.h"
#include "nt_reset.h"
#include "nt_sbpriv.h"
#include "nt_sbdsp.h"
#include "nt_sbfm.h"
#include "nt_sbmid.h"
#include "nt_sbmxr.h"
#include "ios.h"
#include "stdio.h"
#include "stdarg.h"

#define SB_ADAPTOR		SPARE_ADAPTER1

/*
 *   Symbolic names for port addresses
 */
#define MIXER_ADDRESS		0x04
#define MIXER_DATA			0x05
#define RESET_PORT          0x06
#define FM_STATUS			0x08
#define FM_DATA				0x09
#define READ_DATA           0x0A
#define WRITE_PORT          0x0C        // Data or command
#define WRITE_STATUS        0x0C
#define READ_STATUS         0x0E

/*
 * MPU 401 commands
 */
#define MPU401_RESET        0xFF
#define MPU401_ENTER_UART   0x3F
#define MPU401_ACK			0xFE
#define MPU401_DSR			0x80

/*
 * ==========================================================================
 * Global data
 * ==========================================================================
 */

GLOBAL BOOL bHighSpeedMode = FALSE;
GLOBAL io_addr BasePort;
GLOBAL io_addr MpuBasePort;
GLOBAL WORD SbInterrupt;
GLOBAL WORD SbDmaChannel;

/*
 * ==========================================================================
 * Local data
 * ==========================================================================
 */
IMPORT HMIDIOUT HMidiOut;

LOCAL USHORT DisplayFlags = 0;
LOCAL BOOL bSBAttached = FALSE;
LOCAL BOOL bDevicesActive = FALSE;
LOCAL BYTE MpuInData = 0;
LOCAL HANDLE hWinmm = NULL;
LOCAL enum {
    IntelligentMode = 0,
    UARTMode
}
MpuMode = IntelligentMode;

/*
 * ==========================================================================
 * Local function declarations
 * ==========================================================================
 */
LOCAL BOOL LoadWinmm(void);
LOCAL BOOL InitializeIoAddresses();
LOCAL BOOL InitDevices();
LOCAL BOOL ConnectPorts(io_addr start, io_addr end);
LOCAL BOOL InstallIoHook();
LOCAL VOID DeInstallIoHook();
LOCAL VOID VsbByteIn(io_addr port, half_word *data);
LOCAL VOID VsbByteOut(io_addr port, half_word data);


#if DBG
GLOBAL int DebugLevel = SBDEBUG_ERR;
LOCAL int DebugCount = 0;
#define DEBUG_START 0


/***************************************************************************

 Generate debug output in printf type format

 ****************************************************************************/

GLOBAL VOID DbgOut(LPSTR lpszFormat, ...)
{
    char buf[256];
    va_list va;

    /* BUGBUG: What if this overflows? Present in original XP NTVDM source,
     * Removed it, as it makes no sense to me
    if (++DebugCount < DEBUG_START) {
        return;
    }
     */
    OutputDebugStringA("VSB: ");

    va_start(va, lpszFormat);
    vsprintf(buf, lpszFormat, va);
    va_end(va);

    OutputDebugStringA(buf);
    OutputDebugStringA("\r\n");
}
#endif // DBG


/*
 * ==========================================================================
 * Global functions
 * ==========================================================================
 */
GLOBAL BOOL SbInitialize()
{
    bSBAttached = FALSE;
    if (VDMForWOW || !InitializeIoAddresses())
        return FALSE;
    if (!InstallIoHook())
    {
        dprintf2(("*** failed to install IO Hooks!!!"));
        return FALSE;
    }
    if (!LoadWinmm())
    {
        DeInstallIoHook();
        return FALSE;
    }
    bSBAttached = TRUE;
    return TRUE;
}

GLOBAL VOID SbReinitialize(char *lpBlasterEnv, UINT cbBlasterEnv)
{
    UINT  i = 0;
    DWORD addr;
    io_addr Base, MpuBase;

    if (!VDMForWOW && lpBlasterEnv && cbBlasterEnv)
    {
        Base = BasePort;
        MpuBase = MpuBasePort;
        if (cbBlasterEnv > 0 && lpBlasterEnv[0])
        {
            while (i<cbBlasterEnv)
            {
                while (i<cbBlasterEnv && lpBlasterEnv[i]==' ') i++;
                if (i>=cbBlasterEnv) break;
                switch (lpBlasterEnv[i])
                {
                case 'a':
                case 'A':
                    if (sscanf(&lpBlasterEnv[++i], "%x", &addr) == 1)
                        Base = addr;
                    break;
                case 'd':
                case 'D':
                    if (sscanf(&lpBlasterEnv[++i], "%x", &addr) == 1)
                        SbDmaChannel = addr;
                    break;
                case 'i':
                case 'I':
                    if (sscanf(&lpBlasterEnv[++i], "%x", &addr) == 1)
                        SbInterrupt = addr;
                    break;
                case 'p':
                case 'P':
                    if (sscanf(&lpBlasterEnv[++i], "%x", &addr) == 1)
                        MpuBase = addr;
                    break;
                }
                while (i<cbBlasterEnv && lpBlasterEnv[i]!=' ') i++;
            }
        }
        dprintf3(("Base %x, DMA %x, INT %x, MPU %x", Base, SbDmaChannel, SbInterrupt, MpuBase));
        if (Base)
        {
            if (Base != BasePort || MpuBase != MpuBasePort)
            {
                if (bSBAttached)
                {
                    SbCloseDevices();
                    DeInstallIoHook();
                    BasePort = Base;
                    MpuBasePort = MpuBase;
                    if (!InstallIoHook())
                    {
                        FreeLibrary(hWinmm);
                        bSBAttached = FALSE;
                    }
                }
            }
        }
        else
        {
            SbTerminate();
        }
    }
}

GLOBAL VOID SbCloseDevices()
{
#ifndef W10
    VDM_PM_CLI_DATA ControlData;

    ControlData.Control = PM_CLI_CONTROL_DISABLE;
    NtVdmControl(VdmPMCliControl, &ControlData);
#endif
    if (bDevicesActive)
    {
        DetachMidi();
        CloseFMDevice();
        CleanUpWave();
        bDevicesActive = FALSE;
    }
}

GLOBAL VOID SbTerminate()
{
    if (bSBAttached)
    {
        SbCloseDevices();
        DeInstallIoHook();
        FreeLibrary(hWinmm);
    }
    bSBAttached = FALSE;
}

GLOBAL VOID ResetAll()
{
    dprintf3(("Resetting"));
    ResetFM();
    ResetMixer();
    ResetDSP();
}


GLOBAL VOID DisplaySbMode(USHORT mode)
{
    if (IsDebuggerPresent() && (mode & DisplayFlags))
    {
        switch (mode)
        {
        case SBMODE_CYCLE:
            DbgOut("VSB: SINGLE CYCLE mode\n");
            break;
        case SBMODE_HICYCLE:
            DbgOut("VSB: HIGH SPEED SINGLE CYCLE mode\n");
            break;
        case SBMODE_AUTOINIT:
            DbgOut("VSB: AUTO-INIT mode\n");
            break;
        case SBMODE_HIAUTOINIT:
            DbgOut("VSB: HIGH SPEED AUTO-INIT mode\n");
            break;
        case SBMODE_MIDI:
            DbgOut("VSB: MIDI mode\n");
            break;
        case SBMODE_MIXER:
            DbgOut("VSB: MIXER mode\n");
            break;
        case SBMODE_FM:
            DbgOut("VSB: ADLIB/FM mode\n");
            break;
        }
        DisplayFlags = ~mode;
    }
}


/*
 * ==========================================================================
 * Local functions
 * ==========================================================================
 */

LOCAL BOOL LoadWinmm(void)
{
    hWinmm = LoadLibraryA("WINMM.DLL");
    if (hWinmm)
    {
        SetVolumeProc = (tSetVolumeProc)GetProcAddress(hWinmm, "waveOutSetVolume");
        GetVolumeProc = (tGetVolumeProc)GetProcAddress(hWinmm, "waveOutGetVolume");
        GetNumDevsProc = (tGetNumDevsProc)GetProcAddress(hWinmm, "waveOutGetNumDevs");
        GetDevCapsProc = (tGetDevCapsProc)GetProcAddress(hWinmm, "waveOutGetDevCapsA");
        OpenProc = (tOpenProc)GetProcAddress(hWinmm, "waveOutOpen");
        PauseProc = (tPauseProc)GetProcAddress(hWinmm, "waveOutPause");
        RestartProc = (tRestartProc)GetProcAddress(hWinmm, "waveOutRestart");
        ResetProc = (tResetProc)GetProcAddress(hWinmm, "waveOutReset");
        CloseProc = (tCloseProc)GetProcAddress(hWinmm, "waveOutClose");
        GetPositionProc = (tGetPositionProc)GetProcAddress(hWinmm, "waveOutGetPosition");
        WriteProc = (tWriteProc)GetProcAddress(hWinmm, "waveOutWrite");
        PrepareHeaderProc = (tPrepareHeaderProc)GetProcAddress(hWinmm, "waveOutPrepareHeader");
        UnprepareHeaderProc = (tUnprepareHeaderProc)GetProcAddress(hWinmm, "waveOutUnprepareHeader");
        SetMidiVolumeProc = (tSetMidiVolumeProc)GetProcAddress(hWinmm, "midiOutSetVolume");
        GetMidiVolumeProc = (tGetMidiVolumeProc)GetProcAddress(hWinmm, "midiOutGetVolume");
        MidiGetNumDevsProc = (tMidiGetNumDevsProc)GetProcAddress(hWinmm, "midiOutGetNumDevs");
        MidiGetDevCapsProc = (tMidiGetDevCapsProc)GetProcAddress(hWinmm, "midiOutGetDevCapsA");
        MidiOpenProc = (tMidiOpenProc)GetProcAddress(hWinmm, "midiOutOpen");
        MidiResetProc = (tMidiResetProc)GetProcAddress(hWinmm, "midiOutReset");
        MidiCloseProc = (tMidiCloseProc)GetProcAddress(hWinmm, "midiOutClose");
        MidiLongMsgProc = (tMidiLongMsgProc)GetProcAddress(hWinmm, "midiOutLongMsg");
        MidiShortMsgProc = (tMidiShortMsgProc)GetProcAddress(hWinmm, "midiOutShortMsg");
        MidiPrepareHeaderProc = (tMidiPrepareHeaderProc)GetProcAddress(hWinmm, "midiOutPrepareHeader");
        MidiUnprepareHeaderProc = (tMidiUnprepareHeaderProc)GetProcAddress(hWinmm, "midiOutUnprepareHeader");

        if (SetVolumeProc
                && GetVolumeProc
                && GetNumDevsProc
                && GetDevCapsProc
                && OpenProc
                && ResetProc
                && CloseProc
                && GetPositionProc
                && WriteProc
                && PrepareHeaderProc
                && SetMidiVolumeProc
                && UnprepareHeaderProc
                && MidiGetNumDevsProc
                && MidiGetDevCapsProc
                && MidiOpenProc
                && MidiResetProc
                && MidiCloseProc
                && MidiLongMsgProc
                && MidiShortMsgProc
                && MidiPrepareHeaderProc
                && MidiUnprepareHeaderProc
                && GetMidiVolumeProc)
        {
            return TRUE;
        }
        dprintf2(("Can not get all the MM api entries"));
    }
    return FALSE;
}

LOCAL BOOL InitializeIoAddresses()
{
    BasePort = SB_BASE_PORT;
    MpuBasePort = MPU_BASE_PORT;
    SbInterrupt = 5;
    SbDmaChannel = 1;
    return TRUE;
}

LOCAL BOOL InitDevices()
{
    io_addr port;
#ifndef W10
    VDM_PM_CLI_DATA ControlData;

    ControlData.Control = PM_CLI_CONTROL_ENABLE;
    NtVdmControl(VdmPMCliControl, &ControlData);
#endif
    if (PrepareWaveInitialization() && FindWaveDevice())
    {
        if (!InitializeMidi())
        {
            if (MpuBasePort)
            {
                for (port = MpuBasePort; port <= MpuBasePort + 1; port++)
                    io_disconnect_port(port++, SB_ADAPTOR);
                MpuBasePort = 0;
            }
            dprintf2(("Unable to Initialize MIDI resources"));
        }
        bDevicesActive = TRUE;
        OpenFMDevice();
        return TRUE;
    }
    return FALSE;
}

LOCAL BOOL ConnectPorts(io_addr start, io_addr end)
{
    io_addr port = start;
    if (start > end) return TRUE;
    while (io_connect_port(port, SB_ADAPTOR, IO_READ_WRITE))
        if (++port > end)
            return TRUE;
    while (start <= port)
        io_disconnect_port(start++, SB_ADAPTOR);
    return FALSE;
}

LOCAL BOOL InstallIoHook()
{
    io_addr port;

    io_define_inb(SB_ADAPTOR, VsbByteIn);
    io_define_outb(SB_ADAPTOR, VsbByteOut);
    if (ConnectPorts(BasePort + 4, BasePort + 6))
    {
        if (ConnectPorts(BasePort + 8, BasePort + 10))
        {
            if (ConnectPorts(BasePort + 12, BasePort + 12))
            {
                if (ConnectPorts(BasePort + 14, BasePort + 14))
                {
                    if (ConnectPorts(OPL_BASE, OPL_DATA))
                    {
                        if (MpuBasePort && !ConnectPorts(MpuBasePort, MpuBasePort + 1))
                            MpuBasePort = 0;
                        return TRUE;
                    }
                    for (port = BasePort + 14; port <= BasePort + 14; port++)
                        io_disconnect_port(port, SB_ADAPTOR);
                }
                for (port = BasePort + 12; port <= BasePort + 12; port++)
                    io_disconnect_port(port, SB_ADAPTOR);
            }
            for (port = BasePort + 8; port <= BasePort + 10; port++)
                io_disconnect_port(port, SB_ADAPTOR);
        }
        for (port = BasePort + 4; port <= BasePort + 6; port++)
            io_disconnect_port(port, SB_ADAPTOR);
    }
    return FALSE;
}

LOCAL VOID DeInstallIoHook()
{
    io_addr port;

    for (port = BasePort + 4; port <= BasePort + 6; port++)
        io_disconnect_port(port, SB_ADAPTOR);

    for (port = BasePort + 8; port <= BasePort + 10; port++)
        io_disconnect_port(port, SB_ADAPTOR);

    for (port = BasePort + 12; port <= BasePort + 12; port++)
        io_disconnect_port(port, SB_ADAPTOR);

    for (port = BasePort + 14; port <= BasePort + 14; port++)
        io_disconnect_port(port, SB_ADAPTOR);

    for (port = OPL_BASE; port <= OPL_DATA; port++)
        io_disconnect_port(port, SB_ADAPTOR);

    if (MpuBasePort)
    {
        for (port = MpuBasePort; port <= MpuBasePort + 1; port++)
            io_disconnect_port(port++, SB_ADAPTOR);
    }
}

LOCAL VOID VsbByteIn(io_addr port, half_word *data)
{
    *data = 0xFF;

    if (bDevicesActive || InitDevices())
    {
        switch (port - BasePort)
        {
        case MIXER_DATA:
            MixerDataRead(data);
            break;

        case FM_STATUS:
            port = OPL_BASE;
            break;

        case READ_DATA:
            /*
             *  The only useful things they can read are :
             *     0xAA after RESET
             *
             *     The DSP version
             */

            DspReadData(data);
            dprintf1(("r-RdDt %x", *data));
            break;

        case WRITE_STATUS:

            /*
             *  Can always write
             */

            *data = bHighSpeedMode ? 0xFF : 0x7F;
            dprintf1(("r-WtSta %x", *data));
            break;

        case READ_STATUS:
            /*
             *  See if we think there is something to read
             */

            DspReadStatus(data);
            dprintf1(("r-RdSta %x", *data));
            break;
        }

        /* BUGBUG: Original XP SB2.0 uses hardcoded 0x300 and 0x330 address
         * for MPU here and ignores setting from BLASTER env. Doesn't make
         * sense for me, so fixed here
         */
        if (port == MpuBasePort)
        {
            *data = MpuInData;
            MpuInData = 0xFF;
            dprintf1(("r-MPU RdDt %x", *data));
        }
        else if (port == MpuBasePort + 1)
        {
            *data = MpuInData != 0xFF ? 0 : MPU401_DSR;
            dprintf1(("r-MPU RdSt %x", *data));
        }
        else if (port == OPL_BASE)
            FMStatusRead(data);

        dprintf5(("Read  %4X, <= %2X", port, *data));
    }
}

LOCAL VOID VsbByteOut(io_addr port, half_word data)
{
    if (!bDevicesActive && !InitDevices())
        return;

    dprintf5(("Write %4X, => %2X", port, data));

    switch (port - BasePort)
    {
    case MIXER_ADDRESS:
        DisplaySbMode(SBMODE_MIXER);
        MixerAddrWrite(data);
        break;

    case MIXER_DATA:
        MixerDataWrite(data);
        break;

    case RESET_PORT:
        dprintf1(("w-Reset %x", data));
        DspResetWrite(data);
        break;

    case FM_STATUS:
        port = OPL_BASE;
        break;

    case FM_DATA:
        port = OPL_DATA;
        break;

    case WRITE_PORT:
        if (!bHighSpeedMode)
        {
            dprintf1(("w-wt %x", data));
            DspWrite(data);
        }
        break;
    }


    /* BUGBUG: Original XP SB2.0 uses hardcoded 0x300 and 0x330 address
     * for MPU here and ignores setting from BLASTER env. Doesn't make
     * sense to me, so fixed here
     */
    if (port == MpuBasePort)
    {
        dprintf1(("w-MPU wtDt %x", data));
        switch (MpuMode)
        {
        case IntelligentMode:
            dprintf2(("App sending MPU data when in INTELLIGENT mode!  Data dumped!"));
            break;
        case UARTMode:
            if (HMidiOut) BufferMidi(data);
            break;
        default:
            dprintf2(("Invalid MPU mode!"));
            break;
        }
        return;
    }
    else if (port == MpuBasePort + 1)
    {
        DisplaySbMode(SBMODE_MIDI);
        dprintf1(("r-MPU wtCmd %x", data));
        if (data == MPU401_RESET || data == MPU401_ENTER_UART)
            MpuInData = MPU401_ACK;
        switch (MpuMode)
        {
        case IntelligentMode:
            switch (data)
            {
            case MPU401_ENTER_UART:
                DisplaySbMode(SBMODE_MIDI);
                dprintf3(("Switching MPU to UART (dumb) mode."));
                if (!HMidiOut)
                    OpenMidiDevice(0);
                MpuMode = UARTMode;
                break;

            case MPU401_RESET:
                dprintf3(("Reseting MPU while in intelligent mode."));
                break;

            default:
                dprintf3(("Unknown MPU401 command 0x%x sent while in intelligent mode!", data));
                break;
            }
            break;

        case UARTMode:
            if (data == MPU401_RESET)
            {
                dprintf3(("App Reseting MPU while in UART mode, switching to intelligent mode."));
                MpuMode = IntelligentMode;
                if (HMidiOut)
                    CloseMidiDevice();
                dprintf3(("MPU Reset done."));
            }
            break;
        default:
            dprintf2(("Invalid MPU mode!"));
            break;
        }
        return;
    }

    switch (port)
    {
    case OPL_BASE:
        DisplaySbMode(SBMODE_FM);
        FMRegisterSelect(data);
        break;

    case OPL_DATA:
        FMDataWrite(data);
        break;
    }
}
