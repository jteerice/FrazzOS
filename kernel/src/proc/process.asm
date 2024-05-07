section .text
extern current_task
global switch_to_task_asm
switch_to_task_asm:

    ; Save registers
    mov [rcx], rax
    mov [rcx+8], rbx
    mov [rcx+16], rcx
    mov [rcx+24], rdx
    mov [rcx+32], rdi
    mov [rcx+40], rsi
    mov [rcx+48], r8
    mov [rcx+56], r9
    mov [rcx+64], r10
    mov [rcx+72], r11
    mov [rcx+80], r12
    mov [rcx+88], r13
    mov [rcx+96], r14
    mov [rcx+104], r15
    mov [rcx+112], rbp
 

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

    ; Restore registers
    mov rax, [r8]
    mov rbx, [r8+8]
    mov rcx, [r8+16]
    mov rdx, [r8+24]
    mov rdi, [r8+32]
    mov rsi, [r8+40]
    mov r8, [r8+48]
    mov r9, [r8+56]
    mov r10, [r8+64]
    mov r11, [r8+72]
    mov r12, [r8+80]
    mov r13, [r8+88]
    mov r14, [r8+96]
    mov r15, [r8+104]
    mov rbp, [r8+112]

    ret