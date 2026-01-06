#include <stdint.h>

#define NULL ((void*)0)
#define COM1_PORT 0x3F8

// UEFIの基本型定義
typedef unsigned long long UINTN;
typedef unsigned int UINT32;
typedef unsigned long long UINT64;
typedef unsigned short UINT16;
typedef unsigned char UINT8;
typedef UINT64 EFI_PHYSICAL_ADDRESS;
typedef UINT64 EFI_VIRTUAL_ADDRESS;
typedef UINTN EFI_STATUS;

typedef struct {
    UINT32 Type;
    UINT32 Pad;
    EFI_PHYSICAL_ADDRESS PhysicalStart;
    EFI_VIRTUAL_ADDRESS VirtualStart;
    UINT64 NumberOfPages;
    UINT64 Attribute;
} EFI_MEMORY_DESCRIPTOR;

typedef struct {
    char dummy1[24]; // 前の方にある他の関数を飛ばすためのダミー
    
    // GetMemoryMap関数の場所（プロトタイプ定義）
    EFI_STATUS (*GetMemoryMap)(
        UINTN *MemoryMapSize,
        EFI_MEMORY_DESCRIPTOR *MemoryMap,
        UINTN *MapKey,
        UINTN *DescriptorSize,
        UINT32 *DescriptorVersion
    );

    // AllocatePages関数の場所
    EFI_STATUS (*AllocatePages)(
        UINT32 Type,
        UINT32 MemoryType,
        UINTN Pages,
        EFI_PHYSICAL_ADDRESS *Memory
    );
    

    char dummy2[104]; // 間の関数を飛ばす（調整済み）

    // AllocatePool関数の場所
    EFI_STATUS (*AllocatePool)(
        UINT32 PoolType,
        UINTN Size,
        void **Buffer
    );
} EFI_BOOT_SERVICES;
int is_transmit_empty(){

    //ダミー関数
    return inb(COM1_PORT + 5) & 0x20;

}



extern char kernel_stack_bottom[];
//　シリアル出力　の実装関数
static inline UINT8 inb(UINT16 port){
    UINT8 data;

    _asm_ _volatile_("inb %w1, %0" : "=a"(data) : "Nd"(port));
    return data;
}
//inlineの定義１００
static inline void outb(UINT16 port, UINT8 data ){

_asm_ _volatile_("outb %b0, %w1" : : "a"(data), "Nd"(port));

}



    void serial_write_char(char c) {

        //データ送信可能になるまで待つ
        //is_transmit_empty()関数はUEFIのじてんでじっそうさらてる
        //is_transmit_empty()がエンプティじゃなくなる時まで個々のワイルで待機
while (!is_transmit_empty());

asm volatile("outb %0, %1" : : "a"(c), "dN"(0x3F8));



    };

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

typedef struct _EFI_RUNTIME_SERVICES {
    EFI_TABLE_HEADER Hdr;
    // 他のメンバーは省略
} EFI_RUNTIME_SERVICES;

typedef struct {
    UINT64 VendorGuid[2];
    void* VendorTable;
} EFI_CONFIGURATION_TABLE;

// ConOut（画面出力）プロトコルの定義
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
    EFI_INPUT_READ_KEY ReadKeyStroke; //これが入力の実体
} EFI_SIMPLE_TEXT_INPUT_PROTOCOL;

typedef struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
    void*           _unused;
    EFI_TEXT_STRING OutputString; // これがPrintの実体
} EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

// SystemTable（UEFIの全機能への入り口）の定義
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


// エントリポイント
__attribute__((ms_abi))

EFI_STATUS EFI_Main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {

BootServices = SystemTable->BootServices;

    // メモリマップを取得
    UINTN MemoryMapSize = 0;
    EFI_MEMORY_DESCRIPTOR *MemoryMap = NULL;
    UINTN MapKey;
    UINTN DescriptorSize;
    UINT32 DescriptorVersion;
    EFI_STATUS Status;

    // サイズを取得
    Status = BootServices->GetMemoryMap(&MemoryMapSize, NULL, &MapKey, &DescriptorSize, &DescriptorVersion);
    if (Status != 0) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16 *)L"GetMemoryMap size failed\n");
        while (1);
    }

    // バッファ確保
    Status = BootServices->AllocatePool(0, MemoryMapSize, (void **)&MemoryMap);
    if (Status != 0) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16 *)L"AllocatePool failed\n");
        while (1);
    }

    // メモリマップ取得
    Status = BootServices->GetMemoryMap(&MemoryMapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion);
    if (Status != 0) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16 *)L"GetMemoryMap failed\n");
        while (1);
    }

    // カーネルを置くメモリ確保 (例: 1ページ)
    EFI_PHYSICAL_ADDRESS KernelAddress;
    UINTN Pages = 1;
    Status = BootServices->AllocatePages(0, 1, Pages, &KernelAddress);
    if (Status != 0) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16 *)L"AllocatePages failed\n");
        while (1);
    }

    SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16 *)L"Hello");
    while (1){
SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16 *)L"kernel check");

    }
   
    
}