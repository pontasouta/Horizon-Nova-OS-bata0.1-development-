#ifndef PRINT_H
#define PRINT_H
#include "efi.h"
#include "efi.h"

void PrintDecimal(UINTN value, EFI_SYSTEM_TABLE* SystemTable) {
    CHAR16 buffer[32];
    int pos = 30;
    buffer[31] = L'\0';

    if (value == 0) {
        buffer[pos--] = L'0';
    } else {
        while (value > 0 && pos >= 0) {
            buffer[pos--] = (CHAR16)(L'0' + (value % 10));
            value /= 10;
        }
    }

    SystemTable->ConOut->OutputString(SystemTable->ConOut, &buffer[pos + 1]);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"\r\n");
}
#endif // PRINT_H