all: BOOTX64.EFI myoskernel.elf

BOOTX64.EFI:
	clang \
  -target x86_64-unknown-windows \
  -ffreestanding -fno-stack-protector -mno-red-zone \
  -O0 \
  -fshort-wchar -fno-pic -fno-pie -fno-builtin \
  -fuse-ld=lld \
  -Wl,-entry:efi_main \
  -Wl,-subsystem:efi_application \
  -nostdlib \
  horaizon-nova-osboot.c efi_guid.c \
  -I ../includemyos \
  -o BOOTX64.EFI

	cp BOOTX64.EFI esp/EFI/BOOT/BOOTX64.EFI

myoskernel.elf:
	clang \
	-target x86_64-unknown-elf \
	-ffreestanding -fno-stack-protector -mno-red-zone \
	-nostdlib \
	-static \
	-Wl,-T,linker.ld \
	-Wl,-entry=mainkernel \
	horaizon-nova-oskernel.c \
	-o myoskernel.elf
	
	
	llvm-objcopy --strip-all myoskernel.elf

	cp myoskernel.elf esp/myoskernel.elf