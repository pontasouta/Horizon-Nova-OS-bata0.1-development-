all:
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
  -o BOOTX64.EFI

	objcopy --target efi-app-x86-64 BOOTX64.EFI  BOOTX64.efi
	cp BOOTX64.efi esp/EFI/BOOT/BOOTX64.EFI