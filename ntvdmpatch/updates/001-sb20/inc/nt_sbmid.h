BOOL InitializeMidi();
BOOL OpenMidiDevice(DWORD dwCallback);
VOID CloseMidiDevice();
VOID BufferMidi(BYTE data);
VOID DetachMidi();
