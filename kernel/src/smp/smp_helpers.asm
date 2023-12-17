[bits 64]
section .text

global smp_check_flag
smp_check_flag:
    xor rax, rax
    mov al, byte [0x510]
    ret

global prepare_ap_trampoline
prepare_ap_trampoline:
    push rbp
    mov rbp, rsp
    mov word [0x510], 0
    mov qword [0x520], rdi
    mov qword [0x540], rsi     
    sgdt [0x560]
    sidt [0x580]
    mov rsp, rbp
    pop rbp
    ret