BOOL OpenFMDevice();
VOID CloseFMDevice();
VOID ResetFM();
VOID FMStatusRead(half_word *value);
VOID FMRegisterSelect(half_word value);
VOID FMDataWrite(half_word value);
