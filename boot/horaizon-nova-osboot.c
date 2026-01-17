#include <../includemyos/efi.h>
#include <stdint.h>
#ifdef NULL
#define NULL ((void*)0)
#endif
//EFI SERVICE is created in bootinclude/myos/efi.h

// エントリポイント

extern __attribute__((ms_abi)) EFI_STATUS EFI_Main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {

  

    BootServices = SystemTable->BootServices;
UINTN MemoryMapSize = 0;
void* MemoryMap = 0;
UINTN MapKey;
UINTN DescriptorSize;
UINT32 DescriptorVersion;
   BootServices->GetMemoryMap(&MemoryMapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion); //entry memory map size get
   MemoryMapSize += DescriptorSize * 10;
   BootServices->AllocatePool(EfiLoaderData, MemoryMapSize, (void**)&MemoryMap);
   EFI_STATUS status = BootServices->GetMemoryMap( &MemoryMapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion ); //entry memory map get
   UINTN entryCount = MemoryMapSize / DescriptorSize;
for (UINTN i = 0; i < entryCount; i++) {
    EFI_MEMORY_DESCRIPTOR *desc = (EFI_MEMORY_DESCRIPTOR *)((UINT8 *)MemoryMap + (i * DescriptorSize));
    
    // ここで desc->Type や desc->PhysicalStart などを使って表示
    // たとえば：
    CHAR16 buffer[64];
    Print(buffer, sizeof(buffer), L"Type: %u, Start: %lx\r\n", desc->Type, desc->PhysicalStart);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, buffer);
}



    SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16 *)L"Hello");
    
SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16 *)L"entry point check\n");

BootServices->ExitBootServices(ImageHandle, MapKey);
BootServices->ExitBootServices(ImageHandle, MapKey);

    return 0;
}
