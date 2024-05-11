section .text
extern current_task
global switch_to_task_asm
switch_to_task_asm:

    ; Save registers
    mov [current_task+16], rax
    mov [current_task+16+8], rbx
    mov [current_task+16+16], rcx
    mov [current_task+16+24], rdx
    mov [current_task+16+32], rdi
    mov [current_task+16+40], rsi
    mov [current_task+16+48], r8
    mov [current_task+16+56], r9
    mov [current_task+16+64], r10
    mov [current_task+16+72], r11
    mov [current_task+16+80], r12
    mov [current_task+16+88], r13
    mov [current_task+16+96], r14
    mov [current_task+16+104], r15
    mov [current_task+16+112], rbp
 

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

    ; Restore registers
    mov rax, [rdi+16]
    mov rbx, [rdi+16+8]
    mov rcx, [rdi+16+16]
    mov rdx, [rdi+16+24]
    mov rsi, [rdi+16+40]
    mov r8, [rdi+16+48]
    mov r9, [rdi+16+56]
    mov r10, [rdi+16+64]
    mov r11, [rdi+16+72]
    mov r12, [rdi+16+80]
    mov r13, [rdi+16+88]
    mov r14, [rdi+16+96]
    mov r15, [rdi+16+104]
    mov rbp, [rdi+16+112]
    mov rdi, [rdi+16+32]

    ret
