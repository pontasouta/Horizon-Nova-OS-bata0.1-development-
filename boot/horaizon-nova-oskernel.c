
#include "../includemyos/framebuffer.h"
// IDTエントリの構造体定義
struct IDTEntry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  ist;
    uint8_t  type_attr;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t zero;
} __attribute__((packed));
struct IDTR {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

struct IDTEntry idt[256];

void set_idt_entry(int vector, void (*handler)()) {
    uint64_t addr = (uint64_t)handler;
    idt[vector].offset_low  = addr & 0xFFFF;
    idt[vector].selector    = 0x08; // カーネルコードセグメント
    idt[vector].ist         = 0;
    idt[vector].type_attr   = 0x8E; // present, interrupt gate
    idt[vector].offset_mid  = (addr >> 16) & 0xFFFF;
    idt[vector].offset_high = (addr >> 32) & 0xFFFFFFFF;
    idt[vector].zero        = 0;
}
void load_idt() {
    struct IDTR idtr;
    idtr.limit = sizeof(idt) - 1;
    idtr.base = (uint64_t)&idt;
    __asm__ volatile("lidt %0" : : "m"(idtr));
}
__attribute__((interrupt))
void dummy_handler(void* frame) {
    // 何もしない
}



__attribute__((section(".text.entry")))
void mainkernel(FramebufferInfo* fbinfo) {
    __asm__ __volatile__("cli");
    // カーネルのメイン関数
    for (int i = 0; i < 256; i++) {
        set_idt_entry(i, dummy_handler);
    }
    load_idt();
    
    uint32_t* fb_ptr = (uint32_t*)fbinfo->framebuffer;

 ((uint32_t*)fbinfo->framebuffer)[0] = 0x00FF0000; // 画面の最初のピクセルを赤に設定（例）
    while (1) {
        __asm__("hlt");
        // 無限ループで停止
    }
}