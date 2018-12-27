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
#include "softpc.h"
#include "xt.h"
#include "dma.h"
#include "vdm.h"
#include "nt_vdd.h"
#include "nt_eoi.h"
#include "nt_sbpriv.h"
#include "nt_sbdsp.h"

#define SB_VERSION          0x201   // We pretend to be DSP version 2.1

/*
 *   DSP commands
 */
#define DSP_CARD_IDENTIFY   0xE0    // Doing card identification
#define DSP_GET_VERSION     0xE1    // dsp version command
#define DSP_SPEAKER_ON      0xD1    // speaker on command
#define DSP_SPEAKER_OFF     0xD3    // speaker off command
#define DSP_SET_SAMPLE_RATE 0x40    // set the sample rate
#define DSP_SET_BLOCK_SIZE  0x48    // set dma block size
#define DSP_WRITE           0x14    // Start non-auto DMA
#define DSP_WRITE_AUTO      0x1C    // auto init output mode
#define DSP_READ            0x24    // Start non-auto read
#define DSP_READ_AUTO       0x2C    // auto init mode input
#define DSP_HALT_DMA        0xD0    // stop dma
#define DSP_CONTINUE_DMA    0xD4    // continue halted dma
#define DSP_STOP_AUTO       0xDA    // exit from auto init mode
#define DSP_MIDI_READ       0x31    // Interrupt driver midi input
#define DSP_MIDI_READ_UART  0x35    // Interrupt driver midi input (uart mode)
#define DSP_MIDI_TS_READ    0x37    // Midi time-stamped read
#define DSP_MIDI_WRITE      0x38    // Midi output
#define DSP_GENERATE_INT    0xF2    // Special code to generate a interrupt
#define DSP_DIRECT_WAVE_OUT 0x10    // polled output
#define DSP_HI_WRITE_AUTO   0x90    // 8 bit high speed auto init dma output
#define DSP_HI_WRITE_SINGLE 0x91    // 8 bit high speed single cycle output
#define DSP_TABLE_MUNGE     0xE2    // table munge
#define DSP_RESREG_WRITE    0xE4    // write reserved register
#define DSP_RESREG_READ     0xE8    // read reserved register

#define WAVE_FMT_SEARCH	(WAVE_FORMAT_4M08 | WAVE_FORMAT_1M08)

/*
 * ==========================================================================
 * Global data
 * ==========================================================================
 */

GLOBAL HANDLE HWaveOut = NULL;

/*
 * ==========================================================================
 * Local Data
 * ==========================================================================
 */

IMPORT WORD SbInterrupt, SbDmaChannel;
IMPORT BOOL bHighSpeedMode;

LOCAL UINT WaveOutDevice;
LOCAL DWORD OriginalWaveVol, StartingWaveOutPos, PreviousWaveVol;
LOCAL DWORD PreviousWaveOutPos, NextWaveOutPos, PageSize, BurstSize, SBBlockSize;
LOCAL CRITICAL_SECTION DspLock;
LOCAL USHORT Chan = 0, StartingDmaAddr, StartingDmaCount;
LOCAL BOOL SbAnswerDMAPosition = FALSE;
LOCAL DMA_ADAPT *pDmaAdp = NULL;
LOCAL DMA_CNTRL *pDcp = NULL;
LOCAL HANDLE DspWaveSem, ThreadStarted, ThreadFinished, DspResetEvent, DspResetDone, DspWavePlayed;
LOCAL DWORD DspNextRead, DspNextWrite, DspBufferSize, DspBufferTotalBursts, DspNumberDmaQueries, DspVirtualInc, DspVirtualIncx;
LOCAL DWORD *WaveBlockSizes = NULL, TimeConstant = 166, EndingDmaValue;
LOCAL PBYTE DspBuffer = NULL;
LOCAL BYTE IdentByte, ReservedRegister;
LOCAL BOOL bDspActive, bDspPaused, bWriteBurstStarted, bDspReset, bExitDMAThread, bDSPActive, bExitAuto;
LOCAL WAVEHDR *WaveHdrs;
LOCAL PCMWAVEFORMAT WaveFormat = { { WAVE_FORMAT_PCM, 1, 0, 0, 1 }, 8};
LOCAL VDD_DMA_INFO DmaInfo;

/*
 *  State machines
 */

LOCAL enum {
    ResetNotStarted = 1,
    Reset1Written
}
ResetState = ResetNotStarted;

LOCAL enum {
    WriteCommand = 1,         // Initial state and after RESET
    CardIdent,
    TableMunge,
    RsvdRegWrite,
    SetTimeConstant,
    BlockSizeFirstByte,
    BlockSizeSecondByte,
    BlockSizeFirstByteWrite,
    BlockSizeSecondByteWrite,
    BlockSizeFirstByteRead,
    BlockSizeSecondByteRead,
    MidiWrite
}
DSPWriteState = WriteCommand;

LOCAL enum {
    NothingToRead = 1,
    Reset,
    FirstVersionByte,
    SecondVersionByte,
    ReadIdent,
    ReadRsvdReg,
    SpeakerStatus,
    DirectWaveIn,
    MidiRead
}
DSPReadState = NothingToRead;

LOCAL enum {
    AutoInitMode = 0,
    CycleMode = 1,
    NoMode = 2
}
DspMode = NoMode;

/*
 * ==========================================================================
 * Local function declarations
 * ==========================================================================
 */

LOCAL BOOL GenerateHdrs(DWORD size);
LOCAL BOOL GetWaveOutPosition(DWORD *pos);
LOCAL BOOL WaitOnWaveOutIdle();
LOCAL VOID PrepareHeaders();
LOCAL VOID UnprepareHeaders();
LOCAL BOOL OpenWaveDevice(DWORD dwCallback);
LOCAL VOID CloseWaveDevice();
LOCAL VOID ResetWaveDevice();
LOCAL VOID AutoInitEoiHook(int IrqLine, int CallCount);
LOCAL VOID ExitAutoMode();
LOCAL BOOL QueryDMA(VDD_DMA_INFO *pDmaInfo);
LOCAL VOID DmaDataToDsp(DWORD mode);
LOCAL VOID WriteCommandByte(BYTE command);
LOCAL DWORD GetSamplingRate(void);
LOCAL VOID GenerateInterrupt(int delay);
LOCAL PBYTE GetDMATransferAddress();
LOCAL VOID SetDMAStatus(VDD_DMA_INFO *pDmaInfo, BOOL bRequesting, BOOL bTerminalCntRchd);
LOCAL BOOL SetDMACountAddr(VDD_DMA_INFO *pDmaInfo);
LOCAL VOID SetSpeaker(BOOL bOn);
LOCAL VOID SetWaveOutVolume(DWORD NewWaveVolume);
LOCAL VOID PauseDMA();
LOCAL BOOL TestWaveFormat(DWORD SampleRate);
LOCAL VOID TableMunger(BYTE data);
LOCAL VOID DspProcessBlock(BOOL bCycle);
LOCAL DWORD DspThreadEntry(LPVOID lpThreadParameter);
LOCAL BOOL SetWaveFormat();
LOCAL BOOL StartDspDmaThread(BOOL bCycle);
LOCAL VOID StopDspDmaThread(BOOL bWait);

/*
 * ==========================================================================
 * Global functions
 * ==========================================================================
 */

GLOBAL BOOL FindWaveDevice()
{
    UINT uNumDevs = GetNumDevsProc();
    UINT uDeviceID;
    WAVEOUTCAPS caps;

    if (uNumDevs)
    {
        for (uDeviceID = 0; GetDevCapsProc(uDeviceID, &caps, sizeof(caps)) == MMSYSERR_NOERROR && uDeviceID < uNumDevs; uDeviceID++)
        {
            if ((caps.dwFormats & WAVE_FMT_SEARCH) == WAVE_FMT_SEARCH)
            {
                WaveOutDevice = uDeviceID;
                return TRUE;
            }
        }
    }
    dprintf2(("Wave device not found"));
    return FALSE;
}

GLOBAL BOOL PrepareWaveInitialization()
{
    SYSTEM_INFO SystemInfo;

    if (!PageSize)
    {
        GetSystemInfo(&SystemInfo);
        PageSize = SystemInfo.dwPageSize;
        InitializeCriticalSection(&DspLock);

        pDmaAdp = dmaGetAdaptor();
        pDcp    = &pDmaAdp->controller[dma_physical_controller(SbDmaChannel)];
        Chan    = dma_physical_channel(SbDmaChannel);
    }
    BurstSize = 512;
    if (GenerateHdrs(0x3000))
    {
        DspWaveSem = CreateSemaphore(NULL, 0, 100, NULL);
        ThreadStarted = CreateEvent(NULL, FALSE, FALSE, NULL);
        ThreadFinished = CreateEvent(NULL, FALSE, FALSE, NULL);
        DspResetEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        DspResetDone = CreateEvent(NULL, FALSE, FALSE, NULL);
        DspWavePlayed = CreateEvent(NULL, FALSE, FALSE, NULL);
        return TRUE;
    }
    return FALSE;
}

GLOBAL VOID DspReadData(BYTE *data)
{
    switch (DSPReadState)
    {
    case NothingToRead:
        *data = 0xFF;
        break;

    case Reset:
        *data = 0xAAu;
        DSPReadState = NothingToRead;
        dprintf1(("rd Reset"));
        break;

    case FirstVersionByte:
        *data = SB_VERSION / 256;
        DSPReadState = SecondVersionByte;
        dprintf1(("rd 1st Version"));
        break;

    case SecondVersionByte:
        *data = SB_VERSION % 256;
        DSPReadState = NothingToRead;
        dprintf1(("rd 2nd Version"));
        break;

    case ReadIdent:
        *data = ~IdentByte;
        DSPReadState = NothingToRead;
        dprintf1(("rd Id"));
        break;

    case ReadRsvdReg:
        *data = ReservedRegister;
        DSPReadState = NothingToRead;
        dprintf1(("rd RsvdReg"));
        break;

    default:
        dprintf2(("Unrecognized read state"));
        break;
    }
}

GLOBAL BOOL DspResetWrite(BYTE data)
{
    if (data == 1)
    {
        ResetState = Reset1Written;
    }
    else if (ResetState == Reset1Written && data == 0)
    {
        ResetState = ResetNotStarted;

        /*
         *  OK - reset everything
         */

        bDspReset = TRUE;
        if (!bHighSpeedMode)
            bExitDMAThread = TRUE;
        return ResetAll();
    }
    return TRUE;
}

GLOBAL VOID ResetDSP()
{
    if (bDspActive)
    {
        SetEvent(DspResetEvent);
        if (bExitDMAThread) WaitForSingleObject(DspResetDone, INFINITE);
    }
    if (bExitDMAThread)
    {
        StopDspDmaThread(1);
        CloseHandle(DspWaveSem);
        DspWaveSem = CreateSemaphore(NULL, 0, 100, NULL);
        ResetEvent(ThreadStarted);
        ResetEvent(ThreadFinished);
        ResetEvent(DspResetEvent);
        HWaveOut = NULL;
        TimeConstant = 256 - 1000000 / 11025;
        WaveFormat.wf.nSamplesPerSec = 0;
        WaveFormat.wf.nAvgBytesPerSec = 0;
        bDspActive = FALSE;
        bExitDMAThread = FALSE;
    }
    DspMode = NoMode;
    SBBlockSize = 0x7FF;
    DspNextRead = DspBufferTotalBursts - 1;
    DspNextWrite = 0;
    bDspPaused = FALSE;
    bDspReset = FALSE;
    ResetEvent(DspResetDone);
    ResetEvent(DspWavePlayed);
    NextWaveOutPos = 0;
    SbAnswerDMAPosition = FALSE;
    bHighSpeedMode = 0;
    DSPReadState = Reset;
    DSPWriteState = WriteCommand;
}

GLOBAL VOID DspWrite(BYTE data)
{
    static DWORD blksize = 0;
    VDD_DMA_INFO DmaInfo;
    BYTE *TransferAddr;

    switch (DSPWriteState)
    {
    case WriteCommand:
        dprintf1(("wt CMD"));
        WriteCommandByte(data);
        break;

    case MidiWrite:
        dprintf1(("wt MIDI Byte"));
        BufferMidi(data);
        DSPWriteState = WriteCommand;
        break;

    case CardIdent:
        dprintf1(("wt ID"));
        IdentByte = data;
        DSPReadState = ReadIdent;
        DSPWriteState = WriteCommand;
        break;

    case TableMunge:
        dprintf1(("wt TblMunge"));
        TableMunger(data);
        DSPWriteState = WriteCommand;
        break;

    case RsvdRegWrite:
        dprintf1(("wt RsvReg"));
        ReservedRegister = data;
        DSPWriteState = WriteCommand;
        break;

    case SetTimeConstant:
        dprintf1(("wr TmCnst"));
        TimeConstant = (DWORD)data;
        dprintf3(("Time constant is %X", data));
        dprintf3(("Set sampling rate %d", GetSamplingRate()));
        DSPWriteState = WriteCommand;
        break;

    case BlockSizeFirstByte:
        dprintf1(("wt 1st Blksize"));
        blksize = data;
        DSPWriteState = BlockSizeSecondByte;
        break;

    case BlockSizeSecondByte:
        dprintf1(("wt 2nd Blksize"));
        SBBlockSize = ((DWORD)data << 8) + blksize;
        DSPWriteState = WriteCommand;
        dprintf3(("Block size = 0x%x", SBBlockSize));
        break;

    case BlockSizeFirstByteWrite:
        dprintf1(("wt 1st Blksize single"));
        blksize = (DWORD)data;
        DSPWriteState = BlockSizeSecondByteWrite;
        break;

    case BlockSizeSecondByteWrite:
        dprintf1(("wt 2nd Blksize single"));
        SBBlockSize = blksize + (data << 8);
        DSPWriteState = WriteCommand ;
        dprintf3(("Block size = 0x%x, Single Cycle starting", blksize + (data << 8)));

        if (SBBlockSize > 16)
        {
            DisplaySbMode(SBMODE_CYCLE);
            StartDspDmaThread(TRUE);
        }
        else
        {
            if (SBBlockSize)
            {
                QueryDMA(&DmaInfo);
                DmaInfo.count -= SBBlockSize + 1;
                DmaInfo.addr += SBBlockSize + 1;
                SetDMACountAddr(&DmaInfo);
                if (DmaInfo.count == -1)
                    SetDMAStatus(&DmaInfo, TRUE, TRUE);
            }
            GenerateInterrupt(2);
        }
        break;

    case BlockSizeFirstByteRead:
        dprintf1(("wt 1st IN Blksize"));
        blksize = data;
        DSPWriteState = BlockSizeSecondByteRead;
        break;

    case BlockSizeSecondByteRead:
        dprintf1(("wt 2nd IN Blksize"));
        SBBlockSize = blksize + (data << 8);
        DSPWriteState = WriteCommand;
        dprintf3(("IN Blksize set to 0x%x", blksize + (data << 8)));
        if (SBBlockSize > 16)
            return;
        if ((TransferAddr = GetDMATransferAddress()) != -1)
            *TransferAddr = 0x80u;
        GenerateInterrupt(2);
        break;

    case 0x30:
    case 0x31:
    case 0x32:
    case 0x33:
        dprintf3(("Cmd-Midi non UART read"));
        break;

    case 0x34:
    case 0x35:
        dprintf3(("Cmd-Midi UART I/O xxx"));
        break;

    case 0x37:
        break;

    case 0x38:
        dprintf3(("Cmd-MIDI out poll"));
        DSPWriteState = MidiWrite;
        break;

    default:
        dprintf2(("Unrecognized DSP write state %x", DSPWriteState));
        break;
    }
}

GLOBAL VOID DspReadStatus(BYTE *data)
{
    *data = DSPReadState == NothingToRead?0x7F:0xFF;
}

GLOBAL VOID SbGetDMAPosition()
{
    DWORD voffset;

    if (HWaveOut && SbAnswerDMAPosition)
    {
        while (!bWriteBurstStarted)
            Sleep(0);
        EnterCriticalSection(&DspLock);
        dprintf4(("SbGetDMAPosition"));
        ++DspNumberDmaQueries;
        if (!SbAnswerDMAPosition)
        {
            LeaveCriticalSection(&DspLock);
            return;
        }
        if (SBBlockSize < 0x400)
        {
            if (DspVirtualInc + DmaInfo.addr - StartingDmaAddr >= 3 * SBBlockSize >> 2)
            {
                Sleep(0);
            }
            else
            {
                DmaInfo.addr += DspVirtualInc;
                DmaInfo.count -= DspVirtualInc;
                dprintf4(("virt addr = %x, count = %x", DmaInfo.addr, DmaInfo.count));
                SetDMACountAddr(&DmaInfo);
            }
            LeaveCriticalSection(&DspLock);
            return;
        }
        if (DspMode == CycleMode)
            ++PreviousWaveOutPos;
        voffset = DspVirtualInc + DmaInfo.addr - StartingDmaAddr;
        if (voffset >= SBBlockSize - 80)
            DspVirtualInc = 2;
        if (voffset >= SBBlockSize - 8)
            voffset = SBBlockSize - 8;
        if (DspVirtualIncx > 192)
            Sleep(0);
        if (DspVirtualIncx > 128)
            Sleep(0);
        Sleep(0);
        dprintf3(("voffset = %x inc = %x\n", voffset, DspVirtualInc));
        DmaInfo.addr  = StartingDmaAddr  + voffset;
        DmaInfo.count = StartingDmaCount - voffset;
        SetDMACountAddr(&DmaInfo);
        dprintf4(("INB: AFT Cnt= %x, Addr= %x\n", DmaInfo.count, DmaInfo.addr));
        LeaveCriticalSection(&DspLock);
    }
}

GLOBAL VOID ContinueDMA()
{
    bDspPaused = FALSE;
    Sleep(0);
}

GLOBAL VOID CleanUpWave()
{
    bDspReset = TRUE;
    bExitDMAThread = TRUE;
    ResetDSP();
    CloseHandle(DspWaveSem);
    CloseHandle(ThreadStarted);
    CloseHandle(ThreadFinished);
    CloseHandle(DspResetEvent);
    CloseHandle(DspResetDone);
    CloseHandle(DspWavePlayed);
    VirtualFree(DspBuffer, 0, MEM_RELEASE);
    VirtualFree(WaveBlockSizes, 0, MEM_RELEASE);
    VirtualFree(WaveHdrs, 0, MEM_RELEASE);
    WaveHdrs = NULL;
    WaveBlockSizes = NULL;
    DspBuffer = NULL;
}

/*
 * ==========================================================================
 * Local functions
 * ==========================================================================
 */

LOCAL BOOL GenerateHdrs(DWORD size)
{
    LPSTR pBuffer;
    DWORD Bursts;
    int i;
    DWORD HeaderSize;

    HeaderSize = PageSize * ((PageSize + size - 1) / PageSize);
    dprintf3(("Genereate Header size %x", HeaderSize));
    if (DspBuffer)
        VirtualFree(DspBuffer, 0, MEM_RELEASE);
    if (WaveHdrs)
        VirtualFree(WaveHdrs, 0, MEM_RELEASE);
    if (WaveBlockSizes)
        VirtualFree(WaveBlockSizes, 0, MEM_RELEASE);
    DspBuffer = (PBYTE)VirtualAlloc(NULL, HeaderSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!DspBuffer)
    {
        dprintf2(("Unable to allocate DspBuffer memory"));
        return FALSE;
    }
    DspBufferTotalBursts = HeaderSize / BurstSize;
    WaveHdrs = (WAVEHDR *)VirtualAlloc(NULL, sizeof(WAVEHDR) * DspBufferTotalBursts, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!WaveHdrs)
    {
        dprintf2(("Unable to allocate WaveHdr memory"));
        VirtualFree(DspBuffer, 0, MEM_RELEASE);
        DspBuffer = NULL;
        return 0;
    }
    WaveBlockSizes = (DWORD *)VirtualAlloc(NULL, sizeof(DWORD) * DspBufferTotalBursts, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!WaveBlockSizes)
    {
        dprintf2(("Unable to allocate WaveBlockSize  memory"));
        VirtualFree(DspBuffer, 0, MEM_RELEASE);
        VirtualFree(WaveHdrs, 0, MEM_RELEASE);
        WaveHdrs = NULL;
        DspBuffer = NULL;
        return FALSE;
    }
    pBuffer = (LPSTR)DspBuffer;
    Bursts = 0;
    if (DspBufferTotalBursts > 0)
    {
        i = 0;
        do
        {
            WaveHdrs[i].dwBufferLength = BurstSize;
            WaveHdrs[i].lpData = pBuffer;
            WaveHdrs[i].dwFlags = 0;
            WaveHdrs[i].dwLoops = 0;
            WaveHdrs[i].dwUser = 0;
            pBuffer += BurstSize;
            ++Bursts;
            ++i;
        }
        while (Bursts < DspBufferTotalBursts);
    }
    DspBufferSize = HeaderSize;
    DspNextRead = DspBufferTotalBursts - 1;
    DspNextWrite = 0;
    return 1;
}

LOCAL BOOL GetWaveOutPosition(DWORD *pos)
{
    MMTIME mmt;

    mmt.wType = TIME_SAMPLES;
    if (GetPositionProc(HWaveOut, &mmt, sizeof(mmt)) == MMSYSERR_NOERROR)
    {
        *pos = mmt.u.sample;
        return TRUE;
    }
    dprintf2(("Get Waveout position failed\n"));
    return FALSE;
}

LOCAL BOOL WaitOnWaveOutIdle()
{
    DWORD pos, lastpos = 0;
    BOOL result;

    while (TRUE)
    {
        if (!(result = GetWaveOutPosition(&pos)) || lastpos == pos)
            break;
        lastpos = pos;
        Sleep(1u);
    }
    return result;
}

LOCAL VOID PrepareHeaders()
{
    DWORD Burst;

    dprintf4(("Prepare Headers"));
    if (DspBufferTotalBursts)
    {
        for (Burst = 0; Burst < DspBufferTotalBursts; Burst++)
            PrepareHeaderProc(HWaveOut, &WaveHdrs[Burst], sizeof(WaveHdrs[0]));
    }
}

LOCAL VOID UnprepareHeaders()
{
    DWORD Burst;

    dprintf4(("Unprepare Headers"));
    if (DspBufferTotalBursts)
    {
        for (Burst = 0; Burst < DspBufferTotalBursts; Burst++)
        {
            while (UnprepareHeaderProc(HWaveOut, &WaveHdrs[Burst], sizeof(WaveHdrs[0])) == WAVERR_STILLPLAYING)
                Sleep(1);
            WaveHdrs[Burst].dwFlags = 0;
            WaveHdrs[Burst].dwUser = 0;
        }
    }
}

LOCAL BOOL OpenWaveDevice(DWORD dwCallback)
{
    MMRESULT rc;

    rc = OpenProc(&HWaveOut, WaveOutDevice, &WaveFormat, dwCallback, 0, dwCallback?CALLBACK_FUNCTION:0);
    if (rc == MMSYSERR_NOERROR)
    {
        GetVolumeProc(HWaveOut, &OriginalWaveVol);
        PreviousWaveVol = OriginalWaveVol;
        NextWaveOutPos = 0;
        return TRUE;
    }
    else
    {
        dprintf2(("Failed to open wave device - code %d", rc));
        return FALSE;
    }
}

LOCAL VOID CloseWaveDevice()
{
    if (HWaveOut)
    {
        DWORD dwVolume;

        dprintf3(("Closing wave device"));
        if (GetVolumeProc(HWaveOut, &dwVolume) && dwVolume == PreviousWaveVol)
            SetVolumeProc(HWaveOut, OriginalWaveVol);
        WaitOnWaveOutIdle();
        UnprepareHeaders();
        ResetWaveDevice();
        if (CloseProc(HWaveOut))
        {
            dprintf2(("Unable to close wave out device"));
        }
        else
        {
            HWaveOut = NULL;
        }
    }
}

LOCAL VOID ResetWaveDevice()
{
    dprintf4(("Resetting wave device"));
    if (HWaveOut && ResetProc(HWaveOut))
        dprintf2(("Unable to reset wave out device"));
}

LOCAL VOID AutoInitEoiHook(int IrqLine, int CallCount)
{
    if (SetWaveFormat())
    {
        dprintf3(("auto init CHANGING Wave Out device"));
        CloseWaveDevice();
        if (!HWaveOut)
        {
            OpenWaveDevice(0);
            PrepareHeaders();
        }
    }
    if (!bDspPaused)
        DmaDataToDsp(0);
}

LOCAL VOID ExitAutoMode()
{
    DspMode = NoMode;
    if (bExitAuto)
    {
        bExitAuto = FALSE;
        dprintf3(("ExitAuto CMD detected in Auto Mode"));
    }
    else
        dprintf3(("SingleCycle detected in Auto Mode"));
    RemoveEOIHook(SbInterrupt, AutoInitEoiHook);
    NextWaveOutPos = 0;
    dprintf3(("Auto-Init block done"));
}

LOCAL BOOL QueryDMA(VDD_DMA_INFO *pDmaInfo)
{
    pDmaInfo->addr  = ((WORD)pDcp->current_address[Chan][1] << 8)
                      | (WORD)pDcp->current_address[Chan][0];

    pDmaInfo->count = ((WORD)pDcp->current_count[Chan][1] << 8)
                      | (WORD)pDcp->current_count[Chan][0];

    pDmaInfo->page    = pDmaAdp->pages.page[SbDmaChannel];
    pDmaInfo->status  = pDcp->status.all;
    pDmaInfo->mode    = pDcp->mode[Chan].all;
    pDmaInfo->mask    = pDcp->mask;

    dprintf4((
                 "DMA Info : addr  %4X, count %4X, page %4X, status %2X, mode %2X, mask %2X",
                 pDmaInfo->addr,
                 pDmaInfo->count,
                 pDmaInfo->page,
                 pDmaInfo->status,
                 pDmaInfo->mode,
                 pDmaInfo->mask));
    return TRUE;
}

LOCAL VOID DmaDataToDsp(DWORD mode)
{
    PBYTE pPhyAddr;
    DWORD i, nCount, nRemain;

    if (DspMode != AutoInitMode || !bDspReset)
    {
        EnterCriticalSection(&DspLock);
        QueryDMA(&DmaInfo);
        StartingDmaAddr  = DmaInfo.addr;
        StartingDmaCount = DmaInfo.count;
        if (SBBlockSize > 0xFF)
            SbAnswerDMAPosition = TRUE;
        LeaveCriticalSection(&DspLock);
        if (DmaInfo.count != -1 && DmaInfo.count && !(DmaInfo.mask & (1 << SbDmaChannel)))
        {
            dprintf3(("Wt: xfer DMA data to DspBuffer"));
            pPhyAddr = (PBYTE)Sim32GetVDMPointer((DmaInfo.page << 28) + DmaInfo.addr, 0, 0);
            if (pPhyAddr == -1)
                dprintf2(("Unable to get transfer address"));
            else
            {
                dprintf4((
                             "Wt: DMA Virtual= %4X, Physical= %4X, size= %4X BlkSize = %4x",
                             DmaInfo.addr,
                             pPhyAddr,
                             DmaInfo.count,
                             SBBlockSize));
                nRemain = (SBBlockSize + 1) % BurstSize;
                nCount = (SBBlockSize + 1) / BurstSize;
                if (nRemain) nCount++;
                WaveBlockSizes[DspNextWrite] = SBBlockSize;
                if (WaveHdrs[DspNextWrite].dwFlags & WHDR_INQUEUE)
                    ResetWaveDevice();
                for (i = 0; i < nCount; ++i)
                {
                    dprintf3(("Write: Current burst Block at %x", DspNextWrite));
                    if (i == nCount - 1)
                    {
                        memcpy(DspBuffer + BurstSize * DspNextWrite, pPhyAddr, nRemain?nRemain:BurstSize);
                    }
                    else
                    {
                        memcpy(DspBuffer + BurstSize * DspNextWrite, pPhyAddr, BurstSize);
                        pPhyAddr += BurstSize;
                    }
                    DspNextWrite = (DspNextWrite + 1) % DspBufferTotalBursts;
                    ReleaseSemaphore(DspWaveSem, 1, 0);
                }
                if (SBBlockSize >= 0x1FF)
                    Sleep(0);
            }
        }
    }
}

LOCAL BOOL WriteBurst(DWORD Size)
{
    WAVEHDR *WaveHdr;

    WaveHdr = &WaveHdrs[DspNextRead];
    dprintf3(("read: write burst at block %x", DspNextRead));
    WaveHdr->dwBufferLength = Size;
    return WriteProc(HWaveOut, WaveHdr, sizeof(WAVEHDR)) == MMSYSERR_NOERROR;
}

LOCAL VOID WriteCommandByte(BYTE command)
{
    VDD_DMA_INFO DmaInfo;

    switch (command)
    {
    case DSP_GET_VERSION:
        dprintf3(("Cmd-GetVer"));
        DSPReadState = FirstVersionByte;
        break;

    case DSP_CARD_IDENTIFY:
        dprintf3(("Cmd-Id"));
        DSPWriteState = CardIdent;
        break;

    case DSP_SPEAKER_ON:
        dprintf3(("Cmd-Speaker ON"));
        SetSpeaker(TRUE);
        break;

    case DSP_SPEAKER_OFF:
        dprintf3(("Cmd-Speaker OFF"));
        SetSpeaker(FALSE);
        break;

    case DSP_SET_SAMPLE_RATE:
        dprintf3(("Cmd-Set Sample Rate"));
        DSPWriteState = SetTimeConstant;
        break;

    case DSP_SET_BLOCK_SIZE:
        dprintf3(("Cmd-Set Block Size"));
        DSPWriteState = BlockSizeFirstByte;
        return;

    case DSP_WRITE:
        dprintf3(("Cmd- DSP OUT"));
        DSPWriteState = BlockSizeFirstByteWrite;
        return;

    //case DSP_DIRECT_WAVE_OUT:  // NOT IMPLEMENTED


    case DSP_WRITE_AUTO:
        dprintf3(("Cmd-DSP OUT Auto"));
        if (SBBlockSize <= 16)
        {
            if (SBBlockSize)
            {
                QueryDMA(&DmaInfo);
                DmaInfo.count   -= SBBlockSize + 1;
                DmaInfo.addr += SBBlockSize + 1;
                SetDMACountAddr(&DmaInfo);
                if (DmaInfo.count == -1)
                    SetDMAStatus(&DmaInfo, TRUE, TRUE);
            }
            GenerateInterrupt(2);
        }
        else
        {
            DisplaySbMode(SBMODE_AUTOINIT);
            StartDspDmaThread(FALSE);
        }
        break;

    case DSP_READ:
        dprintf3(("Cmd- DSP IN - non Auto"));
        DSPWriteState = BlockSizeFirstByteRead;
        break;


    // case DSP_READ_AUTO:  // NOT IMPLEMENTED

    case DSP_HALT_DMA:
        dprintf3(("Cmd-Pause DMA"));
        PauseDMA();
        break;

    case DSP_CONTINUE_DMA:
        dprintf3(("Cmd - Continue DMA"));
        ContinueDMA();
        break;

    case DSP_STOP_AUTO:
        dprintf3(("Cmd- Exit Auto-Init"));
        bExitAuto = TRUE;
        break;

    case DSP_GENERATE_INT:
        dprintf3(("Cmd-GenerateInterrupt"));
        GenerateInterrupt(1);
        break;

    case DSP_HI_WRITE_AUTO:
        dprintf3(("Cmd-DSP HS OUT AUTO"));
        bHighSpeedMode = TRUE;
        DisplaySbMode(SBMODE_HIAUTOINIT);
        StartDspDmaThread(FALSE);
        break;

    case DSP_HI_WRITE_SINGLE:
        dprintf3(("Cmd- DSP HS OUT"));
        bHighSpeedMode = TRUE;
        DisplaySbMode(SBMODE_HICYCLE);
        StartDspDmaThread(FALSE);
        break;

    case DSP_TABLE_MUNGE:
        dprintf3(("Cmd-Table Munge"));
        DSPWriteState = TableMunge;
        break;

    case DSP_RESREG_WRITE:
        dprintf3(("Cmd-Wt Res Reg"));
        DSPWriteState = RsvdRegWrite;
        break;

    case DSP_RESREG_READ:
        dprintf3(("Cmd-Rd Res Reg"));
        DSPReadState = ReadRsvdReg;
        break;

    default:
        dprintf3(("Unrecognized DSP command %2X", command));
        break;
    }
}


/*
 *  Get sampling rate for time constant
 */
LOCAL DWORD GetSamplingRate(void)
{
    /*
     *  Sampling rate = 1000000 / (256 - Time constant)
     */

    if (!TimeConstant) TimeConstant = 1;

    if (TimeConstant<=132) return 8000;
    if (TimeConstant>=165 && TimeConstant<=166) return 11025;  // 256 - 1000000 / 11025
    if (TimeConstant>=210 && TimeConstant<=211) return 22050;  // 256 - 1000000 / 22050
    if (TimeConstant>=233) return 44100;
    return 1000000 / (256 - TimeConstant);
}

/*
 *  Generate the device interrupt
 */
LOCAL VOID GenerateInterrupt(int delay)
{
    if (delay)
    {
        dprintf3(("Generating interrupt with %x ms delay", delay));
        host_DelayHwInterrupt(SbInterrupt, 1, 1000 * delay);
    }
    else
    {
        dprintf3(("Generating interrupt"));
        ica_hw_interrupt(ICA_MASTER, SbInterrupt, 1);
    }
}

LOCAL VOID SetSpeaker(BOOL bOn)
{
    if (HWaveOut)
    {
        if (bOn)
        {
            if (PreviousWaveVol == 0)
                SetWaveOutVolume(-1);
        }
        else
            SetWaveOutVolume(0);
    }
}

LOCAL VOID SetWaveOutVolume(DWORD NewWaveVolume)
{
    DWORD CurrentWaveVol;

    if (HWaveOut && GetVolumeProc(HWaveOut, &CurrentWaveVol))
    {
        if (CurrentWaveVol != PreviousWaveVol)
            OriginalWaveVol = CurrentWaveVol;
        PreviousWaveVol = NewWaveVolume;
        SetVolumeProc(HWaveOut, NewWaveVolume);
    }
}

LOCAL PBYTE GetDMATransferAddress()
{
    PBYTE pAddr = Sim32GetVDMPointer((pDmaAdp->pages.page[SbDmaChannel] << 28) + (
                                         ((WORD)pDcp->current_address[Chan][1] << 8) | (WORD)pDcp->current_address[Chan][0]), 0, 0);

    dprintf4(("Physical Transfer address = %8X", pAddr));
    return pAddr;
}

LOCAL VOID SetDMAStatus(VDD_DMA_INFO *pDmaInfo, BOOL bRequesting, BOOL bTerminalCntRchd)
{
    if (bRequesting)
    {
        pDmaInfo->status |= (0x10 << SbDmaChannel);
        dprintf4(("DMA set as requesting"));
    }
    else
    {
        pDmaInfo->status &= ~(0x10 << SbDmaChannel);
        dprintf4(("DMA set as not requesting"));
    }

    if (bTerminalCntRchd)
    {
        pDmaInfo->status |= (1 << SbDmaChannel);
        dprintf4(("DMA set as terminal count reached"));
    }
    else
    {
        pDmaInfo->status &= ~(1 << SbDmaChannel);
        dprintf4(("DMA set as terminal count not reached"));
    }

    pDcp->status.all = pDmaInfo->status;
}

LOCAL BOOL SetDMACountAddr(VDD_DMA_INFO *pDmaInfo)
{
    pDcp->current_address[Chan][1] = (half_word)HIBYTE(pDmaInfo->addr);
    pDcp->current_address[Chan][0] = (half_word)LOBYTE(pDmaInfo->addr);
    pDcp->current_count[Chan][1]   = (half_word)HIBYTE(pDmaInfo->count);
    pDcp->current_count[Chan][0]   = (half_word)LOBYTE(pDmaInfo->count);
    if (pDcp->current_count[Chan] == -1 && pDcp->mode[Chan].bits.auto_init)
    {
        pDcp->current_count[Chan][1]  = (half_word)HIBYTE(pDcp->base_count[Chan]);
        pDcp->current_count[Chan][0]  = (half_word)LOBYTE(pDcp->base_count[Chan]);
        pDcp->current_address[Chan][1]  = (half_word)HIBYTE(pDcp->base_address[Chan]);
        pDcp->current_address[Chan][0]  = (half_word)LOBYTE(pDcp->base_address[Chan]);
    }
    return TRUE;
}

LOCAL VOID PauseDMA()
{
    DWORD pms;

    bDspPaused = TRUE;
    if (DspMode == CycleMode
            && SbAnswerDMAPosition
            && GetWaveOutPosition(&pms)
            && SBBlockSize - pms + StartingWaveOutPos + 1 > BurstSize / 2)
    {
        Sleep(0);
    }
}

LOCAL BOOL TestWaveFormat(DWORD SampleRate)
{
    PCMWAVEFORMAT Format;

    Format = WaveFormat;
    Format.wf.nSamplesPerSec = SampleRate;
    Format.wf.nAvgBytesPerSec = SampleRate;

    return OpenProc(NULL, WaveOutDevice, &Format.wf, 0, 0, WAVE_FORMAT_QUERY) == MMSYSERR_NOERROR;
}

LOCAL VOID TableMunger(BYTE data)
{
    VDD_DMA_INFO DmaInfo;
    static BOOL bFirstByte = TRUE;
    static BYTE lastbyte;
    BYTE *TransferAddr;

    if (bFirstByte)
    {
        dprintf4(("Munging first byte"));

        lastbyte = (((data & 0x10) != 0 ? 64 : 32) - 2 * (data & 6)) + data;
        dprintf4(("Writing first byte"));
        TransferAddr = GetDMATransferAddress();
        if (TransferAddr != -1)
        {
            *TransferAddr = lastbyte;
            QueryDMA(&DmaInfo);
            --DmaInfo.count;
            ++DmaInfo.addr;
            SetDMACountAddr(&DmaInfo);
            bFirstByte = FALSE;
            return;
        }
    }
    else
    {
        dprintf4(("Munging second byte"));
        lastbyte += data ^ 0xA5;
        dprintf4(("Writing second byte"));
        TransferAddr = GetDMATransferAddress();
        if (TransferAddr != -1)
        {
            *TransferAddr = lastbyte;
            QueryDMA(&DmaInfo);
            --DmaInfo.count;
            ++DmaInfo.addr;
            SetDMACountAddr(&DmaInfo);
            if (DmaInfo.count == -1)
                SetDMAStatus(&DmaInfo, 0, 1);
            bFirstByte = TRUE;
        }
    }

    dprintf2(("Unable to get dma address"));
}

LOCAL VOID DspProcessBlock(BOOL bCycle)
{
    DWORD WaveBlockSize, Increment;

    DspNextRead = (DspNextRead + 1) % DspBufferTotalBursts;
    WaveBlockSize = WaveBlockSizes[DspNextRead] + 1;
    if (DspMode == CycleMode)
    {
        ResetWaveDevice();
        GetWaveOutPosition(&StartingWaveOutPos);
    }
    else
    {
        if (NextWaveOutPos)
        {
            StartingWaveOutPos = NextWaveOutPos;
        }
        else
        {
            ResetWaveDevice();
            GetWaveOutPosition(&StartingWaveOutPos);
        }
        NextWaveOutPos = WaveBlockSize + StartingWaveOutPos;
    }
    PreviousWaveOutPos = StartingWaveOutPos;
    if (NextWaveOutPos > 0xFFF00000)
        NextWaveOutPos = 0;
    if (DspNumberDmaQueries)
    {
        Increment = WaveBlockSize / DspNumberDmaQueries;
        DspVirtualInc = WaveBlockSize / DspNumberDmaQueries;
        if (WaveBlockSize <= 1024)
        {
            if (Increment > WaveBlockSize / 4)
                DspVirtualInc = Increment = WaveBlockSize / 4;
        }
        else
        {
            if (Increment < 5)
                DspVirtualInc = Increment = 5;
            else if (Increment > 512)
                DspVirtualInc = Increment = 512;
        }
    }
    else
    {
        DspVirtualInc = Increment = WaveBlockSize >> 4;
    }
    DspVirtualIncx = Increment;
    dprintf4(("NoQ = %x, Inc = %x\n", DspNumberDmaQueries, Increment));
    DspNumberDmaQueries = 0;
    while (WaveBlockSize > 0)
    {
        if (WaveBlockSize >= BurstSize)
        {
            if (!bDspReset) WriteBurst(BurstSize);
            WaveBlockSize -= BurstSize;
        }
        else
        {
            if (!bDspReset) WriteBurst(WaveBlockSize);
            WaveBlockSize = 0;
        }
        if (WaveBlockSize)
        {
            DspNextRead = (DspNextRead + 1) % DspBufferTotalBursts;
            WaitForSingleObject(DspWaveSem, INFINITE);
        }
    }
    if (!bDspReset)
        bWriteBurstStarted = TRUE;
}


LOCAL DWORD DspThreadEntry(LPVOID lpThreadParameter)
{
    DWORD Delay, SamplesLeft, iDel, pms;
    HANDLE Handles[2];

    Delay = INFINITE;
    iDel = 0;
    bDspActive = TRUE;
    SetEvent(ThreadStarted);
    Handles[0] = DspWaveSem;
    Handles[1] = DspResetEvent;

    while (!bExitDMAThread)
    {
        dprintf3(("Rd: Waiting for wave semaphore with Delay = %x", Delay));
        if (WaitForMultipleObjects(2, Handles, 0, Delay) == WAIT_TIMEOUT)
        {
            EnterCriticalSection(&DspLock);
            if (PreviousWaveOutPos != StartingWaveOutPos  && GetWaveOutPosition(&pms))
            {
                if (pms < StartingWaveOutPos)
                {
                    dprintf2(("rd:sound pos is backward"));
                    pms = StartingWaveOutPos;
                }
                SamplesLeft = SBBlockSize - pms + StartingWaveOutPos + 1;
                dprintf3(("rd: Samples left %x, pos = %x, spos = %x\n", SamplesLeft, pms, StartingWaveOutPos));
                if (SamplesLeft > EndingDmaValue)
                {
                    Delay = 1000 * (SamplesLeft + 1) / WaveFormat.wf.nAvgBytesPerSec + 1;
                    dprintf3(("rd: more interrupt delay %x ...\n", Delay));
                    if (++iDel < 10)
                    {
                        LeaveCriticalSection(&DspLock);
                        continue;
                    }
                    NextWaveOutPos = 0;
                }
            }
            iDel = 0;
            DmaInfo.count = StartingDmaCount - SBBlockSize - 1;
            DmaInfo.addr  = StartingDmaAddr + SBBlockSize + 1;
            SetDMACountAddr(&DmaInfo);
            if (DmaInfo.count == -1)
                SetDMAStatus(&DmaInfo, TRUE, TRUE);
            GenerateInterrupt(0);
            SbAnswerDMAPosition = 0;
            bWriteBurstStarted = 0;
            LeaveCriticalSection(&DspLock);
            dprintf4(("rd: Dma Position = %4X, count = %4X", DmaInfo.addr, DmaInfo.count));
            Delay = INFINITE;
        }
        else
        {
            Delay = INFINITE;
            if (bDspReset)
            {
                if (!DspMode)
                    ExitAutoMode();
            }
            else
            {
                dprintf3(("Rd: Wave semaphore received"));
                if (DspMode == CycleMode)
                {
                    if (SetWaveFormat())
                    {
                        dprintf3(("Single-Cycle CHANGING Wave Out device"));
                        CloseWaveDevice();
                    }
                    if (!HWaveOut)
                    {
                        OpenWaveDevice(0);
                        PrepareHeaders();
                    }
                    DspProcessBlock(1);
                    EndingDmaValue = 4;
                    bHighSpeedMode = 0;
                    Delay = 1000 * (SBBlockSize + 1) / WaveFormat.wf.nAvgBytesPerSec + 1;
                }
                else
                {
                    DspProcessBlock(0);
                    if (bExitAuto)
                        ExitAutoMode();
                    EndingDmaValue = 32;
                    if (SBBlockSize >= 0x400)
                    {
                        if (SBBlockSize > 0x1800)
                            Delay = 1000 * (SBBlockSize - BurstSize + 1) / WaveFormat.wf.nAvgBytesPerSec + 1;
                        else
                            Delay = 1000 * (SBBlockSize + 1) / WaveFormat.wf.nAvgBytesPerSec + 1;
                    }
                    else
                    {
                        Delay = 1000 * ((SBBlockSize >> 3) + SBBlockSize + 1) / WaveFormat.wf.nAvgBytesPerSec + 1;
                    }
                }
                dprintf3(("Interrupt Delay = %x\n", Delay));
            }
        }
    }
    SetEvent(DspResetDone);
    CloseWaveDevice();
    bDspActive = 0;
    SetEvent(ThreadFinished);
    RemoveEOIHook(SbInterrupt, AutoInitEoiHook);
    dprintf3(("Dsp DMA thread is exiting"));
    return 0;
}

LOCAL BOOL SetWaveFormat()
{
    DWORD SampleRate, i;

    if (TimeConstant != 0xFFFF)
    {
        SampleRate = GetSamplingRate();
        dprintf3(("Requested sample rate is %d", SampleRate));
        if (SampleRate != WaveFormat.wf.nSamplesPerSec)
        {

            /*
             *  Search for a suitable format
             */

            if (!TestWaveFormat(SampleRate))
            {
                dprintf4(("Finding closest wave format"));
                for (i = 0; i<50000; i++)
                {
                    if (TestWaveFormat(SampleRate - i))
                    {
                        SampleRate  -= i;
                        break;
                    }
                    if (TestWaveFormat(SampleRate  + i))
                    {
                        SampleRate  += i;
                        break;
                    }
                }

                if (i == 50000)
                {
                    dprintf2(("Unable to find suitable wave format"));
                    return FALSE;
                }
            }

            if (SampleRate != WaveFormat.wf.nSamplesPerSec)
            {
                dprintf3(("Setting %d samples per second", SampleRate));
                WaveFormat.wf.nSamplesPerSec = SampleRate;
                WaveFormat.wf.nAvgBytesPerSec = SampleRate;
                TimeConstant = 0xFFFF;
                return TRUE;
            }
        }
        TimeConstant = 0xFFFF;
    }
    return FALSE;
}

LOCAL BOOL StartDspDmaThread(BOOL bCycle)
{
    HANDLE hThread;
    DWORD ThreadId;

    bWriteBurstStarted = FALSE;
    if (!bDspActive)
    {
        hThread = CreateThread(NULL, 0, DspThreadEntry, NULL, CREATE_SUSPENDED, &ThreadId);
        if (!hThread)
        {
            dprintf2(("Create Dsp DMA thread failed code %d", GetLastError()));
            return FALSE;
        }
        bDspActive = TRUE;
        bExitAuto = FALSE;
        DspNextRead = DspBufferTotalBursts - 1;
        DspNextWrite = 0;
        CloseHandle(DspWaveSem);
        DspWaveSem = CreateSemaphore(NULL, 0, 100, NULL);
        if (!SetThreadPriority(hThread, THREAD_PRIORITY_HIGHEST))
            dprintf2(("Unable to set thread priority"));
        ResumeThread(hThread);
        CloseHandle(hThread);
        WaitForSingleObject(ThreadStarted, INFINITE);
    }
    if (SBBlockSize + 1 > DspBufferSize - BurstSize)
    {
        if (HWaveOut)
        {
            WaitOnWaveOutIdle();
            UnprepareHeaders();
        }
        GenerateHdrs(BurstSize + SBBlockSize + 1);
        if (HWaveOut)
            PrepareHeaders();
    }
    if (bCycle)
    {
        if (DspMode == AutoInitMode)
            ExitAutoMode();
        DspMode = CycleMode;
    }
    else
    {
        if (!HWaveOut || TimeConstant != 0xFFFF)
        {
            CloseWaveDevice();
            SetWaveFormat();
            OpenWaveDevice(0);
            PrepareHeaders();
            RegisterEOIHook(SbInterrupt, AutoInitEoiHook);
        }
        DspMode = AutoInitMode;
    }
    bDspPaused = FALSE;
    DmaDataToDsp(DspMode);
    return TRUE;
}

LOCAL VOID StopDspDmaThread(BOOL bWait)
{
    if (bDspActive)
    {
        dprintf3(("Stopping DSP DMA thread"));
        ReleaseSemaphore(DspWaveSem, 1, 0);

        if (bWait)
        {
            dprintf4(("Waiting for thread to exit"));
            WaitForSingleObject(ThreadFinished, INFINITE);
            dprintf4(("thread has exited"));
        }
    }
}

