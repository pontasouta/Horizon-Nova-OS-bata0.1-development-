all:
	clang -I. -target x86_64-elf -ffreestanding -fPIC -fno-stack-protector -mno-red-zone  -m64 -nostdlib -fuse-ld=lld -T linker.ld -o horaizon-nova-osboot.elf horaizon-nova-osboot.c
	objcopy --target efi-app-x86-64 horaizon-nova-osboot.elf horaizon-nova-osboot.efi
	cp horaizon-nova-osboot.efi esp/EFI/BOOT/BOOTX64.EFI