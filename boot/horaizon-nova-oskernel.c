#include "../includemyos/framebuffer.h"
#include "../includemyos/kernel/idt.h"
#include "../includemyos/kernel/hivefile/hivefile.h" // ハイブ構造体定義
#include "../includemyos/kernel/pagetable/pagetable.h"                               // ページテーブルヘッダ
#include <stdint.h>

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

// 大きめの模擬ハイブメモリを確保（4096Bヘッダ + 1024Bデータ領域）


// ============================================================================
// 3. 各種関数の実装
// ============================================================================
void init_gdt() {
    struct GDTR gdtr;
    gdt[0].limit_low = 0; gdt[0].base_low = 0; gdt[0].base_middle = 0; gdt[0].access_byte = 0; gdt[0].flags_limit_high = 0; gdt[0].base_high = 0;
    gdt[1].limit_low = 0xFFFF; gdt[1].base_low = 0; gdt[1].base_middle = 0; gdt[1].access_byte = 0x9A; gdt[1].flags_limit_high = 0xA0; gdt[1].base_high = 0;
    gdt[2].limit_low = 0xFFFF; gdt[2].base_low = 0; gdt[2].base_middle = 0; gdt[2].access_byte = 0x92; gdt[2].flags_limit_high = 0xC0; gdt[2].base_high = 0;
    gdtr.limit = sizeof(gdt) - 1; gdtr.base = (uint64_t)&gdt;
    __asm__ volatile("lgdt %0\n\t" "mov $0x10, %%ax\n\t" "mov %%ax, %%ds\n\t" "mov %%ax, %%es\n\t" "mov %%ax, %%ss\n\t" "mov %%ax, %%fs\n\t" "mov %%ax, %%gs\n\t" "pushq $0x08\n\t" "leaq 1f(%%rip), %%rax\n\t" "pushq %%rax\n\t" "lretq\n\t" "1:\n\t" : : "m"(gdtr) : "rax", "ax");
}

void set_idt_entry(int vector, void (*handler)()) {
    uint64_t addr = (uint64_t)handler;
    idt[vector].offset_low = addr & 0xFFFF; idt[vector].selector = 0x08; idt[vector].ist = 0; idt[vector].type_attr = 0x8E; idt[vector].offset_mid = (addr >> 16) & 0xFFFF; idt[vector].offset_high = (addr >> 32) & 0xFFFFFFFF; idt[vector].zero = 0;
}

void load_idt() { struct IDTR idtr; idtr.limit = sizeof(idt) - 1; idtr.base = (uint64_t)&idt; __asm__ volatile("lidt %0" : : "m"(idtr)); }
__attribute__((interrupt)) void dummy_handler(struct InterruptFrame* frame) {}

// ============================================================================
// 4. ハイブファイル（レジストリ）解析ロジック
// ============================================================================
int parse_hive_value_blind(void* hive_base_ptr) {
    HiveBaseBlock* base = (HiveBaseBlock*)hive_base_ptr;
    
    // 【修正】配列の各要素をインデックスで正しく比較
    if (base->signature[0] != 0x72 || base->signature[1] != 0x65 ||
        base->signature[2] != 0x67 || base->signature[3] != 0x66) return 0xDEAD0001;
        
    uint32_t root_offset = base->root_cell_offset;
    HiveCell* root_cell = (HiveCell*)((uintptr_t)hive_base_ptr + 4096 + root_offset);
    HiveKeyNode* root_node = (HiveKeyNode*)(root_cell->data);
    
    // 【修正】配列の各要素をインデックスで正しく比較
    if (root_node->signature[0] != 0x6E || root_node->signature[1] != 0x6B) return 0xDEAD0002;
    if (root_node->num_values == 0) return 0xDEAD0003;
    
    uint32_t v_list_offset = root_node->values_list_offset;
    HiveCell* v_list_cell = (HiveCell*)((uintptr_t)hive_base_ptr + 4096 + v_list_offset);
    uint32_t* vk_offset_array = (uint32_t*)(v_list_cell->data);
    uint32_t first_vk_offset = vk_offset_array[0]; // 【修正】インデックス[0]を指定

    HiveCell* vk_cell = (HiveCell*)((uintptr_t)hive_base_ptr + 4096 + first_vk_offset);
    HiveValueNode* vk_node = (HiveValueNode*)(vk_cell->data);
    
    // 【修正】配列の各要素をインデックスで正しく比較
    if (vk_node->signature[0] != 0x76 || vk_node->signature[1] != 0x6B) return 0xDEAD0004;
    
    return (vk_node->data_type << 16) | (vk_node->name_len & 0xFFFF);
}

// ============================================================================
// 5. カーネルエントリポイント
// ============================================================================
// 5. カーネルエントリポイント
// ============================================================================
__attribute__((section(".text.entry")))
void mainkernel(FramebufferInfo* fbinfo) {
    __asm__ __volatile__("cli"); // 割り込み禁止
    
    // 1. CPUディスクリプタの基本初期化
    init_gdt();
    for (int i = 0; i < 256; i++) set_idt_entry(i, dummy_handler);
    load_idt();

    // ★【重要】かつてクラッシュ（#PF）の原因だった nk_cell や vk_cell への
    // 直接代入コード（nk_cell->size = -80; など）は、すべて綺麗に消去されています。

    // 2. 独立モジュール化したパーサーの実行
    // ヘッダーで定義した固定データをそのまま安全にパースします
    volatile int parse_result = parse_and_verify_hive((void*)mock_hive_data);

    // 3. 安全・確実な新ページテーブル（CR3/CR4）の動的ロード
    uint64_t real_vram_phys = (uint64_t)fbinfo->framebuffer; 
    setup_kernel_page_table(real_vram_phys);

    // 4. パース結果（成功なら 1）を EAX レジスタに叩き込む
    __asm__ __volatile__("movl %0, %%eax" : : "r"(parse_result) : "eax");

    // 5. 安全なハルト無限ループ
    while (1) {
        __asm__ __volatile__("hlt");
    }
}