#include "../includemyos/efi.h"
#include "../includemyos/framebuffer.h"
#include "../includemyos/guid.h"
#include "../includemyos/elf.h"
#include "../includemyos/printhexstatus.h"

#include <stdint.h>

#ifndef NULL
#define NULL ((void *)0)
#endif

#define KERNEL_LOAD_ADDRESS 0x200000
typedef int64_t INTN;

EFI_BOOT_SERVICES *gBS; 

EFI_BOOT_SERVICES *BootServices;
EFI_SYSTEM_TABLE *SystemTable;
/*

EFI_STATUS GetFileInfo(EFI_FILE_PROTOCOL *file, EFI_FILE_INFO **outInfo)
{
    EFI_STATUS status;
    UINTN infoSize = 0;  // 呼ぶたびに必ず 0 で初期化される

    // 1. サイズを確認（わざと失敗させて正確なサイズを知る）
    status = file->GetInfo(file, &gEfiFileInfoGuid, &infoSize, NULL);
    if (status != EFI_BUFFER_TOO_SMALL) {
        return status; 
    }

    // 2. 正しいサイズ分だけメモリを確保
    status = gBS->AllocatePool(EfiLoaderData, infoSize, (void**)outInfo);
    if (EFI_ERROR(status)) {
        return status;
    }

    // 3. 確保したメモリに本番のデータを書き込む
    status = file->GetInfo(file, &gEfiFileInfoGuid, &infoSize, (void*)*outInfo);
    
    return status;
}
  
*/
EFI_STATUS GetFileInfo(EFI_FILE_PROTOCOL *file, EFI_FILE_INFO **outInfo)
{
    EFI_STATUS status;
    UINTN infoSize = 0;

    // 1. サイズを確認
    status = file->GetInfo(file, &gEfiFileInfoGuid, &infoSize, NULL);
    if (status != EFI_BUFFER_TOO_SMALL) {
        // ★もしここでエラーになるなら、そもそもファイルの情報が取れていません
        return status; 
    }

    // 2. メモリを確保
    status = gBS->AllocatePool(EfiLoaderData, infoSize, (void**)outInfo);
    if (EFI_ERROR(status)) {
        // ★もしここでエラーになるなら、メモリが足りなくて確保に失敗しています
        return status;
    }

    // 3. 本番のデータを書き込む
    status = file->GetInfo(file, &gEfiFileInfoGuid, &infoSize, (void*)*outInfo);
    if (EFI_ERROR(status)) {
        // ★もしここでエラーになるなら、最後の書き込みで失敗しています
        return status;
    }
    
    return status;
}


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
    status = root->Open(root, &kernelFile, L"myoskernel.elf",EFI_FILE_MODE_READ, 0);
    if (status != EFI_SUCCESS) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut,L"ERROR: Kernel file not found\n");
        return status;
    }
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Kernel file opened\n");

    // ========== カーネルファイル情報を取得 ==========
    
    //EFI_FILE_INFO *fileInfo = NULL;
    //status = GetFileInfo(kernelFile, &fileInfo);
    //SystemTable->ConOut->OutputString(SystemTable->ConOut, L"testpoint1\n");

    //if (status != EFI_SUCCESS) {
      //  SystemTable->ConOut->OutputString(SystemTable->ConOut, L"testpoint1\n");
        //SystemTable->ConOut->OutputString(SystemTable->ConOut, L"ERROR: GetInfo for kernel failed\n");
      //  return status;
    //}
    //UINTN kernelSize = fileInfo->FileSize;
    //BootServices->FreePool(fileInfo);
EFI_FILE_INFO *fileInfo = NULL;
status = GetFileInfo(kernelFile, &fileInfo);
/*
if (status != EFI_SUCCESS) {
    SystemTable->ConOut->OutputString(SystemTable->ConOut,L"ERROR: GetInfo for kernel failed\n");
    return status;
}
*/

UINTN kernelSize = fileInfo->FileSize;

    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Kernel size obtained\n");

    // ========== フォントファイルを開く ==========
    EFI_FILE_PROTOCOL *fontfile = NULL;
    status = root->Open(root, &fontfile,
                        L"\\EFI\\BOOT\\solarize-12x29-psf\\Solarize.12x29.psf",
                        EFI_FILE_MODE_READ, 0);
    if (status != EFI_SUCCESS) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut,
            L"WARNING: Font file not found, continuing without font\n");
    } else {
        EFI_FILE_INFO *fontFileInfo = NULL;
        status = GetFileInfo(fontfile, &fontFileInfo);


        fontSize = fontFileInfo->FileSize;
        BootServices->FreePool(fontFileInfo);
        UINTN readSize = fontSize + 10;


        status = BootServices->AllocatePool(EfiLoaderData, readSize, &fontBuffer);
        if (EFI_ERROR(status)) {
            SystemTable->ConOut->OutputString(SystemTable->ConOut,
                L"ERROR: AllocatePool for font failed\n");
            return status;
        }

        status = fontfile->Read(fontfile, &readSize, fontBuffer);
        if (EFI_ERROR(status)) {
            SystemTable->ConOut->OutputString(SystemTable->ConOut,
                L"ERROR: Read font failed\n");
            return status;
        }

        EFI_PHYSICAL_ADDRESS safeFontAddr = 0x1000000;
        UINTN fontPages = (fontSize + 0xFFF) / 0x1000;
        status = BootServices->AllocatePages(AllocateAnyPages, EfiLoaderData,
                                             fontPages, &safeFontAddr);
        if (status == EFI_SUCCESS) {
            BootServices->CopyMem((void *)safeFontAddr, fontBuffer, fontSize);
            fbinfo.font = (void *)safeFontAddr;
            fbinfo.font_size = fontSize;
        }
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Font loaded\n");
    }
/*

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
*/
    // ========== カーネルを一時的なメモリに読み込む ==========
    /*
    
    UINTN totalPages = (kernelSize + 0xFFF) / 0x1000;
    
    // 1. AllocateAnyPagesを使うときは、初期値を0にしてUEFIに場所を探してもらう
    EFI_PHYSICAL_ADDRESS kernelAddr = 0; 
    
    status = BootServices->AllocatePages(AllocateAnyPages, EfiLoaderData,
                                         totalPages, &kernelAddr);
    if (status != EFI_SUCCESS) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut,
            L"ERROR: AllocatePages for kernel failed\n");
        return status;
    }
    
    // 2. 自動で決まった「kernelAddr」の場所にファイルを読み込む
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
    
            // 3. 【重要】展開先（dest）のメモリを個別に確保する
            UINTN destPages = (phdr->p_memsz + 0xFFF) / 0x1000;
            EFI_PHYSICAL_ADDRESS destAddr = phdr->p_vaddr;
            
            status = BootServices->AllocatePages(AllocateAddress, EfiLoaderData,
                                                 destPages, &destAddr);
            if (status != EFI_SUCCESS) {
                SystemTable->ConOut->OutputString(SystemTable->ConOut,
                    L"ERROR: AllocatePages for PT_LOAD segment failed\n");
                return status;
            }
    
            // 4. 安全に確保されたメモリにコピーする
            BootServices->CopyMem(dest, src, phdr->p_filesz);
            if (phdr->p_memsz > phdr->p_filesz) {
                BootServices->SetMem(
                    (void *)((uint8_t *)dest + phdr->p_filesz),
                    phdr->p_memsz - phdr->p_filesz, 0);
            }
        }
    }
    
    // 5. 使い終わった一時的なファイル読み込み用メモリは解放する
    BootServices->FreePages(kernelAddr, totalPages);
    
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"ELF loaded\n");
    */

 // ========== カーネルを一時的なメモリに読み込む ==========
// 1. 【変更】AllocatePoolを使うため、totalPagesの計算は不要になります。
// kernelAddr を (void *) 型で受け取れるようにポインタを用意します。
// ========== カーネルを一時的なメモリに読み込む ==========
void *kernelBuffer = NULL;

// AllocatePoolでカーネルのサイズ分だけ確保
status = BootServices->AllocatePool(EfiLoaderData, kernelSize, &kernelBuffer);
if (status != EFI_SUCCESS) {
    SystemTable->ConOut->OutputString(SystemTable->ConOut,
        L"ERROR: AllocatePool for kernel failed\n");
    return status;
}

// 確保した場所にファイルを読み込む
kernelFile->SetPosition(kernelFile, 0);

// 【デバッグ追加】現状のサイズが本当に正しいか、一度「巨大なサイズ（例: 1MB = 1024*1024）」で
// 強制的に上書きして、ファイル全体を確実に読み込ませてみる
// (実際のファイルサイズより大きくても、ファイル終端まで読んだ時点でReadが実サイズに書き換えてくれます)
kernelSize = 1024 * 1024; 

status = kernelFile->Read(kernelFile, &kernelSize, kernelBuffer);
if (status != EFI_SUCCESS) {
    SystemTable->ConOut->OutputString(SystemTable->ConOut,
        L"ERROR: Read kernel failed\n");
    BootServices->FreePool(kernelBuffer);
    return status;
}
SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Kernel loaded\n");

// 計算しやすいように、一度バイト単位のポインタ（uint8_t*）に変換します
uint8_t *kernelBytePtr = (uint8_t *)kernelBuffer;

// ========== ELF 解析・展開 ==========
// 【修正】確保したバッファの先頭をELFヘッダとして読み込む
Elf64_Ehdr *ehdr = (Elf64_Ehdr *)kernelBytePtr;

if (ehdr->e_ident[0] != 0x7f || 
    ehdr->e_ident[1] != 0x45 || // 'E' を 0x45 に
    ehdr->e_ident[2] != 0x4c || // 'L' を 0x4c に
    ehdr->e_ident[3] != 0x46) { // 'F' を 0x46 に
    
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"ERROR: Invalid ELF magic\n");
    BootServices->FreePool(kernelBuffer);
    return EFI_LOAD_ERROR;
}
SystemTable->ConOut->OutputString(SystemTable->ConOut, L"ELF OK\n");

for (int i = 0; i < ehdr->e_phnum; i++) {
    // 【修正】バイト単位のポインタを使って、ズレがないように計算します
    Elf64_Phdr *phdr = (Elf64_Phdr *)(kernelBytePtr + ehdr->e_phoff +
                                       (i * ehdr->e_phentsize));
    
    if (phdr->p_type == 1) { // PT_LOAD
        // 【修正】データ元（src）もバイト単位で正しく計算します
        void *src  = (void *)(kernelBytePtr + phdr->p_offset);
        void *dest = (void *)phdr->p_vaddr;

        // 本来の宛先（0x1000000 など）へコピー
        BootServices->CopyMem(dest, src, phdr->p_filesz);
        if (phdr->p_memsz > phdr->p_filesz) {
            BootServices->SetMem(
                (void *)((uint8_t *)dest + phdr->p_filesz),
                phdr->p_memsz - phdr->p_filesz, 0);
        }
    }
}

// 使い終わった一時メモリを解放
BootServices->FreePool(kernelBuffer);

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

// どんなファイル（file）でも、その情報を outInfo に入れて返してくれる関数
