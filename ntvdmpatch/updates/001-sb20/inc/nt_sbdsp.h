BOOL FindWaveDevice();
BOOL PrepareWaveInitialization();
VOID DspReadData(BYTE *data);
VOID DspReadStatus(BYTE *data);
BOOL DspResetWrite(BYTE data);
VOID ContinueDMA();
VOID ResetDSP();
VOID CleanUpWave();
