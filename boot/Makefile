all:
	clang -target x86_64-elf -ffreestanding -fPIC -fno-stack-protector -mno-red-zone  -m64 -nostdlib -fuse-ld=lld -T linker.ld -o myoskernel.elf myoskernel.c
	objcopy --target efi-app-x86-64 myoskernel.elf myoskernel.efi
	cp myoskernel.efi esp/EFI/BOOT/BOOTX64.EFI

