#include <../includemyos/efi.h>
#include <stdint.h>
//EFI SERVICE is created in includemyos/efi.h

// エントリポイント

extern __attribute__((ms_abi)) EFI_STATUS EFI_Main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {

    BootServices = SystemTable->BootServices;

    

    SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16 *)L"Hello");
    while (1){
SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16 *)L"kernel check");

    }
   
    
}