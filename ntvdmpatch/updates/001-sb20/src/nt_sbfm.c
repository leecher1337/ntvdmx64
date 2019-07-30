/*
 * Code for the lousy Soundblaster 2.0 emulation
 * of NTVDM starting with Windows XP
 *
 * This is more meant for completeness to reconstruct
 * Windows XP NTVDM than it is useful, better use
 * SoundFX 2000:
 *
 * http://www.softsystem.co.uk/products/soundfx.htm
 *
 * NOTE: This relies on Kernel mode AdLib Emulation
 * which is NOT present in x64 versions of Windows, so
 * this won't work there.
 */

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <mmsystem.h>
#include "../../../../inc/vdm.h"
#include "insignia.h"
#include "host_def.h"
#include "xt.h"
#include "nt_sbpriv.h"
#include "nt_sbfm.h"
#include "nt_sbmxr.h"

/* AdLib registers */
#define AL_TIMER1		0x02
#define AL_TIMER2		0x03
#define AL_TISTATE		0x04

#define AL_TI1STARTSTOP	0x01
#define AL_TI2STARTSTOP	0x02
#define AL_TI2MASK		0x20
#define AL_TI1MASK		0x40
#define AL_TIRESET		0x80

#define AL_CHANLO		0xA0	/* + channel number */
#define AL_CHANHI		0xB0	/* + channel number */

/*
 * ==========================================================================
 * Global data
 * ==========================================================================
 */

GLOBAL HMIDIOUT HFM = NULL;

/*
 * ==========================================================================
 * Local Data
 * ==========================================================================
 */

IMPORT io_addr BasePort;

LOCAL BOOL FMActive = FALSE;
LOCAL DWORD Position;
LOCAL half_word Status, AdlibRegister;
LOCAL BOOL Timer1Started, Timer2Started;
LOCAL DWORD PortData[64];

/*
 * ==========================================================================
 * Global functions
 * ==========================================================================
 */

GLOBAL BOOL OpenFMDevice()
{
    VDM_ADLIB_DATA ControlData;
    UINT uDeviceID, res;

    ControlData.VirtualPortEnd = BasePort + 9;
    ControlData.VirtualPortStart = BasePort + 8;
    ControlData.PhysicalPortStart = OPL_BASE;
    ControlData.PhysicalPortEnd = OPL_DATA;
    FMActive = FALSE;
    HFM = NULL;

    if (FindFMSynthDevice(&uDeviceID))
    {
        res = MidiOpenProc(&HFM, uDeviceID, 0, 0, 0);
        if (res == MMSYSERR_NOERROR)
        {
            MixerSetMidiVolume(8);
#ifndef W10
            ControlData.Action = ADLIB_DIRECT_IO;
            NtVdmControl(VdmAdlibEmulation, &ControlData);
#endif
            dprintf3(("Direct IO access for Adlib"));
        }
        else
        {
            dprintf2(("Failed to open FM Synth device - code %d", res));
            HFM = NULL;
        }
    }
    if (!HFM)
    {
#ifndef W10
        ControlData.Action = ADLIB_KERNEL_EMULATION;
        NtVdmControl(VdmAdlibEmulation, &ControlData);
#endif
        dprintf3(("Kernel mode emulation for Adlib"));
    }
    FMActive = TRUE;
    return TRUE;
}

GLOBAL VOID CloseFMDevice()
{
    VDM_ADLIB_DATA ControlData;

    dprintf3(("Closing FM device"));
    if (HFM)
    {
        MidiResetProc(HFM);
        MidiCloseProc(HFM);
        HFM = NULL;
    }
    if (FMActive)
    {
#ifndef W10
        ControlData.Action = ADLIB_USER_EMULATION;
        ControlData.VirtualPortEnd = BasePort + 9;
        ControlData.VirtualPortStart = BasePort + 8;
        ControlData.PhysicalPortStart = OPL_BASE;
        ControlData.PhysicalPortEnd = OPL_DATA;
        NtVdmControl(VdmAdlibEmulation, &ControlData);
#endif
        FMActive = FALSE;
    }
}

GLOBAL VOID ResetFM()
{
    AdlibRegister = 0;
    Position = 0;
    Timer1Started = FALSE;
    Timer2Started = FALSE;
    Status = 6;
}

GLOBAL VOID FMStatusRead(half_word *value)
{
    *value = Status;
}

GLOBAL VOID FMRegisterSelect(half_word value)
{
    AdlibRegister = value;
}

GLOBAL VOID FMDataWrite(half_word value)
{
    if (AdlibRegister == 0x105)	/* BUG? Can never be true??  */
        value &= 0xFEu;
    if (Position > 38)
    {
        dprintf2(("Attempting to write beyond end of PortData array"));
    }
    else
    {
        PortData[Position++] = MAKELONG(OPL_BASE, AdlibRegister);
        PortData[Position++] = MAKELONG(OPL_DATA, value);
    }
    if (Position == 40 || (AdlibRegister >= AL_CHANHI && AdlibRegister <= AL_CHANHI + 0x0D) || AdlibRegister == AL_TISTATE)
    {
        if (FMPortWrite())
        {
            /* Mostly debugging stuff without any use..? */
            if (AdlibRegister == AL_TISTATE)
            {
                if (value & 0x80)
                    Status = 0;
                if (!(value & AL_TI1STARTSTOP) || (Status & AL_TI1MASK))
                {
                    Timer1Started = FALSE;
                }
                else
                {
                    dprintf3(("Timer 1 started"));
                    Status |= 0xC0u;
                    Timer1Started = TRUE; /* BUG? Missing in original NTVDM source */
                }
                if (!(value & AL_TI2STARTSTOP) || (Status & AL_TI2MASK))
                {
                    Timer2Started = FALSE;
                }
                else
                {
                    dprintf3(("Timer 2 started"));
                    Status |= 0xA0u;
                    Timer2Started = TRUE;
                }
            }
        }
        else dprintf2(("Failed to write to device!"));
    }
}

/*
 * ==========================================================================
 * Local functions
 * ==========================================================================
 */

LOCAL BOOL FindFMSynthDevice(UINT *puDeviceId)
{
    UINT i, nDevices;
    MIDIOUTCAPS MidiOutCaps;

    nDevices = MidiGetNumDevsProc();
    if (nDevices)
    {
        for (i=0; i<nDevices; i++)
        {
            if (MidiGetDevCapsProc(i, &MidiOutCaps, sizeof(MidiOutCaps)) == MMSYSERR_NOERROR &&
                    MidiOutCaps.wTechnology == MOD_FMSYNTH)
            {
                *puDeviceId = i;
                return TRUE;
            }
        }
    }
    dprintf2(("FM Synth device not found"));
    return FALSE;
}

LOCAL BOOL FMPortWrite()
{
    LPSTR Buffer;
    DWORD NumberOfBytesWritten = 0;

    if (FMActive)
    {
        dprintf5(("Writing %d bytes of data to port", sizeof(PortData[0]) * Position));
        if (!WriteFile(HFM, PortData, sizeof(PortData[0]) * Position, &NumberOfBytesWritten, 0))
        {
            FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 0, GetLastError(),
                          LANG_USER_DEFAULT, (LPSTR)&Buffer, 0, 0);
            dprintf2(("FM write failed: %d bytes of data written, error message: %s", NumberOfBytesWritten, Buffer));
            LocalFree(Buffer);
            return FALSE;
        }
    }

    Position = 0;
    return TRUE;
}

