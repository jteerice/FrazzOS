section .text
global load_tss
load_tss:
    push rbp
    mov rbp, rsp
    mov rax, [rdi]
    ltr ax
    mov rsp, rbp
    pop rbp
    ret