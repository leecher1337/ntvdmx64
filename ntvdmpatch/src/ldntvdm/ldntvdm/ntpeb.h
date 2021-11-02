#if !defined(DEFINE_BASEMSG32)
typedef struct tag_UNICODE_STRING64
{
	union
	{
		struct
		{
			WORD Length;
			WORD MaximumLength;
		};
		ULONGLONG dummy;
	};
	ULONGLONG Buffer;
} UNICODE_STRING64;
#endif

typedef struct tag_PEB
{
	union
	{
		struct
		{
			BYTE InheritedAddressSpace;
			BYTE ReadImageFileExecOptions;
			BYTE BeingDebugged;
			BYTE BitField;
		};
		ULONGLONG dummy01;
	};
	ULONGLONG Mutant;
	ULONGLONG ImageBaseAddress;
	ULONGLONG Ldr;
	ULONGLONG ProcessParameters;
	ULONGLONG SubSystemData;
	ULONGLONG ProcessHeap;
	ULONGLONG FastPebLock;
	ULONGLONG AtlThunkSListPtr;
	ULONGLONG IFEOKey;
	ULONGLONG CrossProcessFlags;
	ULONGLONG UserSharedInfoPtr;
	DWORD SystemReserved;
	DWORD AtlThunkSListPtr32;
	ULONGLONG ApiSetMap;
	ULONGLONG TlsExpansionCounter;
	ULONGLONG TlsBitmap;
	DWORD TlsBitmapBits[2];
	ULONGLONG ReadOnlySharedMemoryBase;
	ULONGLONG HotpatchInformation;
	ULONGLONG ReadOnlyStaticServerData;
	ULONGLONG AnsiCodePageData;
	ULONGLONG OemCodePageData;
	ULONGLONG UnicodeCaseTableData;
	DWORD NumberOfProcessors;
	union
	{
		DWORD NtGlobalFlag;
		DWORD dummy02;
	};
	LARGE_INTEGER CriticalSectionTimeout;
	ULONGLONG HeapSegmentReserve;
	ULONGLONG HeapSegmentCommit;
	ULONGLONG HeapDeCommitTotalFreeThreshold;
	ULONGLONG HeapDeCommitFreeBlockThreshold;
	DWORD NumberOfHeaps;
	DWORD MaximumNumberOfHeaps;
	ULONGLONG ProcessHeaps;
	ULONGLONG GdiSharedHandleTable;
	ULONGLONG ProcessStarterHelper;
	ULONGLONG GdiDCAttributeList;
	ULONGLONG LoaderLock;
	DWORD OSMajorVersion;
	DWORD OSMinorVersion;
	WORD OSBuildNumber;
	WORD OSCSDVersion;
	DWORD OSPlatformId;
	DWORD ImageSubsystem;
	DWORD ImageSubsystemMajorVersion;
	ULONGLONG ImageSubsystemMinorVersion;
	ULONGLONG ActiveProcessAffinityMask;
	ULONGLONG GdiHandleBuffer[30];
	ULONGLONG PostProcessInitRoutine;
	ULONGLONG TlsExpansionBitmap;
	DWORD TlsExpansionBitmapBits[32];
	ULONGLONG SessionId;
	ULARGE_INTEGER AppCompatFlags;
	ULARGE_INTEGER AppCompatFlagsUser;
	ULONGLONG pShimData;
	ULONGLONG AppCompatInfo;
	UNICODE_STRING64 CSDVersion;
	ULONGLONG ActivationContextData;
	ULONGLONG ProcessAssemblyStorageMap;
	ULONGLONG SystemDefaultActivationContextData;
	ULONGLONG SystemAssemblyStorageMap;
	ULONGLONG MinimumStackCommit;
	ULONGLONG FlsCallback;
	LIST_ENTRY64 FlsListHead;
	ULONGLONG FlsBitmap;
	DWORD FlsBitmapBits[4];
	ULONGLONG FlsHighIndex;
	ULONGLONG WerRegistrationData;
	ULONGLONG WerShipAssertPtr;
	ULONGLONG pContextData;
	ULONGLONG pImageHeaderHash;
	ULONGLONG TracingFlags;
} PEB64;

#pragma pack(1)
typedef struct _PEB32
{
	UCHAR				InheritedAddressSpace;				// 0
	UCHAR				ReadImageFileExecOptions;			// 1
	UCHAR				BeingDebugged;					// 2
	union
	{
		UCHAR BitField;                                                     //0x3
		struct
		{
			UCHAR ImageUsesLargePages : 1;                                    //0x3
			UCHAR IsProtectedProcess : 1;                                     //0x3
			UCHAR IsImageDynamicallyRelocated : 1;                            //0x3
			UCHAR SkipPatchingUser32Forwarders : 1;                           //0x3
			UCHAR IsPackagedProcess : 1;                                      //0x3
			UCHAR IsAppContainer : 1;                                         //0x3
			UCHAR IsProtectedProcessLight : 1;                                //0x3
			UCHAR IsLongPathAwareProcess : 1;                                 //0x3
		};
	};
	ULONG				Mutant;						// 4
	ULONG				ImageBaseAddress;				// 8
	ULONG				Ldr;							// C
	ULONG				ProcessParameters;				// 10
	ULONG				SubSystemData;
	ULONG				ProcessHeap;
	ULONG				FastPebLock;
	ULONG				AtlThunkSListPtr;
	ULONG				IFEOKey;
	union
	{
		ULONG CrossProcessFlags;
		struct
		{
			ULONG ProcessInJob : 1;
			ULONG ProcessInitializing : 1;
			ULONG ProcessUsingVEH : 1;
			ULONG ProcessUsingVCH : 1;
			ULONG ReservedBits0 : 28;
		};
	};
	union
	{
		ULONG KernelCallbackTable;                                          //0x2c
		ULONG UserSharedInfoPtr;                                            //0x2c
	};
	ULONG SystemReserved;                                                   //0x30
	ULONG AtlThunkSListPtr32;                                               //0x34
	ULONG ApiSetMap;                                                        //0x38
	ULONG TlsExpansionCounter;                                              //0x3c
	ULONG TlsBitmap;                                                        //0x40
	ULONG TlsBitmapBits[2];                                                 //0x44
	ULONG ReadOnlySharedMemoryBase;                                         //0x4c
	ULONG SharedData;                                                       //0x50
	ULONG ReadOnlyStaticServerData;                                         //0x54
	ULONG AnsiCodePageData;                                                 //0x58
	ULONG OemCodePageData;                                                  //0x5c
	ULONG UnicodeCaseTableData;                                             //0x60
	ULONG NumberOfProcessors;                                               //0x64
	ULONG NtGlobalFlag;                                                     //0x68
	union _LARGE_INTEGER CriticalSectionTimeout;                            //0x70
	ULONG HeapSegmentReserve;                                               //0x78
	ULONG HeapSegmentCommit;                                                //0x7c
	ULONG HeapDeCommitTotalFreeThreshold;                                   //0x80
	ULONG HeapDeCommitFreeBlockThreshold;                                   //0x84
	ULONG NumberOfHeaps;                                                    //0x88
	ULONG MaximumNumberOfHeaps;                                             //0x8c
	ULONG ProcessHeaps;                                                     //0x90
	ULONG GdiSharedHandleTable;                                             //0x94
	ULONG ProcessStarterHelper;                                             //0x98
	ULONG GdiDCAttributeList;                                               //0x9c
	ULONG LoaderLock;                                                       //0xa0
	ULONG OSMajorVersion;                                                   //0xa4
	ULONG OSMinorVersion;                                                   //0xa8
	USHORT OSBuildNumber;                                                   //0xac
	USHORT OSCSDVersion;                                                    //0xae
	ULONG OSPlatformId;                                                     //0xb0
	ULONG ImageSubsystem;                                                   //0xb4
	ULONG ImageSubsystemMajorVersion;                                       //0xb8
	ULONG ImageSubsystemMinorVersion;                                       //0xbc
	ULONG ActiveProcessAffinityMask;                                        //0xc0
	ULONG GdiHandleBuffer[34];                                              //0xc4
	ULONG PostProcessInitRoutine;                                           //0x14c
} PEB32, *PPEB32;
#pragma pack()

#define NtCurrentPeb() (NtCurrentTeb()->ProcessEnvironmentBlock)

// Using PEB_ARCH to not conflict with PEB from Winternl.h
#ifdef _WIN64
typedef PEB64 PEB_ARCH, *PPEB_ARCH;
#else
typedef PEB32 PEB_ARCH, *PPEB_ARCH;
#endif
