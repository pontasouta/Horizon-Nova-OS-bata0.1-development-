 void mainkernel() {
    // カーネルのメイン関数
    while (1) {
        wh_asm_("hlt");
        // 無限ループで停止
    }
}