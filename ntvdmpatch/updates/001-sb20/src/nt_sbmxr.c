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
 * Module: CT1335 Mixer
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
#include "nt_sbmxr.h"

#define VOL_LEVEL(x)   (0x10001 * (((((DWORD)x >> 1) & 7) << 13) - 1))	/* 0..7, Step 4 dB */
#define VOICE_LEVEL(x) (0x10001 * (((((DWORD)x >> 1) & 3) << 14) - 1))  /* 0..3, Step 7 dB */

/*
 * ==========================================================================
 * Local Data
 * ==========================================================================
 */

IMPORT HWAVEOUT HWaveOut;
IMPORT HMIDIOUT HFM, HMidiOut;

LOCAL enum {
    MixerReset = 1,
    MasterVolume,
    MidiVolume,
    CdVolume,
    VoiceVolume
} MixerState = MixerReset;

LOCAL struct {
    BYTE Master;
    BYTE Midi;
    BYTE Cd;
    BYTE Voice;
} MixerSettings = {0};

/*
 * ==========================================================================
 * Global functions
 * ==========================================================================
 */

GLOBAL VOID MixerAddrWrite(half_word value)
{
    switch (value)
    {
    case 0x02:
        MixerState = MasterVolume;
        break;
    case 0x06:
        MixerState = MidiVolume;
        break;
    case 0x08:
        MixerState = CdVolume;
        break;
    case 0x0A:
        MixerState = VoiceVolume;
        break;
    default:
        MixerState = MixerReset;
        break;
    }
}

GLOBAL VOID MixerDataRead(half_word *value)
{

    switch (MixerState)
    {
    case MixerReset:
        ResetMixer();
        break;
    case MasterVolume:
        *value = MixerSettings.Master;
        break;
    case MidiVolume:
        *value = MixerSettings.Midi;
        break;
    case CdVolume:
        *value = MixerSettings.Cd;
        break;
    case VoiceVolume:
        *value = MixerSettings.Voice;
        break;
    }
}

GLOBAL VOID MixerDataWrite(half_word value)
{
    switch (MixerState)
    {
    case MixerReset:
        ResetMixer();
        break;
    case MasterVolume:
        MixerSetMasterVolume(value);
        break;
    case MidiVolume:
        MixerSetMidiVolume(value);
        break;
    case CdVolume:
        MixerSettings.Cd = value;
        break;
    case VoiceVolume:
        MixerSetVoiceVolume(value);
        break;
    }
}

GLOBAL VOID ResetMixer()
{
    MixerSetMasterVolume(8);
    MixerSetMidiVolume(8);
    MixerSettings.Cd = 0;
    MixerSetVoiceVolume(4);
    MixerState = MixerReset;
}

GLOBAL VOID MixerSetMasterVolume(half_word vol)
{
    MixerSettings.Master = vol;
    if (HWaveOut) SetVolumeProc(HWaveOut, VOL_LEVEL(vol));
}

GLOBAL VOID MixerSetMidiVolume(half_word vol)
{
    MixerSettings.Midi = vol;
    if (HFM || HMidiOut != HFM)
    {
        if (HFM) SetMidiVolumeProc(HFM, VOL_LEVEL(vol));
        if (HMidiOut) SetMidiVolumeProc(HMidiOut, VOL_LEVEL(vol));
    }
}

GLOBAL VOID MixerSetVoiceVolume(half_word vol)
{
    MixerSettings.Voice = vol;
    if (HWaveOut) SetVolumeProc(HWaveOut, VOICE_LEVEL(vol));
}

