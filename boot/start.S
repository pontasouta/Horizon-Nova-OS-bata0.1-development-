[bits 64]
extern mainkernel ; C言語側の関数を呼ぶ宣言

section .text
global _start
_start:
    ; 1. スタックポインタの初期化
    mov rsp, kernel_stack_top
    
    ; 2. C言語のメイン関数（mainkernel）へジャンプ
    ; 引数（fbinfo）は通常 RDI レジスタ経由で渡されます
    call mainkernel

    ; 3. 万が一戻ってきたら停止させる
.halt:
    hlt
    jmp .halt

section .bss
align 16
kernel_stack:
    resb 16384 ; 16KBのスタック領域
kernel_stack_top: