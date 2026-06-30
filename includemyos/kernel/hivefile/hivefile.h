#ifndef HIVEFILE_H
#define HIVEFILE_H

#include <stdint.h>

#pragma pack(push, 1)

// ① ファイル先頭のヘッダ（4096バイト）
typedef struct {
    char     signature[4];          // "regf"
    uint32_t primary_sequence;      // シーケンス番号1
    uint32_t secondary_sequence;    // シーケンス番号2
    uint64_t last_written_time;     // FILETIME形式タイムスタンプ
    uint32_t major_version;         // 1
    uint32_t minor_version;         // 3 または 5
    uint32_t file_type;             // 0 = Normal Hive
    uint32_t file_format;           // 1 = Direct Memory Architecture
    uint32_t root_cell_offset;      // ルートのnkセルへの相対オフセット
    uint32_t hive_bins_data_size;   // 全hbinの合計サイズ
    uint32_t clustering_factor;     // 1
    char     file_name[64];         // UTF-16LEでのファイル名（任意）
    char     reserved1[396];        // パディング（512バイト目まで）
    uint32_t checksum;              // 先頭508バイトのXOR-32チェックサム
    char     reserved2[3584];       // 4096バイト境界までの残りパディング
} HiveBaseBlock;

// ② Hive Bin Header (4KB境界ごとのヘッダ)
typedef struct {
    char     signature[4];          // "hbin"
    uint32_t offset_from_first_bin; // 先頭hbinからのオフセット
    uint32_t size;                  // このBinのサイズ（4096の倍数）
    char     reserved[8];
    uint64_t timestamp;
} HiveBinHeader;

// ③ Cell共通ヘッダ (hbinの中に敷き詰められる)
typedef struct {
    int32_t  size;                  // 負数＝使用中、正数＝空き領域
    char     data[1];               // ここからnkやvkが始まる（可変長データ用のアドレス受け）
} HiveCell;

// ④ nk (Key) Cell - ディレクトリ構造のノード
typedef struct {
    char     signature[2];          // "nk"
    uint16_t flags;                 // 0x0020 = Root, 0x0008 = Subkey
    uint64_t last_written_time;
    uint32_t parent_key_offset;     // 親nkのオフセット
    uint32_t num_subkeys;           // 子キーの数
    uint32_t subkeys_list_offset;   // 子キーリスト(lf/lh/li等)へのオフセット
    uint32_t num_values;            // このキーが持つ値（vk）の数
    uint32_t values_list_offset;    // vkのオフセット配列へのオフセット
    uint32_t security_key_offset;
    uint32_t class_name_offset;
    uint16_t max_name_len;
    uint16_t max_class_len;
    uint16_t max_value_name_len;
    uint16_t max_value_data_len;
    uint32_t work_var;
    uint16_t name_len;              // キー名の長さ
    uint16_t class_len;
    char     name[1];               // キー名文字列実体（可変長）
} HiveKeyNode;

// ⑤ vk (Value) Cell - 設定の値（ファイルの実体）
typedef struct {
    char     signature[2];          // "vk"
    uint16_t name_len;              // 値の名前の長さ（0の場合は「(既定)」を表す）
    uint32_t data_len;              // データの長さ（最上位ビットが1の場合は特殊な格納方法）
    uint32_t data_offset;           // データが保存されている場所のオフセット
    uint32_t data_type;             // データ型（1=文字列、4=32bit数値 など）
    uint16_t flags;                 // 0x0001 = 名前がASCII（通常はこれ）
    uint16_t spare;
    char     name;               // 値の名前（可変長、name_lenバイト分）
} HiveValueNode;
// 4096バイトのベースブロック + データ領域 の模擬ハイブデータを
// 最初から書き換え不要の「固定値（const）」として定義します。
// これにより、起動直後のメモリ書き込み違反（#PF）を完全に回避します！
__attribute__((aligned(4096)))
static const uint8_t mock_hive_data[4096 + 512] = {
    // --- [0〜4095バイト目] ベースブロック (ヘッダー) ---
    'r', 'e', 'g', 'f',          // 0x00: マジックナンバー
    0x01, 0x00, 0x00, 0x00,      // 0x04: シーケンス1
    0x01, 0x00, 0x00, 0x00,      // 0x08: シーケンス2
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x0c: タイムスタンプ
    0x01, 0x00, 0x00, 0x00,      // 0x14: Major Version (1)
    0x03, 0x00, 0x00, 0x00,      // 0x18: Minor Version (3)
    0x00, 0x00, 0x00, 0x00,      // 0x1c: File Type (0)
    0x01, 0x00, 0x00, 0x00,      // 0x20: File Format (1)
    0x00, 0x00, 0x00, 0x00,      // 0x24: ★ルートセル(nk)への相対オフセット (0x00000000 = Hbin直後)
    
    // 4096バイト目（Hbin領域の開始）まで0で埋める
    [4096 + 0]  = 0xB0, 0xFF, 0xFF, 0xFF, // Hbin直後のセルサイズ: -80 (0xFFFFFFB0)
    [4096 + 4]  = 'n', 'k',              // nkシグネチャ
    [4096 + 6]  = 0x20, 0x00,            // flags: 0x0020 (Root)
    
    // サブキーや値のリストへのオフセット（必要に応じて足していきます）
    [4096 + 24] = 0x01, 0x00, 0x00, 0x00, // num_values = 1
    [4096 + 28] = 0x80, 0x00, 0x00, 0x00, // values_list_offset = 128 (0x80)

    // 128バイト目（0x80）：vl (Value List) セル
    [4096 + 128] = 0xF0, 0xFF, 0xFF, 0xFF, // セルサイズ: -16 (0xFFFFFFF0)
    [4096 + 132] = 0x00, 0x01, 0x00, 0x00, // vkへのオフセット配列 [0] = 256 (0x0100)

    // 256バイト目（0x0100）：vk (Value) セル
    [4096 + 256] = 0xD8, 0xFF, 0xFF, 0xFF, // セルサイズ: -40 (0xFFFFFFD8)
    [4096 + 260] = 'v', 'k',              // vkシグネチャ
    [4096 + 262] = 0x06, 0x00,            // name_len = 6
    [4096 + 264] = 0x04, 0x00, 0x00, 0x00, // data_len = 4
    [4096 + 268] = 0x06, 0x00, 0x01, 0x00, // data_offset（パース結果 0x00010006）
    [4096 + 272] = 0x04, 0x00, 0x00, 0x00, // data_type = 4 (REG_DWORD)
};



#pragma pack(pop)

// 関数プロトタイプ宣言
int verify_hive_header(HiveBaseBlock* base);
int parse_and_verify_hive(void* hive_base_ptr);

#endif // HIVEFILE_H
