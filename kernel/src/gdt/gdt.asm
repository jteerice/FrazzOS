section .data

gdt:
    dw 0
    dq 0

section .text
global load_gdt
global load_segment_registers
load_gdt:
    cli
    dec rsi
    mov [gdt], si
    mov [gdt+2], rdi
    lgdt [gdt]
    sti
    ret

load_segment_registers:
    push 0x08
    lea rax, [rel .reload_cs]
    push rax
    retfq

.reload_cs:
    mov ax, 0x10
    mov dx, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret