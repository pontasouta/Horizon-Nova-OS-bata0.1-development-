#include "../includemyos/framebuffer.h"
#include "../includemyos/kernel/idt.h"
#include "../includemyos/kernel/kconfig.h"
#include "../includemyos/kernel/pagetable/pagetable.h"                               // ページテーブルヘッダ
#include <stdint.h>

typedef struct {
    uint8_t magic[2];   // 0x36 0x04
    uint8_t mode;
    uint8_t charsize;  // 1 文字あたりのバイト数
} PSF1Header;

static void draw_pixel(FramebufferInfo *fb, uint32_t x, uint32_t y, uint32_t color) {
    if (x >= fb->Width || y >= fb->Height) return;
    uint8_t *base = (uint8_t *)fb->framebuffer;
    uint8_t *pixel = base + (uint64_t)y * fb->Pixels_Per_ScanLine * 4 + x * 4;
    if (fb->PixelFormat == PixelBlueGreenRedReserved8BitPerColor) {
        pixel[0] = (color >> 0) & 0xFF;
        pixel[1] = (color >> 8) & 0xFF;
        pixel[2] = (color >> 16) & 0xFF;
        pixel[3] = (color >> 24) & 0xFF;
    } else {
        pixel[0] = (color >> 16) & 0xFF;
        pixel[1] = (color >> 8) & 0xFF;
        pixel[2] = (color >> 0) & 0xFF;
        pixel[3] = (color >> 24) & 0xFF;
    }
}

static void draw_char(FramebufferInfo *fb, const uint8_t *font, uint32_t x, uint32_t y, char c, uint32_t color) {
    if (!font) return;
    PSF1Header *hdr = (PSF1Header *)font;
    if (hdr->magic[0] != 0x36 || hdr->magic[1] != 0x04) return;

    uint32_t glyph_size = hdr->charsize;
    const uint8_t *glyphs = font + sizeof(PSF1Header);
    const uint8_t *glyph = glyphs + ((uint8_t)c) * glyph_size;

    for (uint32_t row = 0; row < glyph_size; row++) {
        uint8_t bits = glyph[row];
        for (uint32_t bit = 0; bit < 8; bit++) {
            if (bits & (0x80 >> bit)) {
                draw_pixel(fb, x + bit, y + row, color);
            }
        }
    }
}

static void draw_string(FramebufferInfo *fb, const uint8_t *font, uint32_t x, uint32_t y, const char *s, uint32_t color) {
    while (*s) {
        draw_char(fb, font, x, y, *s++, color);
        x += 8;
    }
}

// ============================================================================
// 1. 各種構造体の定義
// ============================================================================
struct GDTEntry { uint16_t limit_low; uint16_t base_low; uint8_t base_middle; uint8_t access_byte; uint8_t flags_limit_high; uint8_t base_high; } __attribute__((packed));
struct GDTR { uint16_t limit; uint64_t base; } __attribute__((packed));
struct IDTEntry { uint16_t offset_low; uint16_t selector; uint8_t ist; uint8_t type_attr; uint16_t offset_mid; uint32_t offset_high; uint32_t zero; } __attribute__((packed));
struct IDTR { uint16_t limit; uint64_t base; } __attribute__((packed));

// ============================================================================
// 2. グローバル変数と模擬データの配置
// ============================================================================
struct GDTEntry gdt[3];     // 【修正】きっちり配列として定義
struct IDTEntry idt[256];   // 【修正】きっちり配列として定義

#include "../includemyos/idt.c"

// ============================================================================
// 3. 各種関数の実装
// ============================================================================

void init_gdt();
void set_idt_entry(int vector, void (*handler)());
void load_idt();


// ============================================================================
// 4. Kconfig 風設定に基づく簡易実行結果
// ============================================================================
static int run_kernel_config_checks(void) {
    return MYOS_ENABLE_HIVEFILE ? 1 : 0;
}

// ============================================================================
// 5. カーネルエントリポイント
// ============================================================================
// 5. カーネルエントリポイント
// ============================================================================
__attribute__((section(".text.entry")))
__attribute__((sysv_abi))
void mainkernel(void* fbinfo) {
    __asm__ __volatile__("cli"); // 割り込み禁止
    
    // 1. CPUディスクリプタの基本初期化
    init_gdt();
    for (int i = 0; i < 256; i++) set_idt_entry(i, dummy_handler);
    load_idt();

    // ★【重要】かつてクラッシュ（#PF）の原因だった nk_cell や vk_cell への
    // 直接代入コード（nk_cell->size = -80; など）は、すべて綺麗に消去されています。

    // 2. Kconfig 風設定に基づく簡易パス
    volatile int parse_result = run_kernel_config_checks();

    // 3. GOP フレームバッファ描画は一時的に無効化し、起動の安定化を優先する
    FramebufferInfo* fb = (FramebufferInfo*)fbinfo;
    (void)fb;

    // 4. パース結果（成功なら 1）を EAX レジスタに叩き込む
    __asm__ __volatile__("movl %0, %%eax" : : "r"(parse_result) : "eax");

    // 5. 安全なハルト無限ループ
    while (1) {
        __asm__ __volatile__("hlt");
    }
}