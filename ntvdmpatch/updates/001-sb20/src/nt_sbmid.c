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
#include "insignia.h"
#include "host_def.h"
#include "xt.h"
#include "nt_sbpriv.h"
#include "nt_sbmid.h"

#define		BUF_SIZE		0x1000

/*
 * ==========================================================================
 * Global data
 * ==========================================================================
 */

GLOBAL HMIDIOUT HMidiOut = NULL;

/*
 * ==========================================================================
 * Local Data
 * ==========================================================================
 */

LOCAL PBYTE MidiBuffer;
LOCAL LPBYTE MessageBuffer;
LOCAL DWORD LastData, NextData, BytesLeft, LastCommand, LastCommandLength, RunningStatus, NextCopyPosition, Alignment;
LOCAL BOOL MidiInitialized = FALSE;
LOCAL MIDIHDR *MidiHdrs[4];
LOCAL DWORD OriginalMidiVol = 0, PreviousMidiVol = 0;

enum MidiType
{
    InvalidType           = 0x00,    ///< For notifying errors
    NoteOff               = 0x80,    ///< Note Off
    NoteOn                = 0x90,    ///< Note On
    AfterTouchPoly        = 0xA0,    ///< Polyphonic AfterTouch
    ControlChange         = 0xB0,    ///< Control Change / Channel Mode
    ProgramChange         = 0xC0,    ///< Program Change
    AfterTouchChannel     = 0xD0,    ///< Channel (monophonic) AfterTouch
    PitchBend             = 0xE0,    ///< Pitch Bend
    SystemExclusive       = 0xF0,    ///< System Exclusive
    TimeCodeQuarterFrame  = 0xF1,    ///< System Common - MIDI Time Code Quarter Frame
    SongPosition          = 0xF2,    ///< System Common - Song Position Pointer
    SongSelect            = 0xF3,    ///< System Common - Song Select
    TuneRequest           = 0xF6,    ///< System Common - Tune Request
    Clock                 = 0xF8,    ///< System Real Time - Timing Clock
    Start                 = 0xFA,    ///< System Real Time - Start
    Continue              = 0xFB,    ///< System Real Time - Continue
    Stop                  = 0xFC,    ///< System Real Time - Stop
    ActiveSensing         = 0xFE,    ///< System Real Time - Active Sensing
    SystemReset           = 0xFF,    ///< System Real Time - System Reset
};

/*
 * ==========================================================================
 * Local function declarations
 * ==========================================================================
 */

LOCAL VOID ResetMidiDevice();
LOCAL VOID SendMidiRequest();
LOCAL DWORD MidiCopyMessages(LPSTR lpData);

/*
 * ==========================================================================
 * Global functions
 * ==========================================================================
 */

GLOBAL BOOL InitializeMidi()
{
    MidiBuffer = VirtualAlloc(NULL, BUF_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!MidiBuffer)
    {
        dprintf2(("Unable to allocate MidiBuffer memory"));
        return FALSE;
    }
    MessageBuffer = VirtualAlloc(NULL, 0x600u, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!MessageBuffer)
    {
        dprintf2(("Unable to allocate MessageBuffer memory"));
        VirtualFree(MidiBuffer, 0, MEM_RELEASE);
        return FALSE;
    }
    OpenMidiDevice(0);
    if (HMidiOut)
    {
        LastData = 0;
        NextData = 0;
        BytesLeft = 0;
        LastCommand = 0;
        LastCommandLength = 1;
        RunningStatus = 0;
        NextCopyPosition = 0;
        Alignment = 7;
        MidiInitialized = TRUE;
        return TRUE;
    }
    else
    {
        if (MidiBuffer)
        {
            VirtualFree(MidiBuffer, 0, MEM_RELEASE);
            MidiBuffer = NULL;
        }
        if (MessageBuffer)
        {
            VirtualFree(MessageBuffer, 0, MEM_RELEASE);
            MessageBuffer = NULL;
        }
        CloseMidiDevice();
    }
    return FALSE;
}

GLOBAL BOOL OpenMidiDevice(DWORD dwCallback)
{
    int i;
    BYTE *Buffer;
    UINT res;

    for (i=0, Buffer = MessageBuffer; i<sizeof(MidiHdrs)/sizeof(MidiHdrs[0]); i++)
    {
        MidiHdrs[i] = Buffer;
        MidiHdrs[i]->lpData = Buffer + sizeof(MIDIHDR);
        MidiHdrs[i]->dwBufferLength = 320;
        MidiHdrs[i]->dwUser = 0;
        MidiHdrs[i]->dwFlags = 0;
        Buffer += MidiHdrs[i]->dwBufferLength + sizeof(MIDIHDR);
    }

    res = MidiOpenProc(&HMidiOut, -1, dwCallback, 0, dwCallback?CALLBACK_EVENT:0);
    if (res == MMSYSERR_NOERROR)
    {
        if (!HMidiOut)
            return TRUE;
        for (i=0; i<sizeof(MidiHdrs)/sizeof(MidiHdrs[0]); i++)
        {
            if (MidiPrepareHeaderProc(HMidiOut, MidiHdrs[i], sizeof(MIDIHDR)) != MMSYSERR_NOERROR)
            {
                dprintf2(("Prepare MIDI hdr failed"));
                MidiCloseProc(HMidiOut);
                HMidiOut = NULL;
                return FALSE;
            }
        }
        GetMidiVolumeProc(HMidiOut, &OriginalMidiVol);
        PreviousMidiVol = OriginalMidiVol;
        return TRUE;
    }
    else
    {
        dprintf2(("Failed to open MIDI device - code %d", res));
    }

    return FALSE;
}

GLOBAL VOID CloseMidiDevice()
{
    DWORD dwVolume = 0;
    int i;

    dprintf3(("Closing MIDI device"));
    if (MidiInitialized)
        ResetMidiDevice();
    if (HMidiOut)
    {
        if (GetMidiVolumeProc(HMidiOut, &dwVolume) && dwVolume == PreviousMidiVol)
            SetMidiVolumeProc(HMidiOut, OriginalMidiVol);

        for (i=0; i<sizeof(MidiHdrs)/sizeof(MidiHdrs[0]); i++)
        {
            if (MidiUnprepareHeaderProc(HMidiOut, MidiHdrs[i], sizeof(MIDIHDR)) != MMSYSERR_NOERROR)
                dprintf2(("Unprepare MIDI hdr failed"));
        }
        if (MidiCloseProc(HMidiOut) != MMSYSERR_NOERROR)
            dprintf2(("Unable to close MIDI out device"));
        HMidiOut = NULL;
    }
    dprintf3(("Midi Closed"));
}

GLOBAL VOID BufferMidi(BYTE data)
{
    if (BytesLeft)
    {
        if (data < 0xF8)
        {
            if (data < 0xF0)
            {
                if (data >= 0x80)
                {
                    dprintf2(("Got a new command before previous command completed!"));
                    dprintf2(("Truncating previous command!"));
                    *(DWORD *)&MidiBuffer[LastCommand] -= BytesLeft;
                    BytesLeft = 0;
                }
            }
            else if (RunningStatus == 0xF0 && data == 0xF7)
            {
                dprintf4(("Sysex stop!"));
                MidiBuffer[NextData] = 0xF7;
                --BytesLeft;
                NextData = (NextData + 1) % BUF_SIZE;
                *(DWORD *)&MidiBuffer[LastCommand] -= BytesLeft;
                RunningStatus = 0;
                BytesLeft = 0;
                SendMidiRequest();
                return;
            }
            else
            {
                dprintf4(("Got a system common message before previous command completed!"));
                dprintf4(("Truncating previous command!"));
                *(DWORD *)&MidiBuffer[LastCommand] -= BytesLeft;
                BytesLeft = 0;
            }
        }
        else
        {
            if (RunningStatus != 0xF0)
            {
                DWORD i, j;

                dprintf4(("Realtime system message inside a non sysex message!"));
                dprintf4(("Creating a new message."));
                i = NextData;
                j = Alignment + NextData + 6;
                NextData = j % BUF_SIZE;
                while (i != LastCommand)
                {
                    j = (j - 1) % BUF_SIZE;
                    i = (i - 1) % BUF_SIZE;
                    MidiBuffer[j] = MidiBuffer[i];
                }
                *(DWORD*)&MidiBuffer[i] = 1;
                MidiBuffer[(i + sizeof(DWORD)) % BUF_SIZE] = data;
                LastCommand = (~Alignment & (Alignment + LastCommand + 5)) % BUF_SIZE;
                return;
            }
            dprintf4(("Realtime system message inside a sysex message!"));
            *(DWORD *)&MidiBuffer[LastCommand] -= BytesLeft;
            BytesLeft = 0;
        }
    }
    if (!BytesLeft)
    {
        if (data >= 0xF0u)
        {
            if (data < 0xF8)
            {
                RunningStatus = 0;
                LastCommandLength = 0;
            }
            switch (data)
            {
            case 0xF0:
                dprintf4(("Sysex start!"));
                BytesLeft = 0x80;
                RunningStatus = 0xF0;
                LastCommandLength = 0x80;
                break;
            case 0xF1:
            case 0xF3:
                BytesLeft = 2;
                break;
            case 0xF2:
                BytesLeft = 3;
                break;
            case 0xF4:
            case 0xF5:
                dprintf2(("Received undefined system common message 0x%x!", data));
            default:
                BytesLeft = 1;
                break;
            }
        }
        else if (data >= 0x80u)
        {
            RunningStatus = data;
            if (data < 0xC0 || data > 0xDF)
                BytesLeft = 3;
            else
                BytesLeft = 2;
            LastCommandLength = BytesLeft;
        }
        else if (RunningStatus)
        {
            dprintf4(("Using running status 0x%x!", RunningStatus));
            BytesLeft = LastCommandLength - 1;
        }
        else
        {
            dprintf2(("Received data 0x%x without running status.  Dropping!", data));
            return;
        }
        NextData = (~Alignment & (Alignment + NextData)) % BUF_SIZE;
        LastCommand = NextData;
        *(DWORD *)&MidiBuffer[NextData] = BytesLeft;
        NextData = (NextData + sizeof(DWORD)) % BUF_SIZE;
    }
    MidiBuffer[NextData] = data;
    NextData = (NextData + 1) % BUF_SIZE;
    if (BytesLeft-- == 1) SendMidiRequest();
    return;
}

GLOBAL VOID DetachMidi()
{
    dprintf3(("Detach MIDI"));
    if (MidiInitialized)
    {
        CloseMidiDevice();
        VirtualFree(MidiBuffer, 0, MEM_RELEASE);
        VirtualFree(MessageBuffer, 0, MEM_RELEASE);
        MidiInitialized = FALSE;
    }
}

/*
 * ==========================================================================
 * Local functions
 * ==========================================================================
 */

LOCAL VOID ResetMidiDevice()
{
    int i;

    dprintf3(("Resetting MIDI device"));
    if (HMidiOut)
    {
        i = 0;
        do
        {
            if (MidiHdrs[i]->dwFlags & MHDR_INQUEUE)
                Sleep(5000);
            else
                ++i;
        }
        while (i < sizeof(MidiHdrs)/sizeof(MidiHdrs[0]));
        if (MidiResetProc(HMidiOut) != MMSYSERR_NOERROR)
            dprintf2(("Unable to reset MIDI out device"));
    }
    dprintf3(("MIDI device reset"));
}


LOCAL VOID SendMidiRequest()
{
    DWORD i, j;

    if (HMidiOut)
    {
        for (i = NextCopyPosition; i != LastCommand; i = (~Alignment & (Alignment + NextCopyPosition + 8)) % BUF_SIZE)
        {
            if (*(DWORD *)&MidiBuffer[i] > 3)
            {
                for (j=0; j<sizeof(MidiHdrs)/sizeof(MidiHdrs[0]) && (MidiHdrs[j]->dwFlags & MHDR_INQUEUE); j++);
                if (j == sizeof(MidiHdrs)/sizeof(MidiHdrs[0]))
                {
                    dprintf3(("midi: No Midi header available"));
                }
                else
                {
                    dprintf4(("Midi data received"));
                    MidiHdrs[j]->dwBytesRecorded = MidiCopyMessages(MidiHdrs[j]->lpData);
                    dprintf3(("send MIDI data to driver %x", MidiHdrs[j]->dwBytesRecorded));
                    MidiLongMsgProc(HMidiOut, MidiHdrs[j], sizeof(MIDIHDR));
                }
                return;
            }
            MidiShortMsgProc(HMidiOut, *(DWORD*)&MidiBuffer[i + sizeof(DWORD)]);
            NextCopyPosition = (~Alignment & (Alignment + NextCopyPosition + 8)) % BUF_SIZE;
        }
    }
}

LOCAL DWORD MidiCopyMessages(LPSTR lpData)
{
    DWORD i, j;

    dprintf4(("Midi: copy messages"));
    for (i=0; i<0xC0 && NextCopyPosition != LastCommand;)
    {
        j = *(DWORD*)&MidiBuffer[NextCopyPosition];
        i += j;
        NextCopyPosition = (NextCopyPosition + sizeof(DWORD)) % BUF_SIZE;
        for (; j>0; j--)
        {
            *lpData++ = MidiBuffer[NextCopyPosition];
            NextCopyPosition = (NextCopyPosition + 1) % BUF_SIZE;
        }
        NextCopyPosition = (~Alignment & (Alignment + NextCopyPosition)) % BUF_SIZE;
    }
    return i;
}
