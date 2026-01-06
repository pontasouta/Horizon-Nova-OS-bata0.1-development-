OUTPUT_FORMAT("efi-app-x86_64")
OUTPUT_ARCH(i386:x86-64)
ENTRY(EfiMain)

SECTIONS {
    .text 0x0000000000000000 : {
        *(.text)
    }

    .sdata : {
        *(.sdata)
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
