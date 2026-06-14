#include "hivefile.h"

/**
 * 簡易的なメモリ比較関数
 */
static int local_memcmp(const void* s1, const void* s2, uint32_t n) {
    const unsigned char* p1 = s1;
    const unsigned char* p2 = s2;
    for (uint32_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) return p1[i] - p2[i];
    }
    return 0;
}

/**
 * メモリ上のハイブファイルを検証し、成否を数値で返す
 * @param hive_base_ptr ハイブファイルの先頭ポインタ
 * @return 1: 成功(Rootキーまで正常), 0: ヘッダ不正, -1: Rootキー不正
 */
int parse_and_verify_hive(void* hive_base_ptr) {
    if (!hive_base_ptr) return 0;

    // 1. ベースブロック（ヘッダ）のパース
    HiveBaseBlock* base = (HiveBaseBlock*)hive_base_ptr;

    // マジックナンバー "regf" のチェック
    if (local_memcmp(base->signature, "regf", 4) != 0) {
        return 0; // ヘッダ不正
    }

    // 2. ルートキー（nkセル）へのオフセット計算
    uint32_t root_offset = base->root_cell_offset;
    HiveCell* root_cell = (HiveCell*)((uintptr_t)hive_base_ptr + 4096 + root_offset);

    // 3. nk (Key) ノードとしてのパース
    HiveKeyNode* root_node = (HiveKeyNode*)(root_cell->data);

    // マジックナンバー "nk" のチェック
    if (local_memcmp(root_node->signature, "nk", 2) != 0) {
        return -1; // ルートキー不正
    }

    return 1; // すべて成功！
}
