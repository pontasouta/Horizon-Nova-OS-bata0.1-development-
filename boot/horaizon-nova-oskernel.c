
#include "../includemyos/framebuffer.h"
__attribute__((section(".text.entry")))
void mainkernel(FramebufferInfo* fbinfo) {
    // カーネルのメイン関数
    uint32_t* fb_ptr = (uint32_t*)fbinfo->framebuffer;

 ((uint32_t*)fbinfo->framebuffer)[0] = 0x00FF0000; // 画面の最初のピクセルを赤に設定（例）
    while (1) {
        __asm__("hlt");
        // 無限ループで停止
    }
}