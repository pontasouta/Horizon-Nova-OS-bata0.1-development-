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


#pragma pack(pop)

// 関数プロトタイプ宣言
int verify_hive_header(HiveBaseBlock* base);

#endif // HIVEFILE_H
