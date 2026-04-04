#ifndef ELF_H
#define ELF_H
#include <stdint.h>

typedef uint64_t Elf64_Addr;
typedef uint64_t Elf64_Off;
typedef uint16_t Elf64_Half;
typedef uint32_t Elf64_Word;
typedef int32_t  Elf64_Sword;
typedef uint64_t Elf64_Xword;
typedef int64_t  Elf64_Sxword;

#define EI_NIDENT 16

typedef struct {
    unsigned char e_ident[EI_NIDENT]; /* ELF識別情報（マジックナンバー等） */
    Elf64_Half    e_type;      /* オブジェクトファイル形式（実行可能ファイル等） */
    Elf64_Half    e_machine;   /* ターゲットアーキテクチャ（x86_64など） */
    Elf64_Word    e_version;   /* ファイルバージョン */
    Elf64_Addr    e_entry;     /* エントリーポイント（実行開始アドレス） */
    Elf64_Off     e_phoff;     /* プログラムヘッダテーブルのファイルオフセット */
    Elf64_Off     e_shoff;     /* セクションヘッダテーブルのファイルオフセット */
    Elf64_Word    e_flags;     /* プロセッサ固有のフラグ */
    Elf64_Half    e_ehsize;    /* このELFヘッダ自体のサイズ */
    Elf64_Half    e_phentsize; /* プログラムヘッダテーブルの1エントリのサイズ */
    Elf64_Half    e_phnum;     /* プログラムヘッダのエントリ数 */
    Elf64_Half    e_shentsize; /* セクションヘッダテーブルの1エントリのサイズ */
    Elf64_Half    e_shnum;     /* セクションヘッダのエントリ数 */
    Elf64_Half    e_shstrndx;  /* セクション名文字列テーブルのインデックス */
} Elf64_Ehdr;
typedef struct {
    Elf64_Word  p_type;   /* セグメントの種類（PT_LOADならロード対象） */
    Elf64_Word  p_flags;  /* セグメントの属性（読み書き実行権限） */
    Elf64_Off   p_offset; /* ファイル内でのデータ開始位置 */
    Elf64_Addr  p_vaddr;  /* メモリ上の仮想アドレス */
    Elf64_Addr  p_paddr;  /* 物理アドレス（通常はvaddrと同じ） */
    Elf64_Xword p_filesz; /* ファイル内でのデータサイズ */
    Elf64_Xword p_memsz;  /* メモリ上での展開サイズ（BSSなどでfileszより大きくなる） */
    Elf64_Xword p_align;  /* メモリ配置のアライメント（2MBや4KBなど） */
} Elf64_Phdr;
#endif /* ELF_H */