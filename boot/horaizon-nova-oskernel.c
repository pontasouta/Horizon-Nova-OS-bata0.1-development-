#include "../includemyos/framebuffer.h"
#include "../includemyos/kernel/idt.h"
#include "../includemyos/kernel/hivefile/hivefile.h" // ハイブ構造体定義
#include <stdint.h>

// ============================================================================
// 1. 各種構造体の定義
// ============================================================================

// GDTエントリの構造体
struct GDTEntry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access_byte;
    uint8_t  flags_limit_high;
    uint8_t  base_high;
} __attribute__((packed));

struct GDTR {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

// IDTエントリの構造体
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

// PSF1フォントヘッダ
typedef struct {
    uint8_t magic[2];   // 0x36, 0x04
    uint8_t mode;
    uint8_t charsize;
} __attribute__((packed)) PSF1Header;

// ============================================================================
// 2. グローバル変数と模擬データの配置
// ============================================================================
struct GDTEntry gdt[3]; 
struct IDTEntry idt[256];

// 大きめの模擬ハイブメモリを確保（4096Bヘッダ + 1024Bデータ領域）
static uint8_t mock_hive_data[5120] = {
    0x72, 0x65, 0x67, 0x66, // base->signature = "regf"
    0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
    0x01,0x00,0x00,0x00, 0x03,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x01,0x00,0x00,0x00,
    0x00, 0x00, 0x00, 0x00, // base->root_cell_offset = 0x00000000
};

// ============================================================================
// 3. 各種関数の実装（GDT / IDT / 描画）
// ============================================================================

void init_gdt() {
    struct GDTR gdtr;
    
    // Null Descriptor
    gdt[0].limit_low = 0; gdt[0].base_low = 0; gdt[0].base_middle = 0; gdt[0].access_byte = 0; gdt[0].flags_limit_high = 0; gdt[0].base_high = 0;
    // Kernel Code Descriptor
    gdt[1].limit_low = 0xFFFF; gdt[1].base_low = 0; gdt[1].base_middle = 0; gdt[1].access_byte = 0x9A; gdt[1].flags_limit_high = 0xA0; gdt[1].base_high = 0;
    // Kernel Data Descriptor
    gdt[2].limit_low = 0xFFFF; gdt[2].base_low = 0; gdt[2].base_middle = 0; gdt[2].access_byte = 0x92; gdt[2].flags_limit_high = 0xC0; gdt[2].base_high = 0;

    gdtr.limit = sizeof(gdt) - 1;
    gdtr.base = (uint64_t)&gdt;

    __asm__ volatile(
        "lgdt %0\n\t"
        "mov $0x10, %%ax\n\t"
        "mov %%ax, %%ds\n\t"
        "mov %%ax, %%es\n\t"
        "mov %%ax, %%ss\n\t"
        "mov %%ax, %%fs\n\t"
        "mov %%ax, %%gs\n\t"
        "pushq $0x08\n\t"
        "leaq 1f(%%rip), %%rax\n\t"
        "pushq %%rax\n\t"
        "lretq\n\t"
        "1:\n\t"
        : : "m"(gdtr) : "rax", "ax"
    );
}

void set_idt_entry(int vector, void (*handler)()) {
    uint64_t addr = (uint64_t)handler;
    idt[vector].offset_low  = addr & 0xFFFF;
    idt[vector].selector    = 0x08;
    idt[vector].ist         = 0;
    idt[vector].type_attr   = 0x8E;
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

void draw_char(uint32_t* fb, int x, int y, char c, uint32_t color,
               FramebufferInfo* fbinfo, PSF1Header* font, uint8_t* glyphs){
    uint8_t* glyph;
    if (font->mode == 1) {
        glyph = glyphs + c * font->charsize;
    } else {
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

// ============================================================================
// 4. ハイブファイル（レジストリ）解析ロジック（mainkernelの前に配置！）
// ============================================================================
int parse_hive_value_blind(void* hive_base_ptr) {
    HiveBaseBlock* base = (HiveBaseBlock*)hive_base_ptr;

    // 1. "regf" チェック
    if (base->signature[0] != 0x72 || base->signature[1] != 0x65) return 0xDEAD0001;

    // 2. ルート nk セルの特定
    uint32_t root_offset = base->root_cell_offset;
    HiveCell* root_cell = (HiveCell*)((uintptr_t)hive_base_ptr + 4096 + root_offset);
    HiveKeyNode* root_node = (HiveKeyNode*)(root_cell->data);

    // 3. "nk" チェック
    if (root_node->signature[0] != 0x6E || root_node->signature[1] != 0x6B) return 0xDEAD0002;

    // 4. このキーが持つ「値（vk）」の数を確認
    if (root_node->num_values == 0) return 0xDEAD0003;

    // 5. 値リストのセルへ移動
    uint32_t v_list_offset = root_node->values_list_offset;
    HiveCell* v_list_cell = (HiveCell*)((uintptr_t)hive_base_ptr + 4096 + v_list_offset);
    
    // リストセルのデータ領域から最初のvkセルへのオフセットを読み出す
    uint32_t* vk_offset_array = (uint32_t*)(v_list_cell->data);
    uint32_t first_vk_offset = vk_offset_array[0];

    // 6. 本物の 「vk セル」 へ突入！
    HiveCell* vk_cell = (HiveCell*)((uintptr_t)hive_base_ptr + 4096 + first_vk_offset);
    HiveValueNode* vk_node = (HiveValueNode*)(vk_cell->data);

    // 7. "vk" マジックチェック
    if (vk_node->signature[0] != 0x76 || vk_node->signature[1] != 0x6B) {
        return 0xDEAD0004;
    }

    // 8. 【完全成功】データ型を上位、名前の長さを下位ビットに入れて返す
    return (vk_node->data_type << 16) | (vk_node->name_len & 0xFFFF);
}

// ============================================================================
// 5. カーネルエントリポイント（一番下に配置！）
// ============================================================================
__attribute__((section(".text.entry")))
void mainkernel(FramebufferInfo* fbinfo) {
    __asm__ __volatile__("cli"); // 割り込み禁止
    
    init_gdt();
    for (int i = 0; i < 256; i++) {
        set_idt_entry(i, dummy_handler);
    }
    load_idt();

    // ① 4096バイト目：ルート nk セル (フォルダ) の構築
    HiveCell* nk_cell = (HiveCell*)&mock_hive_data[4096 + 0];
    nk_cell->size = -80; 
    HiveKeyNode* nk_node = (HiveKeyNode*)nk_cell->data;
    nk_node->signature[0] = 0x6E; nk_node->signature[1] = 0x6B; // "nk"
    nk_node->num_values = 1;              
    nk_node->values_list_offset = 128;   

    // ② 4096 + 128バイト目：値リストセルの構築
    HiveCell* vl_cell = (HiveCell*)&mock_hive_data[4096 + 128];
    vl_cell->size = -16;
    uint32_t* vl_data = (uint32_t*)vl_cell->data;
    vl_data[0] = 256;                     

    // ③ 4096 + 256バイト目：本物の vk セルの構築
    HiveCell* vk_cell = (HiveCell*)&mock_hive_data[4096 + 256];
    vk_cell->size = -40;
    HiveValueNode* vk_node = (HiveValueNode*)vk_cell->data;
    vk_node->signature[0] = 0x76; vk_node->signature[1] = 0x6B; // "vk"
    vk_node->name_len = 6;                
    vk_node->data_type = 1;               

    // 【運命のパース実行】
    volatile int parse_result = parse_hive_value_blind(mock_hive_data);

    // 最後に RAX レジスタに結果を強制注入してCPUをハルト停止
    __asm__ __volatile__(
        "movl %0, %%eax\n\t"
        "hlt"
        : : "r"(parse_result) : "eax"
    );

    while (1) {
        __asm__("hlt");
    }
}
