
#ifndef EFI_H_
#define EFI_H_
typedef unsigned long long UINTN;
typedef unsigned int UINT32;
typedef unsigned long long UINT64;
typedef unsigned short UINT16;
typedef unsigned char UINT8;
typedef int INT16;
typedef UINT64 EFI_PHYSICAL_ADDRESS;
typedef UINT64 EFI_VIRTUAL_ADDRESS;
typedef UINTN EFI_STATUS;
typedef void VOID;
#ifndef EFI_ERROR
#define EFI_ERROR(Status) (((INTN)(Status)) < 0)
#endif
#define IN
#define OUT
#define OPTIONAL
#define EFIAPI __attribute__((ms_abi))
#define AllocateAnyPages 0 
#define AllocateMaxAddress 1 
#define AllocateAddress 2

typedef struct {
    UINT32 Data1;
    UINT16 Data2;
    UINT16 Data3;
    UINT8 Data4[8];
} EFI_GUID;
typedef
EFI_STATUS
(EFIAPI *EFI_LOCATE_PROTOCOL)(
  IN EFI_GUID *Protocol,
  IN VOID *Registration OPTIONAL,
  OUT VOID **Interface
);


    



// UEFIの基本型
typedef unsigned short CHAR16;
typedef void*          EFI_HANDLE;
typedef struct {
   UINT32 Type;
   UINT32 Pad;
    EFI_PHYSICAL_ADDRESS PhysicalStart;
    EFI_VIRTUAL_ADDRESS VirtualStart;
    UINT64 NumberOfPages;
    UINT64 Attribute;
} EFI_MEMORY_DESCRIPTOR;
typedef enum { EfiReservedMemoryType,
    EfiLoaderCode,
    EfiLoaderData,
    EfiBootServicesCode,
    EfiBootServicesData,
    EfiRuntimeServicesCode,
    EfiRuntimeServicesData,
    EfiConventionalMemory,
    EfiUnusableMemory,
    EfiACPIReclaimMemory,
    EfiACPIMemoryNVS,
    EfiMemoryMappedIO,
    EfiMemoryMappedIOPortSpace,
    EfiPalCode,
    EfiPersistentMemory,
    EfiMaxMemoryType
} EFI_MEMORY_TYPE;

typedef __attribute__((ms_abi)) EFI_STATUS (*EFI_GET_MEMORY_MAP)(
    UINTN *MemoryMapSize,
    EFI_MEMORY_DESCRIPTOR *MemoryMap,
    UINTN *MapKey,
    UINTN *DescriptorSize,
    UINT32 *DescriptorVersion
); 
typedef __attribute__((ms_abi)) EFI_STATUS (*EFI_ALLOCATE_POOL)
(   UINT32 Type, 
    UINTN Size, 
     void **Buffer 
);



typedef struct {
    UINT16 Year;
    UINT8 Month;
    UINT8 Day;
    UINT8 Hour;
    UINT8 Minute;
    UINT8 Second;
    UINT8 Pad1;
    UINT32 Nanosecond;
    INT16 TimeZone;
    UINT8 Daylight;
    UINT8 Pad2;
} EFI_TIME;

typedef __attribute__((ms_abi)) EFI_STATUS (*EFI_HANDLE_PROTOCOL)(
    EFI_HANDLE Handle,
    EFI_GUID *Protocol,
    void **Interface
);

typedef __attribute__((ms_abi)) EFI_STATUS (*EFI_LOCATE_HANDLE_BUFFER)(
    UINT32 SearchType,
    EFI_GUID *Protocol,
    void *SearchKey,
    UINTN *NoHandles,
    EFI_HANDLE **Buffer
);

typedef struct {
    UINT64 Signature;
    UINT32 Revision;
    UINT32 HeaderSize;
    UINT32 CRC32;
    UINT32 Reserved;
} EFI_TABLE_HEADER;
//efi service created start
typedef struct _EFI_RUNTIME_SERVICES {
    EFI_TABLE_HEADER Hdr;
    //  Remaining members are omitted for brevity
} EFI_RUNTIME_SERVICES;
typedef struct {
    EFI_TABLE_HEADER Hdr;

    // Task Priority Services
    VOID *RaiseTPL;    // EFI_RAISE_TPL
    VOID *RestoreTPL;  // EFI_RESTORE_TPL

    // Memory Services
    EFI_STATUS (EFIAPI *AllocatePages)(UINT32 Type, EFI_MEMORY_TYPE MemoryType, UINTN Pages, EFI_PHYSICAL_ADDRESS *Memory);
    EFI_STATUS (EFIAPI *FreePages)(EFI_PHYSICAL_ADDRESS Memory, UINTN Pages);
    EFI_GET_MEMORY_MAP GetMemoryMap;
    EFI_ALLOCATE_POOL  AllocatePool;
    EFI_STATUS (EFIAPI *FreePool)(VOID *Buffer);

    // Event & Timer Services
    VOID *CreateEvent;  // EFI_CREATE_EVENT
    VOID *SetTimer;     // EFI_SET_TIMER
    VOID *WaitForEvent; // EFI_WAIT_FOR_EVENT
    VOID *SignalEvent;  // EFI_SIGNAL_EVENT
    VOID *CloseEvent;   // EFI_CLOSE_EVENT
    VOID *CheckEvent;   // EFI_CHECK_EVENT

    // Protocol Handler Services
    VOID *InstallProtocolInterface;   // EFI_INSTALL_PROTOCOL_INTERFACE
    VOID *ReinstallProtocolInterface; // EFI_REINSTALL_PROTOCOL_INTERFACE
    VOID *UninstallProtocolInterface; // EFI_UNINSTALL_PROTOCOL_INTERFACE
    EFI_HANDLE_PROTOCOL HandleProtocol;
    VOID *Reserved;                   // 予約済み
    VOID *RegisterProtocolNotify;     // EFI_REGISTER_PROTOCOL_NOTIFY
    VOID *LocateHandle;               // EFI_LOCATE_HANDLE
    VOID *LocateDevicePath;           // EFI_LOCATE_DEVICE_PATH
    VOID *InstallConfigurationTable;  // EFI_INSTALL_CONFIGURATION_TABLE

    // Image Services
    VOID *LoadImage;                  // EFI_IMAGE_LOAD
    VOID *StartImage;                 // EFI_IMAGE_START
    VOID *Exit;                       // EFI_EXIT
    VOID *UnloadImage;                // EFI_IMAGE_UNLOAD
    EFI_STATUS (EFIAPI *ExitBootServices)(EFI_HANDLE ImageHandle, UINTN MapKey);

    // Miscellaneous Services
    VOID *GetNextMonotonicCount;      // EFI_GET_NEXT_MONOTONIC_COUNT
    VOID *Stall;                      // EFI_STALL
    VOID *SetWatchdogTimer;           // EFI_SET_WATCHDOG_TIMER

    // Driver Support Services
    VOID *ConnectController;          // EFI_CONNECT_CONTROLLER
    VOID *DisconnectController;       // EFI_DISCONNECT_CONTROLLER

    // Open and Close Protocol Services
    VOID *OpenProtocol;               // EFI_OPEN_PROTOCOL
    VOID *CloseProtocol;              // EFI_CLOSE_PROTOCOL
    VOID *OpenProtocolInformation;    // EFI_OPEN_PROTOCOL_INFORMATION

    // Library Services
    VOID *ProtocolsPerHandle;         // EFI_PROTOCOLS_PER_HANDLE
    EFI_LOCATE_HANDLE_BUFFER LocateHandleBuffer;
    EFI_LOCATE_PROTOCOL      LocateProtocol;
    VOID *InstallMultipleProtocolInterfaces;   // EFI_INSTALL_MULTIPLE_PROTOCOL_INTERFACES
    VOID *UninstallMultipleProtocolInterfaces; // EFI_UNINSTALL_MULTIPLE_PROTOCOL_INTERFACES

    // 32-bit CRC Services
    VOID *CalculateCrc32;             // EFI_CALCULATE_CRC32

    // Miscellaneous Services (Cont'd)
    VOID (EFIAPI *CopyMem)(VOID *Destination, const VOID *Source, UINTN Length);
    VOID (EFIAPI *SetMem)(VOID *Buffer, UINTN Size, UINT8 Value);
    VOID *CreateEventEx;              // EFI_CREATE_EVENT_EX
} EFI_BOOT_SERVICES;

//file protocol create
typedef __attribute__((ms_abi)) EFI_STATUS (EFIAPI *EFI_FILE_CLOSE)(
    void* File
);
typedef __attribute__((ms_abi)) EFI_STATUS (EFIAPI *EFI_FILE_DELETE)(
    void* File
);
typedef __attribute__((ms_abi)) EFI_STATUS (EFIAPI *EFI_FILE_READ)(
    void* File,
    UINTN *BufferSize,
    void* Buffer
);
typedef __attribute__((ms_abi)) EFI_STATUS (EFIAPI *EFI_FILE_WRITE)(
    void* File,
    UINTN *BufferSize,
    void* Buffer
);
typedef __attribute__((ms_abi)) EFI_STATUS (EFIAPI *EFI_FILE_GET_POSITION)(
    void* File,
    UINT64 *Position
);
typedef __attribute__((ms_abi)) EFI_STATUS (EFIAPI *EFI_FILE_SET_POSITION)(
    void* File,
    UINT64 Position
);
typedef __attribute__((ms_abi)) EFI_STATUS (EFIAPI *EFI_FILE_GET_INFO)(
 
  VOID     *This,            // RCX
  EFI_GUID *InformationType, // RDX (ここが 0 にならないように！)
  UINTN    *BufferSize,      // R8
  VOID     *Buffer           // R9
);


typedef __attribute__((ms_abi)) EFI_STATUS (EFIAPI *EFI_FILE_SET_INFO)(
    void* File,
    void* InformationType,
    UINTN BufferSize,
    void* Buffer
);
typedef __attribute__((ms_abi)) EFI_STATUS (EFIAPI *EFI_FILE_FLUSH)(
    void* File
);
typedef __attribute__((ms_abi)) EFI_STATUS (EFIAPI *EFI_FILE_OPEN_EX)(
    void* File,
    void** NewHandle,
    CHAR16 *FileName,
    UINT64 OpenMode,
    UINT64 Attributes,
    void* Token
);
typedef __attribute__((ms_abi)) EFI_STATUS (EFIAPI *EFI_FILE_READ_EX)(
    void* File,
    void* Token
);
typedef __attribute__((ms_abi)) EFI_STATUS (EFIAPI *EFI_FILE_WRITE_EX)(
    void* File,
    void* Token
);
typedef __attribute__((ms_abi)) EFI_STATUS (EFIAPI *EFI_FILE_FLUSH_EX)(
    void* File,
    void* Token
);

typedef __attribute__((ms_abi)) EFI_STATUS (EFIAPI *EFI_FILE_OPEN)(
    void* File,
    void** NewHandle,
    CHAR16 *FileName,
    UINT64 OpenMode,
    UINT64 Attributes
);

typedef __attribute__((ms_abi)) EFI_STATUS (EFIAPI *EFI_OPEN_VOLUME)(
    void* This,
    void** Root
);

typedef struct EFI_FILE_PROTOCOL {
  UINT64                          Revision;
  EFI_FILE_OPEN                   Open;
  EFI_FILE_CLOSE                  Close;
  EFI_FILE_DELETE                 Delete;
  EFI_FILE_READ                   Read;
  EFI_FILE_WRITE                  Write;
  EFI_FILE_GET_POSITION           GetPosition;
  EFI_FILE_SET_POSITION           SetPosition;
  EFI_FILE_GET_INFO               GetInfo;
  EFI_FILE_SET_INFO               SetInfo;
  EFI_FILE_FLUSH                  Flush;
  EFI_FILE_OPEN_EX                OpenEx; // Added for revision 2
  EFI_FILE_READ_EX                ReadEx; // Added for revision 2
  EFI_FILE_WRITE_EX               WriteEx; // Added for revision 2
  EFI_FILE_FLUSH_EX               FlushEx; // Added for revision 2
} EFI_FILE_PROTOCOL;
//file protocol create s end
typedef struct {
    UINT64 VendorGuid[2];
    void* VendorTable;
} EFI_CONFIGURATION_TABLE;

typedef struct {
    UINT64 Size;
    UINT64 FileSize;
    UINT64 PhysicalSize;
    EFI_TIME CreateTime;
    EFI_TIME LastAccessTime;
    EFI_TIME ModificationTime;
    UINT64 Attribute;
    CHAR16 FileName[256];
} EFI_FILE_INFO;

typedef struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;

// 関数の型定義（EFIAPI/ms_abi を忘れずに！）
typedef EFI_STATUS (EFIAPI *EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_OPEN_VOLUME)(
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *This,
    EFI_FILE_PROTOCOL              **Root
);

// 構造体の完全な定義
struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL {
    UINT64                                     Revision;    // 0x00
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_OPEN_VOLUME OpenVolume;  // 0x08
};

// ConOut create

struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL;
typedef __attribute__((ms_abi)) EFI_STATUS (*EFI_INPUT_READ_KEY)(
    struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL *This,
    void* /* EFI_INPUT_KEY */ key
);
typedef struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL {
    EFI_INPUT_READ_KEY ReadKeyStroke; //get keystroke
} EFI_SIMPLE_TEXT_INPUT_PROTOCOL;

typedef struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
    unsigned long long (*Reset)(struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, unsigned char ExtendedVerification);
    // 引数を unsigned short * に変更
    unsigned long long (*OutputString)(struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, unsigned short *String);
    unsigned long long (*TestString)(struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, unsigned short *String);
    unsigned long long (*QueryMode)(struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, unsigned long long ModeNumber, unsigned long long *Columns, unsigned long long *Rows);
    unsigned long long (*SetMode)(struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, unsigned long long ModeNumber);
    unsigned long long (*SetAttribute)(struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, unsigned long long Attribute);
    unsigned long long (*ClearScreen)(struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This);
} EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;






// SystemTable create
typedef struct {
    EFI_TABLE_HEADER                 Hdr;
    CHAR16                           *FirmwareVendor;
    UINT32                           FirmwareRevision;
    EFI_HANDLE                       ConsoleInHandle;
    EFI_SIMPLE_TEXT_INPUT_PROTOCOL   *ConIn;
    EFI_HANDLE                       ConsoleOutHandle;
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *ConOut;
    EFI_HANDLE                       StandardErrorHandle;
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *StdErr;
    EFI_RUNTIME_SERVICES             *RuntimeServices;
    EFI_BOOT_SERVICES                *BootServices;
    UINTN                            NumberOfTableEntries;
    EFI_CONFIGURATION_TABLE          *ConfigurationTable;
} EFI_SYSTEM_TABLE;
    extern EFI_BOOT_SERVICES *gBS;



typedef enum {
  PixelRedGreenBlueReserved8BitPerColor,
  PixelBlueGreenRedReserved8BitPerColor,
  PixelBitMask,
  PixelBltOnly,
  PixelFormatMax
} EFI_GRAPHICS_PIXEL_FORMAT;

typedef struct { UINT32 RedMask;
    UINT32 GreenMask;
    UINT32 BlueMask;
    UINT32 ReservedMask;
    } EFI_PIXEL_BITMASK;

    typedef struct {
  UINT32 Version;
  UINT32 HorizontalResolution;
  UINT32 VerticalResolution;
  EFI_GRAPHICS_PIXEL_FORMAT PixelFormat;
  EFI_PIXEL_BITMASK PixelInformation;
  UINT32 PixelsPerScanLine;
} EFI_GRAPHICS_OUTPUT_MODE_INFORMATION;

typedef struct {
  UINT32 MaxMode;
  UINT32 Mode;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;
  UINTN SizeOfInfo;
  EFI_PHYSICAL_ADDRESS FrameBufferBase;
  UINTN FrameBufferSize;
} EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;

typedef struct _EFI_GRAPHICS_OUTPUT_PROTOCOL EFI_GRAPHICS_OUTPUT_PROTOCOL;
struct _EFI_GRAPHICS_OUTPUT_PROTOCOL {
  EFI_STATUS (*QueryMode)(
    struct _EFI_GRAPHICS_OUTPUT_PROTOCOL *This,
    UINT32 ModeNumber,
    UINTN *SizeOfInfo,
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION **Info
    );
  EFI_STATUS (*SetMode)(
    struct _EFI_GRAPHICS_OUTPUT_PROTOCOL *This,
    UINT32 ModeNumber
    );
  EFI_STATUS (*Blt)(
    struct _EFI_GRAPHICS_OUTPUT_PROTOCOL *This,
    void *BltBuffer,
    UINT32 BltOperation,
    UINTN SourceX,
    UINTN SourceY,
    UINTN DestinationX,
    UINTN DestinationY,
    UINTN Width,
    UINTN Height,
    UINTN Delta
    );
  EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE *Mode;
};

//efi service created end

// Loaded Image Protocol
typedef struct {
    EFI_TABLE_HEADER                Hdr;
    EFI_HANDLE                      ParentHandle;
    EFI_SYSTEM_TABLE                *SystemTable;
    EFI_HANDLE                      DeviceHandle;
    void                            *FilePath;  // EFI_DEVICE_PATH_PROTOCOL
    void                            *Reserved;
    UINT32                          LoadOptionsSize;
    void                            *LoadOptions;
    void                            *ImageBase;
    UINT64                          ImageSize;
    EFI_MEMORY_TYPE                 ImageCodeType;
    EFI_MEMORY_TYPE                 ImageDataType;
    EFI_STATUS (*Unload)(EFI_HANDLE ImageHandle);
} EFI_LOADED_IMAGE_PROTOCOL;
void PrintDecimal(UINTN value, EFI_SYSTEM_TABLE *SystemTable);
    

// GUIDs
extern EFI_GUID gEfiLoadedImageProtocolGuid;
extern EFI_GUID gEfiSimpleFileSystemProtocolGuid;
extern EFI_GUID gEfiFileInfoGuid;

// File open modes
#define EFI_FILE_MODE_READ   0x0000000000000001ULL
#define EFI_FILE_MODE_WRITE  0x0000000000000002ULL
#define EFI_FILE_MODE_CREATE 0x8000000000000000ULL

// Locate Handle Search Types
#define AllHandles 0
#define ByRegisterNotify 1
#define ByProtocol 2



// Status codes
#define EFI_SUCCESS 0
#define EFI_LOAD_ERROR       0x8000000000000001ULL
#define EFI_BUFFER_TOO_SMALL 5
//GOP protocol GUID
#define EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID \
  { 0x9042a9de, 0x23dc, 0x4a38, { 0x96, 0xfb, 0x7a, 0xde, 0xd0, 0x80, 0x51, 0x6a } }
  extern EFI_GUID gEfiGraphicsOutputProtocolGuid;
extern __attribute__((ms_abi)) EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable);



extern EFI_GUID gEfiLoadedImageProtocolGuid;
extern EFI_GUID gEfiSimpleFileSystemProtocolGuid;
extern EFI_GUID gEfiFileInfoGuid;
extern EFI_GUID gEfiGraphicsOutputProtocolGuid;
extern EFI_GUID gopGuid;



#endif /* EFI_H_ */