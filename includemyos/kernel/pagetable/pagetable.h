#ifndef PAGETABLE_H
#define PAGETABLE_H

#include <stdint.h>

// ページエントリの属性フラグ（x86_64仕様）
#define PAGE_PRESENT  (1ULL << 0)  // 0x01: 有効
#define PAGE_RW       (1ULL << 1)  // 0x02: 読み書き可能
#define PAGE_USER     (1ULL << 2)  // 0x04: ユーザーモードアクセス可能
#define PAGE_HUGE     (1ULL << 7)  // 0x80: 2MB巨大ページフラグ (PDエントリ用)

// 4KB境界にきっちりアライメントされた、512エントリ（4096バイト）のテーブル構造体
typedef struct {
    uint64_t entries[512];
} __attribute__((aligned(4096))) PageTable;

/**
 * カーネル専用のページテーブル（CR3）を初期化し、VRAM空間への道をこじ開ける
 * @param vram_base GOPのフレームバッファ物理アドレス
 */
static inline void setup_kernel_page_table(uint64_t vram_base) {
    // =========================================================================
    // 強制ストレートマップ用のテーブル固定配置（物理メモリ 64KB目以降）
    // =========================================================================
  // アドレスを安全な 4MB（0x400000）以降にずらす
uint64_t pml4_phys   = 0x400000;
uint64_t k_pdpt_phys = 0x401000;
uint64_t k_pd_phys   = 0x402000;
uint64_t v_pdpt_phys = 0x403000;
uint64_t v_pd_phys   = 0x404000;

    PageTable* pml4_table  = (PageTable*)pml4_phys;
    PageTable* kernel_pdpt = (PageTable*)k_pdpt_phys;
    PageTable* kernel_pd   = (PageTable*)k_pd_phys;
    PageTable* vram_pdpt   = (PageTable*)v_pdpt_phys;
    PageTable* vram_pd     = (PageTable*)v_pd_phys;

    // 全テーブルをゼロクリア
    for (int i = 0; i < 512; i++) {
        pml4_table->entries[i] = 0;
        kernel_pdpt->entries[i] = 0;
        kernel_pd->entries[i] = 0;
        vram_pdpt->entries[i] = 0;
        vram_pd->entries[i] = 0;
    }

    // 1. カーネルとスタックを含む「最初の1GB全体」を物理＝仮想で1対1マッピング
    pml4_table->entries[0] = k_pdpt_phys | PAGE_PRESENT | PAGE_RW;
    kernel_pdpt->entries[0] = k_pd_phys   | PAGE_PRESENT | PAGE_RW;

    for (int i = 0; i < 512; i++) {
        uint64_t phys_addr = (uint64_t)i * 2 * 1024 * 1024; // 2MB単位
        kernel_pd->entries[i] = phys_addr | PAGE_PRESENT | PAGE_RW | PAGE_HUGE;
    }

    // 2. 動的VRAMマッピング（スタック領域の上書き競合を回避）
    uint64_t v_pml4_idx = (vram_base >> 39) & 0x1FF;
    uint64_t v_pdpt_idx = (vram_base >> 30) & 0x1FF;
    uint64_t v_pd_idx   = (vram_base >> 21) & 0x1FF;

    if (v_pml4_idx == 0 && v_pdpt_idx == 0) {
        // 【修正】VRAMが最初の1GB内にある場合は、すでに上のループで
        // 丸ごとマップされているため、既存のマッピングを壊さないよう何もしない。
    } else {
        // VRAMが1GBより後ろにある場合は、独立した専用テーブルへ安全に流す
        pml4_table->entries[v_pml4_idx] = v_pdpt_phys | PAGE_PRESENT | PAGE_RW;
        vram_pdpt->entries[v_pdpt_idx] = v_pd_phys   | PAGE_PRESENT | PAGE_RW;

        uint64_t vram_aligned = vram_base & 0xFFFFFFFFFFE00000;
        vram_pd->entries[v_pd_idx]     = vram_aligned | PAGE_PRESENT | PAGE_RW | PAGE_HUGE;
        vram_pd->entries[v_pd_idx + 1] = (vram_aligned + 0x200000) | PAGE_PRESENT | PAGE_RW | PAGE_HUGE;
    }

    // 3. CR4のPAE/PSE強制有効化 ＆ CR3へロード
    uint64_t cr4_val;
    __asm__ volatile("mov %%cr4, %0" : "=r"(cr4_val));
    cr4_val |= (1ULL << 4) | (1ULL << 5); 
    __asm__ volatile("mov %0, %%cr4" : : "r"(cr4_val));

    // 新しいページテーブル（0x10000）をCR3にロード
    __asm__ volatile("mov %0, %%cr3" : : "r"(pml4_phys) : "memory");
}

#endif // PAGETABLE_H