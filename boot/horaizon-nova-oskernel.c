
#include "../includemyos/framebuffer.h"
#include "../includemyos/kernel/idt.h"
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
void dummy_handler(struct InterruptFrame* frame) {
    // 何もしない
}
/*
typedef struct {
    uint8_t magic[2];   // 0x36, 0x04 が PSF1 のマジックナンバー
    uint8_t mode;       // ビット0: 512文字なら1、256文字なら0
    uint8_t charsize;   // 1文字あたりのバイト数（例: 16）
} __attribute__((packed)) PSF1Header;
void draw_char(uint32_t* fb, int x, int y, char c, uint32_t color,
               FramebufferInfo* fbinfo, PSF1Header* font, uint8_t* glyphs){
    uint8_t* glyph;
    if (font->mode == 1) {
        // 512文字対応
        glyph = glyphs + c * font->charsize;
    } else {
        // 256文字のみ
        glyph = glyphs + (c % 256) * font->charsize;
    }

    for (int row = 0; row < font->charsize; row++) {
        uint8_t bits = glyph[row];
        for (int col = 0; col < 8; col++) {
            if ((bits >> (7 - col)) & 1) {
                int px = x + col;
                int py = y + row;
                fb[py * fbinfo->Pixels_Per_ScanLine + px] = color;
            }
        }
    }
}
*/




__attribute__((section(".text.entry")))
void mainkernel(FramebufferInfo* fbinfo) {
    __asm__ __volatile__("cli");
    // カーネルのメイン関数
    
    for (int i = 0; i < 256; i++) {
        set_idt_entry(i, dummy_handler);
    }
    //load_idt();
     //uint8_t* font_base = (uint8_t*)fbinfo->font;
    //uint32_t* fb_ptr = (uint32_t*)fbinfo->framebuffer;
    //PSF1Header* font = (PSF1Header*)fbinfo->font;
    __asm__ __volatile__("sti");
   
    //uint8_t* glyphs = (uint8_t*)fbinfo->font + sizeof(PSF1Header);
//if (font->magic[0] != 0x36 || font->magic[1] != 0x04) { 
    //　フォントが壊れてる　または未対応のフォーマット
  //  return; }
    //draw_char(fb_ptr, 0, 0, '1', 0x00FF00, fbinfo, font, glyphs);
    //draw_char((uint32_t*)fbinfo->framebuffer, 8, 8, 'A', 0x00FFFFFF, fbinfo, font, glyphs);
    

 //((uint32_t*)fbinfo->framebuffer)[0] = 0x00FF0000; // 画面の最初のピクセルを赤に設定（例）
   // while (1) {
    //    __asm__("hlt");
    //   // 無限ループで停止
    //}
}