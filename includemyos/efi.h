
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
    UINT8 Data[16];
} EFI_GUID;

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
typedef struct EFI_BOOT_SERVICES {
    EFI_TABLE_HEADER Hdr;
    void *RaiseTPL;
    void *RestoreTPL; 
    void *AllocatePages; 
    void *FreePages;
    EFI_GET_MEMORY_MAP GetMemoryMap;
    EFI_LOCATE_HANDLE_BUFFER LocateHandleBuffer;
    EFI_STATUS (*ExitBootServices)(
        EFI_HANDLE ImageHandle,
        UINTN MapKey
    );
    EFI_ALLOCATE_POOL AllocatePool;
    EFI_HANDLE_PROTOCOL HandleProtocol;

    //  Remaining members are omitted for brevity
} EFI_BOOT_SERVICES;
//file protocol create
typedef __attribute__((ms_abi)) EFI_STATUS (*EFI_FILE_CLOSE)(
    void* File
);
typedef __attribute__((ms_abi)) EFI_STATUS (*EFI_FILE_DELETE)(
    void* File
);
typedef __attribute__((ms_abi)) EFI_STATUS (*EFI_FILE_READ)(
    void* File,
    UINTN *BufferSize,
    void* Buffer
);
typedef __attribute__((ms_abi)) EFI_STATUS (*EFI_FILE_WRITE)(
    void* File,
    UINTN *BufferSize,
    void* Buffer
);
typedef __attribute__((ms_abi)) EFI_STATUS (*EFI_FILE_GET_POSITION)(
    void* File,
    UINT64 *Position
);
typedef __attribute__((ms_abi)) EFI_STATUS (*EFI_FILE_SET_POSITION)(
    void* File,
    UINT64 Position
);
typedef __attribute__((ms_abi)) EFI_STATUS (*EFI_FILE_GET_INFO)(
    void* File,
    void* InformationType,
    UINTN *BufferSize,
    void* Buffer
);
typedef __attribute__((ms_abi)) EFI_STATUS (*EFI_FILE_SET_INFO)(
    void* File,
    void* InformationType,
    UINTN BufferSize,
    void* Buffer
);
typedef __attribute__((ms_abi)) EFI_STATUS (*EFI_FILE_FLUSH)(
    void* File
);
typedef __attribute__((ms_abi)) EFI_STATUS (*EFI_FILE_OPEN_EX)(
    void* File,
    void** NewHandle,
    CHAR16 *FileName,
    UINT64 OpenMode,
    UINT64 Attributes,
    void* Token
);
typedef __attribute__((ms_abi)) EFI_STATUS (*EFI_FILE_READ_EX)(
    void* File,
    void* Token
);
typedef __attribute__((ms_abi)) EFI_STATUS (*EFI_FILE_WRITE_EX)(
    void* File,
    void* Token
);
typedef __attribute__((ms_abi)) EFI_STATUS (*EFI_FILE_FLUSH_EX)(
    void* File,
    void* Token
);

typedef __attribute__((ms_abi)) EFI_STATUS (*EFI_FILE_OPEN)(
    void* File,
    void** NewHandle,
    CHAR16 *FileName,
    UINT64 OpenMode,
    UINT64 Attributes
);

typedef __attribute__((ms_abi)) EFI_STATUS (*EFI_OPEN_VOLUME)(
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

typedef struct {
    EFI_OPEN_VOLUME OpenVolume;
} EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;

// ConOut create
struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;
typedef __attribute__((ms_abi)) EFI_STATUS (*EFI_TEXT_STRING)(
    struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, 
    CHAR16 *String
);
struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL;
typedef __attribute__((ms_abi)) EFI_STATUS (*EFI_INPUT_READ_KEY)(
    struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL *This,
    void* /* EFI_INPUT_KEY */ key
);
typedef struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL {
    EFI_INPUT_READ_KEY ReadKeyStroke; //get keystroke
} EFI_SIMPLE_TEXT_INPUT_PROTOCOL;

typedef struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
    void*           _unused;
    EFI_TEXT_STRING OutputString; // print string
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
#define EFI_BUFFER_TOO_SMALL 5

extern __attribute__((ms_abi)) EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable);
#endif /* EFI_H_ */