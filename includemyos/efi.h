
#ifndef EFI_H_
#define EFI_H_
typedef unsigned long long UINTN;
typedef unsigned int UINT32;
typedef unsigned long long UINT64;
typedef unsigned short UINT16;
typedef unsigned char UINT8;
typedef UINT64 EFI_PHYSICAL_ADDRESS;
typedef UINT64 EFI_VIRTUAL_ADDRESS;
typedef UINTN EFI_STATUS;

// UEFIの基本型
typedef unsigned short CHAR16;
typedef void*          EFI_HANDLE;

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
typedef struct _EFI_BOOT_SERVICES {
    EFI_TABLE_HEADER Hdr;
    //  Remaining members are omitted for brevity
} EFI_BOOT_SERVICES;

typedef struct {
    UINT64 VendorGuid[2];
    void* VendorTable;
} EFI_CONFIGURATION_TABLE;

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

extern EFI_SYSTEM_TABLE *SystemTable;

EFI_BOOT_SERVICES *BootServices;
extern EFI_STATUS EFI_Main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable);
#endif /* EFI_H_ */