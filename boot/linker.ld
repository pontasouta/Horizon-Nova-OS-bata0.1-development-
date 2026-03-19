OUTPUT_FORMAT("elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)
ENTRY(mainkernel)

SECTIONS {
    . = 0x100000; 
    .text : {
        *(.text*)
    }

    .sdata : {
        *(.sdata*)
    }
    .rodata : {
        *(.rodata*)
    }

    .data : {
        *(.data*)
    }

    .bss : {
        *(.bss*)
        :(COMMON)
    }

    
}


