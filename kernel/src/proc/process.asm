section .text
extern current_task
global switch_to_task_asm
switch_to_task_asm:

    ; Save rsp to current task process control block
    mov [rdx], rsp

    ; Setup new task
    mov rsp, [rdi]
    mov rax, [rsi]
    mov rcx, cr3
    cmp rax, rcx
    je .virt_no_change
    mov cr3, rax

.virt_no_change:

    ret