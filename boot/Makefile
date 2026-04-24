all: BOOTX64.EFI myoskernel.elf

BOOTX64.EFI:
	clang \
	-target x86_64-unknown-windows \
	-mabi=ms \
	-fno-common \
	-ffreestanding -fno-stack-protector -mno-red-zone \
	-fshort-wchar -fno-pic -fno-pie \
	-fuse-ld=lld \
	-Wl,-entry:efi_main \
	-Wl,-subsystem:efi_application \
	-nostdlib \
	-I ../includemyos \
	horaizon-nova-osboot.c efi_guid.c \
	-o BOOTX64.EFI
	cp BOOTX64.EFI esp/EFI/BOOT/BOOTX64.EFI

myoskernel.elf:
	nasm -f elf64 start.S -o start.o
	clang \
	-target x86_64-unknown-elf \
	-ffreestanding -fno-stack-protector -mno-red-zone \
	-nostdlib \
	-Wl,-T,linker.ld \
	-Wl,-entry=mainkernel \
	-I ../includemyos \
	start.o horaizon-nova-oskernel.c \
	-o myoskernel.elf 
	cp myoskernel.elf esp/
