section .text
extern current_task
global switch_to_task_asm
switch_to_task_asm:
    push rax
    push rcx
    push rdx
    push rbx
    push rsi
    push rdi
    push rbp

    ; Save rsp to current task process control block
    mov [current_task], rsp

    ; Setup new task
    mov rsp, [rdi]
    mov rax, [rdi+8]
    mov rcx, cr3
    cmp rax, rcx
    je .virt_no_change
    mov cr3, rax

.virt_no_change:
    pop rbp
    pop rdi
    pop rsi
    pop rbx
    pop rdx
    pop rcx
    pop rax

    ret