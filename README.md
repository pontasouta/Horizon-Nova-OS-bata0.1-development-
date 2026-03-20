開発中

##Horizon-NOVA-OS(alpha0.1)
A custom OS compatible with UEFI(under development)
#overview
The ultimate goal of Horizon-NOVA-OS is to achieve native binary compatibility with multiple platforms. I am aiming to build an environment where applications from the following systems can run out-of-the-box:
Linux Apps (ELF binary compatibility)
BSD Apps
macOS Apps (Mach-O binary support)
This is a research-driven project to explore the boundaries of kernel-level emulation and system call translation.

## 🚀 Current Status
GOP bug fix

## 🛠 Tech Stack
- **Languages:** [C ]
- **Architecture:** [x86_64 ]
- **Build Tools:** [Make]
- **Environment:** [QEMU ]

## 🤝 Contribution
I am looking for contributors! Any help in the following areas is greatly appreciated:
- **Development:** [serial output develop, GOP bug fix]
- **Documentation:** Organizing specifications or creating a Wiki
- **Testing:** Bug reporting and verification on QEMU or real hardware
  help bug fix!
  <img width="1274" height="908" alt="image" src="https://github.com/user-attachments/assets/5db28438-b512-4347-a73e-a98ba0f90c63" />


## 🎮 How to Run (Testing)
You can easily test the OS using QEMU. Run the following command in your terminal:
bash and zsh(OMZ) and fish 
cd boot \ 
 qemu-system-x86_64 \                                                                                                                                                                                                 ─╯
  -m 1G \
  -drive if=pflash,format=raw,readonly=on,file=OVMF_CODE.4m.fd \
  -drive format=raw,file=fat:rw:esp \
  -display sdl -vga std \
  -monitor stdio


## 📬 Contact
If you are interested, please feel free to reach out via:
- **GitHub Issues:** Please open a new issue for any inquiries or participation requests.
- **GitHub Discussions:** Join our community to share ideas!




