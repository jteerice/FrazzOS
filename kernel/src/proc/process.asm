section .text
global switch_to_task_asm
switch_to_task_asm:

    ; Save registers
    push rax
    push rbx
    push rcx
    push rdx
    push rdi
    push rsi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    push rbp
 

    ; Save rsp to current task process control block
    mov [rsi], rsp

    ; Setup new task
    mov rsp, [rdi]
    mov rax, [rdi+8]
    mov rcx, cr3
    cmp rax, rcx
    je .virt_no_change
    mov cr3, rax

.virt_no_change:

    ; Restore registers
    pop rbp
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rsi
    pop rdi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    ret