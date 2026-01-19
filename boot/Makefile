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
  -I 	../includemyos/ \
  -I ../includemyos/framebuffer.h \
  -o BOOTX64.EFI

	objcopy --target efi-app-x86-64 BOOTX64.EFI  BOOTX64.efi
	cp BOOTX64.efi esp/EFI/BOOT/BOOTX64.EFI

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