#include <../includemyos/efi.h>
#include <stdint.h>


#ifndef NULL
#define NULL ((void*)0)
#endif
#define KERNEL_LOAD_ADDRESS 0x100000  // カーネルのロードアドレス
//EFI SERVICE is created in bootinclude/myos/efi.h

// エントリポイント
EFI_BOOT_SERVICES* BootServices;
EFI_SYSTEM_TABLE *SystemTable;

extern __attribute__((ms_abi)) EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *ST) {
SystemTable = ST;
BootServices = ST->BootServices;
UINTN DebugMemoryMapSize = 1;
UINTN MemoryMapSize = 1;
EFI_MEMORY_DESCRIPTOR *DebugMemoryMap = NULL;
UINTN MapKey;
UINTN DescriptorSize ;
UINT32 DescriptorVersion;
EFI_STATUS status;
   //EFI_STATUS status = BootServices->GetMemoryMap(&MemoryMapSize, DebugMemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion); //entry memory map size get
   //if ( status != EFI_BUFFER_TOO_SMALL )
   //{
      //   SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16 *)L"1 getmemorymap\n");
    //     return status;
  // }

//SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16 *)L"DescriptorSize = %d\n, DescriptorSize");
//SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16 *)L"MemoryMapSize = %d\n, MemoryMapSize");

  //MemoryMapSize += DescriptorSize * 20;

   //BootServices->AllocatePool(EfiLoaderData, MemoryMapSize, (void**)&DebugMemoryMap);
   //status = BootServices->GetMemoryMap( &MemoryMapSize, DebugMemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion ); //entry memory map get
   //if ( status != EFI_SUCCESS )
   //{
       //  SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16 *)L"oh no crash\n");
     //    return status;
   //};
    
   //UINTN entryCount = MemoryMapSize / DescriptorSize;
//for (UINTN i = 0; i < entryCount; i++) {
    //EFI_MEMORY_DESCRIPTOR *desc = (EFI_MEMORY_DESCRIPTOR *)((UINT8 *)DebugMemoryMap + (i * DescriptorSize));

    // ここで desc->Type や desc->PhysicalStart などを使って表示
    // たとえば：
   // CHAR16 buffer[64];
    
    //SystemTable->ConOut->OutputString(SystemTable->ConOut, buffer);
//}
SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16 *)L"start open root dir\n");
EFI_LOADED_IMAGE_PROTOCOL* loadedImage;
status = BootServices->HandleProtocol(
    ImageHandle,
    &gEfiLoadedImageProtocolGuid,
    (void**)&loadedImage
);
if (status != EFI_SUCCESS) {
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"HandleProtocol(LoadedImage) failed\n");
    return status;
}
EFI_HANDLE deviceHandle = loadedImage->DeviceHandle;



EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* fs;
status =BootServices->HandleProtocol(
deviceHandle,
&gEfiSimpleFileSystemProtocolGuid,
(void**)&fs
);
if (status != EFI_SUCCESS) {
    SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16 *)L"HandleProtocol failed\n");
    return status;
}
//open root directory
EFI_FILE_PROTOCOL* root;
status = fs->OpenVolume(fs, &root);
if (status != EFI_SUCCESS) {
    SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16 *)L"OpenVolume failed\n");
    return status;

}
//kernelfile s open
EFI_FILE_PROTOCOL* kernelFile;
status = root->Open(
    root,
    &kernelFile,
    L"horaizon-nova-oskernel.elf",
    EFI_FILE_MODE_READ,
    0
);
if (status != EFI_SUCCESS){
    SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16 *)L"Open failed\n");
    return status;
}
//file s size get
UINTN infosize = 0;
EFI_FILE_INFO* fileInfo;
status = kernelFile->GetInfo(
    kernelFile,
    &gEfiFileInfoGuid,
    &infosize,
    (void*)0
);
if (status != EFI_BUFFER_TOO_SMALL){
    SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16 *)L"GetInfo size failed\n");
    return status;
}        
// buffer get
status = BootServices->AllocatePool(EfiLoaderData, infosize, (void**)&fileInfo);
if (status != EFI_SUCCESS){
    SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16 *)L"AllocatePool for fileinfo failed\n");
    return status;
}
status = kernelFile->GetInfo(
    kernelFile,
    &gEfiFileInfoGuid,
    &infosize,
    fileInfo
);
if (status != EFI_SUCCESS){
    SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16 *)L"GetInfo failed\n");
    return status;
}

UINTN kernelSize = fileInfo->FileSize;
    status = kernelFile->Read(
    kernelFile,
    &kernelSize,
    (void*)KERNEL_LOAD_ADDRESS
    );
    if (status != EFI_SUCCESS){
        SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16 *)L"Read failed\n");
        return status;
    };
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Kernel loaded\n");



    SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16 *)L"Hello");
    
SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16 *)L"entry point check\n");
// ここまでに：カーネル読み込み完了、必要な BootServices 呼び出しも全部終わってる

while (1) {
    UINTN MemoryMapSize = 0;
    EFI_MEMORY_DESCRIPTOR* MemoryMap = NULL;
    UINTN MapKey;
    UINTN DescriptorSize;
    UINT32 DescriptorVersion;

    // サイズ取得
    BootServices->GetMemoryMap(&MemoryMapSize, NULL, &MapKey, &DescriptorSize, &DescriptorVersion);
    MemoryMapSize += DescriptorSize * 20;
    BootServices->AllocatePool(EfiLoaderData, MemoryMapSize, (void**)&MemoryMap);

    EFI_STATUS status = BootServices->GetMemoryMap(
        &MemoryMapSize,
        MemoryMap,
        &MapKey,
        &DescriptorSize,
        &DescriptorVersion
    );
    if (status != EFI_SUCCESS) {
        // ここでまた BufferTooSmall ならループ継続
        continue;
    }

    status = BootServices->ExitBootServices(ImageHandle, MapKey);
    if (status == EFI_SUCCESS) {
        break; // 抜けたらもう BootServices は使えない
   
    }

    // 失敗したら、また最初から GetMemoryMap やり直し
}
     typedef void (*kernelEntry)(void);
kernelEntry entry = (kernelEntry)(KERNEL_LOAD_ADDRESS);
entry();

return EFI_SUCCESS;

}
