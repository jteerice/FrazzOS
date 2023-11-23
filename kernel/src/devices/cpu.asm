section .text

global cpuid_proc_feat_info

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
