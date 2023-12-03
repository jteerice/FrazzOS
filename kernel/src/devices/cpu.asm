section .text

global cpuid_proc_feat_info
global outw
global inw

cpuid_proc_feat_info:
    push rbp
    mov rbp, rsp
    mov eax, 1
    cpuid
    mov dword [rdi], eax
    mov dword [rdi + 4], edx
    mov dword [rdi + 8], ecx
    mov dword [rdi + 12], ebx
    mov rsp, rbp
    pop rbp
    ret

outw:
    push rbp
    mov rbp, rsp
    mov rax, rsi
    out  dx, ax
    mov rsp, rbp
    pop rbp
    ret

inw:
    push rbp
    mov rbp, rsp
    in ax, dx
    mov rsp, rbp
    pop rbp
    ret