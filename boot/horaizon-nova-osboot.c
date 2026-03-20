
#include "../includemyos/efi.h"
#include "../includemyos/framebuffer.h"
#include "../includemyos/kernel/guid.h"

#include <stdint.h>

#ifndef NULL
#define NULL ((void *)0)
#endif
#define KERNEL_LOAD_ADDRESS 0x100000 // カーネルのロードアドレス
typedef int64_t INTN;                // 64bit UEFI の場合
// EFI SERVICE is created in bootinclude/myos/efi.h


// エントリポイント
EFI_BOOT_SERVICES *BootServices;
EFI_SYSTEM_TABLE *SystemTable;

extern __attribute__((ms_abi)) EFI_STATUS efi_main(EFI_HANDLE ImageHandle,
                                                   EFI_SYSTEM_TABLE *ST) {

    SystemTable = ST;
    BootServices = ST->BootServices;
    EFI_STATUS status = 0;
    void *fontBuffer = NULL;
    UINTN fontSize = 0;
    UINTN MemoryMapSize = 0;
    UINTN DescriptorSize = 0;
    UINT32 DescriptorVersion = 0;
    
    SystemTable->ConOut->OutputString( SystemTable->ConOut,L"=== Horizon Nova OS Boot Loader ===\n");
    SystemTable->ConOut->OutputString(SystemTable->ConOut,L"start GOP\n"); 
    status = BootServices->GetMemoryMap(&MemoryMapSize, NULL, NULL,
                                        &DescriptorSize, NULL);

    if (MemoryMapSize == 0) {
        MemoryMapSize = 4096;
        DescriptorSize = 48;
    }   
    extern EFI_GUID gEfiGraphicsOutputProtocolGuid; 
EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
gop = NULL;
if(gop != NULL){
  SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16 *)L"GOP is not NULL at declaration\n");
}
else{
    SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16 *)L"GOP is NULL at declarationtest\n");
}

  EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    SystemTable->ConOut->OutputString(SystemTable->ConOut,(CHAR16 *)L"geting GOP test\n");
    status = SystemTable->BootServices->LocateProtocol(  
    &gopGuid,
    NULL,
    (void **)&gop);
 
    

    SystemTable->ConOut->OutputString(    SystemTable->ConOut,(CHAR16 *)L"geting GOP test2\n");
         // この後にバグあり
if (status == 0 && gop != NULL) { // 成功かつポインタがあるなら
    SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16 *)L"GOP SUCCESS!\n");
} else {
    SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16 *)L"GOP FAILED...\n");
    
}

    SystemTable->ConOut->OutputString(SystemTable->ConOut,
                                      (CHAR16 *)L"GOP located successfully\n");
    FramebufferInfo fbinfo;
    fbinfo.framebuffer = (void *)gop->Mode->FrameBufferBase;
    fbinfo.Width = gop->Mode->Info->HorizontalResolution;
    fbinfo.Height = gop->Mode->Info->VerticalResolution;
    fbinfo.Pixels_Per_ScanLine = gop->Mode->Info->PixelsPerScanLine;
    fbinfo.font = fontBuffer;
    fbinfo.font_size = fontSize;
    SystemTable->ConOut->OutputString(SystemTable->ConOut,
                                      (CHAR16 *)L"geting GOP test3.1\n");
    // LocateHandleBufferで全SimpleFileSystemハンドルを探す
    EFI_HANDLE *handles = NULL;
    UINTN handleCount = 0;
   
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs = NULL;
    SystemTable->ConOut->OutputString(
        SystemTable->ConOut,
        (CHAR16 *)L"geting GOP test3.2\n"); // この前のところにバグあり
    status = BootServices->LocateHandleBuffer(ByProtocol,
                                              &gEfiSimpleFileSystemProtocolGuid,
                                              NULL, &handleCount, &handles);

    if (status == EFI_SUCCESS && handleCount > 0) {
        // 最初のファイルシステムハンドルを使用
        status = BootServices->HandleProtocol(
            handles[0], &gEfiSimpleFileSystemProtocolGuid, (void **)&fs);

        if (status == EFI_SUCCESS && fs != NULL) {
            SystemTable->ConOut->OutputString(
                SystemTable->ConOut,
                L"FileSystem found via LocateHandleBuffer\n");
        } else {
            SystemTable->ConOut->OutputString(
                SystemTable->ConOut, L"ERROR: HandleProtocol failed\n");
            goto error_to_jump;
        }
    } else {
        SystemTable->ConOut->OutputString(
            SystemTable->ConOut,
            L"FileSystem not found, assuming kernel in memory\n");
        goto error_to_jump;
    }
   
    
    // ルートボリュームを開く
    
    EFI_FILE_PROTOCOL *root = NULL;
    error_to_jump:
    status = fs->OpenVolume(fs, (void **)&root);
    if (status != EFI_SUCCESS) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut,
                                          L"ERROR: OpenVolume failed\n");
        return status;
    }
// --- 1. まずカーネルファイルを開く ---
EFI_FILE_PROTOCOL *kernelFile = NULL;
status = root->Open(root, (void **)&kernelFile, L"myoskernel.bin", EFI_FILE_MODE_READ, 0);

if (status != EFI_SUCCESS) {
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"ERROR: Kernel file not found\r\n");
    return status;
}
SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Kernel file opened\r\n");

// --- 2. ファイル情報を取得するためにサイズを調べる ---
UINTN infosize = 0;
EFI_FILE_INFO *fileInfo = NULL;

// 1回目のGetInfo（サイズ取得用）
status = kernelFile->GetInfo(kernelFile, &gEfiFileInfoGuid, &infosize, (void *)0);

if (status != EFI_BUFFER_TOO_SMALL) {
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"ERROR: GetInfo size failed\r\n");
    return status;
}

// --- 3. メモリ確保とファイル情報の書き込み ---
status = BootServices->AllocatePool(EfiLoaderData, infosize, (void **)&fileInfo);
if (status != EFI_SUCCESS) {
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"ERROR: AllocatePool failed\r\n");
    return status;
}

// 2回目のGetInfo（実際のデータ書き込み）
status = kernelFile->GetInfo(kernelFile, &gEfiFileInfoGuid, &infosize, fileInfo);
if (status != EFI_SUCCESS) {
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"ERROR: GetInfo failed\r\n");
    return status;
}


UINTN kernelSize = fileInfo->FileSize; 

// --- 4. フォントファイルを開く（同じ手順） ---
EFI_FILE_PROTOCOL *fontfile = NULL;
status = root->Open(root, (void **)&fontfile, L"EFI\\BOOT\\solarize-12x29-psf", EFI_FILE_MODE_READ, 0); // バックスラッシュに注意
if (status != EFI_SUCCESS) {
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"ERROR: Failed to open font file\r\n");
}

    if (EFI_ERROR(status)) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut,
                                          L"ERROR: Failed to open font file\n");
        return status;
    }
    // フォントファイルサイズを取得
    UINTN fontInfoSize = 0;
    EFI_FILE_INFO *fontFileInfo = NULL;

    status =
        fontfile->GetInfo(fontfile, &gEfiFileInfoGuid, &fontInfoSize, NULL);
    if (status != EFI_BUFFER_TOO_SMALL) {
        SystemTable->ConOut->OutputString(
            SystemTable->ConOut,
            L"ERROR: GetInfo size for font failed\n"); // sagyou
        return status;
    }

    status = BootServices->AllocatePool(EfiLoaderData, fontInfoSize,
                                        (void **)&fontFileInfo);
    if (EFI_ERROR(status)) {
        SystemTable->ConOut->OutputString(
            SystemTable->ConOut, L"ERROR: AllocatePool for font info failed\n");
        return status;
    }

    status = fontfile->GetInfo(fontfile, &gEfiFileInfoGuid, &fontInfoSize,
                               fontFileInfo);
    if (EFI_ERROR(status)) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut,
                                          L"ERROR: GetInfo for font failed\n");
        return status;
    }

    fontSize = fontFileInfo->FileSize;
    fontBuffer = NULL;
    status = BootServices->AllocatePool(EfiLoaderData, fontSize, &fontBuffer);
    if (EFI_ERROR(status)) {
        SystemTable->ConOut->OutputString(
            SystemTable->ConOut,
            L"ERROR: AllocatePool for font buffer failed\n");
        return status;
    }

    status = fontfile->Read(fontfile, &fontSize, fontBuffer);
    if (EFI_ERROR(status)) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut,
                                          L"ERROR: Read font file failed\n");
        return status;
    }
    SystemTable->ConOut->OutputString(SystemTable->ConOut,
                                      L"Font size after read: ");
    EFI_PHYSICAL_ADDRESS safeFontAddr = 0x1000000;
    UINTN fontPages = (fontSize + 0xFFF) / 0x1000;

    status = BootServices->AllocatePages(AllocateAddress, EfiLoaderData,
                                         fontPages, &safeFontAddr);
    BootServices->CopyMem((void *)safeFontAddr, fontBuffer, fontSize);
    fbinfo.font = (void *)safeFontAddr;
    fbinfo.font_size = fontSize;

    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Font loaded\n");

 
    SystemTable->ConOut->OutputString(SystemTable->ConOut,
                                      L"Kernel size obtained\n");
    // Horizon-NOVA-OS (alpha0.1) 16KBページ管理対応版
    EFI_PHYSICAL_ADDRESS kernelAddr = KERNEL_LOAD_ADDRESS; // 0x100000

    // 16KB（0x4000）単位で計算
    UINTN units16kb = (kernelSize + 0x3FFF) / 0x4000; 
    UINTN totalPages = units16kb * 4; // 1ユニットあたり4つのUEFI標準ページ(4KB)

    status = BootServices->AllocatePages(AllocateAddress, EfiLoaderData, totalPages, &kernelAddr);

    if (status != EFI_SUCCESS) {
     SystemTable->ConOut->OutputString(SystemTable->ConOut, L"ERROR: 16KB aligned block at 1MB busy!\r\n");
     return status;
   }



   
    
    // カーネルを読み込む
    status =
        kernelFile->Read(kernelFile, &kernelSize, (void *)kernelAddr);

    if (status != EFI_SUCCESS) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut,
                                          L"ERROR: Read kernel failed\n");
        return status;
    }

    SystemTable->ConOut->OutputString(SystemTable->ConOut,
                                      L"Kernel loaded to memory\n");

    // メモリマップを取得してExitBootServices
    

    status = BootServices->GetMemoryMap(&MemoryMapSize, NULL, NULL,
                                        &DescriptorSize, NULL);

    if (MemoryMapSize == 0) {
        MemoryMapSize = 4096;
        DescriptorSize = 48;
    }

    int retry = 0;
    while (retry < 5) {
        retry++;

        EFI_MEMORY_DESCRIPTOR *MemoryMap = NULL;
        UINTN MapKey = 0;

        UINTN AllocSize = MemoryMapSize + (DescriptorSize * 20);
        status = BootServices->AllocatePool(EfiLoaderData, AllocSize,
                                            (void **)&MemoryMap);

        if (status != EFI_SUCCESS) {
            SystemTable->ConOut->OutputString(
                SystemTable->ConOut, L"AllocatePool for memorymap failed\n");
            continue;
        }

        status =
            BootServices->GetMemoryMap(&MemoryMapSize, MemoryMap, &MapKey,
                                       &DescriptorSize, &DescriptorVersion);

        if (status != EFI_SUCCESS) {
            SystemTable->ConOut->OutputString(SystemTable->ConOut,
                                              L"GetMemoryMap failed\n");
            continue;
        }

        status = BootServices->ExitBootServices(ImageHandle, MapKey);
        if (status == EFI_SUCCESS) {
            SystemTable->ConOut->OutputString(SystemTable->ConOut,
                                              L"ExitBootServices succeeded\n");
            break;
        }
    }


    SystemTable->ConOut->OutputString(SystemTable->ConOut,
                                      L"Jumping to kernel...\n");

    typedef void (*kernelEntry)(FramebufferInfo *);
    kernelEntry entry = (kernelEntry)(KERNEL_LOAD_ADDRESS);
    entry(&fbinfo);

    return EFI_SUCCESS;
}
                                                   