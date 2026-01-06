OUTPUT_FORMAT("elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)
ENTRY(EFI_Main)

SECTIONS {
    .text 0x0000000000100000 : {
        *(.text)
    }

    .sdata : {
        *(.sdata)
    }
    .rodate : {
        *(.rodate)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(.bss)
    }

    .eh_frame : {
        *(.eh_frame)
    }
}


