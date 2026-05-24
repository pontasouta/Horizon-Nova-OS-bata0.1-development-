#include "../includemyos/efi.h"
#include "../includemyos/framebuffer.h"
#include "../includemyos/guid.h"
#include "../includemyos/elf.h"
#include "../includemyos/printhexstatus.h"

#include <stdint.h>

#ifndef NULL
#define NULL ((void *)0)
#endif

#define KERNEL_LOAD_ADDRESS 0x100000
typedef int64_t INTN;

extern EFI_GUID gEfiFileInfoGuid;
EFI_BOOT_SERVICES *gBS; 

EFI_BOOT_SERVICES *BootServices;
EFI_SYSTEM_TABLE *SystemTable;
static EFI_STATUS GetFileInfo(EFI_FILE_PROTOCOL *file, EFI_FILE_INFO **outInfo);
  


extern __attribute__((ms_abi)) EFI_STATUS efi_main(EFI_HANDLE ImageHandle,
                                                   EFI_SYSTEM_TABLE *ST) {
    SystemTable = ST;
    BootServices = ST->BootServices;
    gBS = SystemTable->BootServices;
    EFI_STATUS status = 0;
    void *fontBuffer = NULL;
    UINTN fontSize = 0;
    UINTN MemoryMapSize = 0;
    UINTN DescriptorSize = 0;
    UINT32 DescriptorVersion = 0;

    SystemTable->ConOut->OutputString(SystemTable->ConOut,
        L"=== Horizon Nova OS Boot Loader ===\n");

    status = BootServices->GetMemoryMap(&MemoryMapSize, NULL, NULL,
                                        &DescriptorSize, NULL);
    if (MemoryMapSize == 0) {
        MemoryMapSize = 4096;
        DescriptorSize = 48;
    }

    // ========== GOP 取得 ==========
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Getting GOP...\n");

    extern EFI_GUID gEfiGraphicsOutputProtocolGuid;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = NULL;

    status = SystemTable->BootServices->LocateProtocol(
        &gEfiGraphicsOutputProtocolGuid, NULL, (void **)&gop);

    if (status != EFI_SUCCESS || gop == NULL) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"ERROR: GOP failed\n");
        return status;
    }
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"GOP SUCCESS!\n");

    FramebufferInfo fbinfo;
    fbinfo.framebuffer = (void *)gop->Mode->FrameBufferBase;
    fbinfo.Width = gop->Mode->Info->HorizontalResolution;
    fbinfo.Height = gop->Mode->Info->VerticalResolution;
    fbinfo.Pixels_Per_ScanLine = gop->Mode->Info->PixelsPerScanLine;
    fbinfo.font = NULL;
    fbinfo.font_size = 0;

    // ========== ファイルシステム取得 ==========
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Finding filesystem...\n");

    EFI_HANDLE *handles = NULL;
    UINTN handleCount = 0;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs = NULL;

    status = BootServices->LocateHandleBuffer(ByProtocol,
                                              &gEfiSimpleFileSystemProtocolGuid,
                                              NULL, &handleCount, &handles);
    if (status != EFI_SUCCESS || handleCount == 0) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut,
            L"ERROR: FileSystem not found\n");
        return status;
    }

    status = BootServices->HandleProtocol(
        handles[0], &gEfiSimpleFileSystemProtocolGuid, (void **)&fs);
    if (status != EFI_SUCCESS || fs == NULL) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut,
            L"ERROR: HandleProtocol failed\n");
        return status;
    }

    // ========== ルートボリュームを開く ==========
    EFI_FILE_PROTOCOL *root = NULL;
    status = fs->OpenVolume(fs, &root);
    if (status != EFI_SUCCESS) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut,
            L"ERROR: OpenVolume failed\n");
        return status;
    }
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"FileSystem OK\n");

    // ========== カーネルファイルを開く ==========
    EFI_FILE_PROTOCOL *kernelFile = NULL;
    status = root->Open(root, (void **)&kernelFile, L"myoskernel.elf",EFI_FILE_MODE_READ, 0);
    if (status != EFI_SUCCESS) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut,L"ERROR: Kernel file not found\n");
        return status;
    }
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Kernel file opened\n");

    // ========== カーネルファイル情報を取得 ==========
    
    EFI_FILE_INFO *fileInfo = NULL;
    status = GetFileInfo(kernelFile, &fileInfo);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"testpoint1\n");

    if (status != EFI_SUCCESS) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"testpoint1\n");
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"ERROR: GetInfo for kernel failed\n");
        return status;
    }
    UINTN kernelSize = fileInfo->FileSize;
    BootServices->FreePool(fileInfo);




    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Kernel size obtained\n");

    // ========== フォントファイルを開く ==========
    EFI_FILE_PROTOCOL *fontfile = NULL;
    status = root->Open(root, (void **)&fontfile,
                        L"EFI\\BOOT\\solarize-12x29-psf",
                        EFI_FILE_MODE_READ, 0);
    if (status != EFI_SUCCESS) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut,
            L"WARNING: Font file not found, continuing without font\n");
    } else {
        EFI_FILE_INFO *fontFileInfo = NULL;
        status = GetFileInfo(fontfile, &fontFileInfo);
        if (status != EFI_SUCCESS) {
            SystemTable->ConOut->OutputString(SystemTable->ConOut,
                L"ERROR: GetInfo for font failed\n");
            return status;
        }

        fontSize = fontFileInfo->FileSize;
        BootServices->FreePool(fontFileInfo);

        status = BootServices->AllocatePool(EfiLoaderData, fontSize, &fontBuffer);
        if (EFI_ERROR(status)) {
            SystemTable->ConOut->OutputString(SystemTable->ConOut,
                L"ERROR: AllocatePool for font failed\n");
            return status;
        }

        status = fontfile->Read(fontfile, &fontSize, fontBuffer);
        if (EFI_ERROR(status)) {
            SystemTable->ConOut->OutputString(SystemTable->ConOut,
                L"ERROR: Read font failed\n");
            return status;
        }

        EFI_PHYSICAL_ADDRESS safeFontAddr = 0x1000000;
        UINTN fontPages = (fontSize + 0xFFF) / 0x1000;
        status = BootServices->AllocatePages(AllocateAddress, EfiLoaderData,
                                             fontPages, &safeFontAddr);
        if (status == EFI_SUCCESS) {
            BootServices->CopyMem((void *)safeFontAddr, fontBuffer, fontSize);
            fbinfo.font = (void *)safeFontAddr;
            fbinfo.font_size = fontSize;
        }
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Font loaded\n");
    }

    // ========== カーネルをメモリに読み込む ==========
    UINTN totalPages = (kernelSize + 0xFFF) / 0x1000;
    EFI_PHYSICAL_ADDRESS kernelAddr = KERNEL_LOAD_ADDRESS;

    status = BootServices->AllocatePages(AllocateAddress, EfiLoaderData,
                                         totalPages, &kernelAddr);
    if (status != EFI_SUCCESS) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut,
            L"ERROR: AllocatePages for kernel failed\n");
        return status;
    }

    status = kernelFile->Read(kernelFile, &kernelSize, (void *)kernelAddr);
    if (status != EFI_SUCCESS) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut,
            L"ERROR: Read kernel failed\n");
        return status;
    }
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Kernel loaded\n");

    // ========== ELF 解析・展開 ==========
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)kernelAddr;

    if (ehdr->e_ident[0] != 0x7f || ehdr->e_ident[1] != 'E' ||
        ehdr->e_ident[2] != 'L' || ehdr->e_ident[3] != 'F') {
        SystemTable->ConOut->OutputString(SystemTable->ConOut,
            L"ERROR: Invalid ELF magic\n");
        return EFI_LOAD_ERROR;
    }
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"ELF OK\n");

    for (int i = 0; i < ehdr->e_phnum; i++) {
        Elf64_Phdr *phdr = (Elf64_Phdr *)(kernelAddr + ehdr->e_phoff +
                                           (i * ehdr->e_phentsize));
        if (phdr->p_type == 1) { // PT_LOAD
            void *src  = (void *)((uint8_t *)kernelAddr + phdr->p_offset);
            void *dest = (void *)phdr->p_vaddr;
            BootServices->CopyMem(dest, src, phdr->p_filesz);
            if (phdr->p_memsz > phdr->p_filesz) {
                BootServices->SetMem(
                    (void *)((uint8_t *)dest + phdr->p_filesz),
                    phdr->p_memsz - phdr->p_filesz, 0);
            }
        }
    }
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"ELF loaded\n");

    // ========== ExitBootServices & カーネルジャンプ ==========
    int retry = 0;
    while (retry < 5) {
        retry++;
        EFI_MEMORY_DESCRIPTOR *MemoryMap = NULL;
        UINTN MapKey = 0;
        UINTN AllocSize = MemoryMapSize + (DescriptorSize * 20);

        status = BootServices->AllocatePool(EfiLoaderData, AllocSize,
                                            (void **)&MemoryMap);
        if (status != EFI_SUCCESS) continue;

        status = BootServices->GetMemoryMap(&MemoryMapSize, MemoryMap, &MapKey,
                                            &DescriptorSize, &DescriptorVersion);
        if (status != EFI_SUCCESS) continue;

        status = BootServices->ExitBootServices(ImageHandle, MapKey);
        if (status == EFI_SUCCESS) break;
    }

    // ExitBootServices後はConOut使用不可
    typedef void (*kernelEntry)(FramebufferInfo *);
    kernelEntry entry = (kernelEntry)(ehdr->e_entry);
    entry(&fbinfo);

    return EFI_SUCCESS;
}
static EFI_STATUS GetFileInfo(EFI_FILE_PROTOCOL *file, EFI_FILE_INFO **outInfo) {
    EFI_STATUS status;
    UINTN infoSize = 0;

    status = file->GetInfo(file, &gEfiFileInfoGuid, &infoSize, NULL);
    if (status != EFI_BUFFER_TOO_SMALL) {
        return status;
    }

    EFI_FILE_INFO *info;
    status = gBS->AllocatePool(EfiLoaderData, infoSize, (void**)&info);
    if (status != EFI_SUCCESS) {
        return status;
    }

    status = file->GetInfo(file, &gEfiFileInfoGuid, &infoSize, info);
    if (status != EFI_SUCCESS) {
        gBS->FreePool(info);
        return status;
    }

    *outInfo = info;
    return EFI_SUCCESS;
}
