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
   BootServices->Get_Memorymap(&MemoryMapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion); 

    SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16 *)L"Hello");
    
SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16 *)L"entry pont check\n");

    
    return 0;
}
