__attribute__((section(".text.entry"))) 
void mainkernel() {
    // カーネルのメイン関数
    while (1) {
        __asm__("hlt");
        // 無限ループで停止
    }
}