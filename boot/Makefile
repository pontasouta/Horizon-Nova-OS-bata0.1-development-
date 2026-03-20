all: BOOTX64.EFI myoskernel.bin

BOOTX64.EFI:
	clang \
  -target x86_64-unknown-windows \
  -ffreestanding -fno-stack-protector -mno-red-zone \
  -fshort-wchar -fno-pic -fno-pie \
  -fuse-ld=lld \
  -Wl,-entry:efi_main \
  -Wl,-subsystem:efi_application \
  -nostdlib \
  horaizon-nova-osboot.c efi_guid.c \
  -I ../includemyos/efi.h \
  -I ../includemyos/framebuffer.h \
  -I ../includemyos/print.h \
  -o BOOTX64.EFI

	cp BOOTX64.EFI esp/EFI/BOOT/BOOTX64.EFI

myoskernel.elf:
	clang \
  -target x86_64-unknown-elf \
  -ffreestanding -fno-stack-protector -mno-red-zone \
  -nostdlib \
  -Wl,-T,linker.ld \
  -Wl,-entry=mainkernel \
  horaizon-nova-oskernel.c \
  -o myoskernel.elf

myoskernel.bin: myoskernel.elf
	objcopy -O binary myoskernel.elf myoskernel.bin