bits 16

extern ap_entry
global ap_trampoline

ap_trampoline:
    cli
    cld
    xor ax, ax
    mov ds, ax
    mov word [0x510], 1
    jmp 0x00:(0x1000 + ap_trampoline - fix_cs)
fix_cs:
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    lgdt [0x1000 + ap_trampoline - gdt_ptr]
    mov edx, dword [0x520]
    mov cr3, edx
    mov eax, cr4
    or eax, 1 << 5
    or eax, 1 << 7
    mov cr4, eax
    mov ecx, 0xc0000080
    rdmsr
    or eax, 0x00000100
    wrmsr
    mov eax, cr0
    or eax, 0x80000001
    and eax, ~(0x60000000)
    mov cr0, eax
    jmp 0x08:(0x1000 + ap_trampoline - init_64)
init_64:
bits 64
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov ax, 0x20
    mov fs, ax
    mov gs, ax

    mov rsp, qword [0x540]

    lgdt [0x560]
    lidt [0x580]

    call ap_entry

align 16
gdt_ptr:
    dw .gdt_end - .gdt_start - 1  ; GDT size
    dq .gdt_start                 ; GDT start

align 16
.gdt_start:

; Null descriptor (required)
.null_descriptor:
    dw 0x0000           ; Limit
    dw 0x0000           ; Base (low 16 bits)
    db 0x00             ; Base (mid 8 bits)
    db 00000000b        ; Access
    db 00000000b        ; Granularity
    db 0x00             ; Base (high 8 bits)

; 64 bit mode kernel
.kernel_code_64:
    dw 0x0000           ; Limit
    dw 0x0000           ; Base (low 16 bits)
    db 0x00             ; Base (mid 8 bits)
    db 10011010b        ; Access
    db 00100000b        ; Granularity
    db 0x00             ; Base (high 8 bits)

.kernel_data:
    dw 0x0000           ; Limit
    dw 0x0000           ; Base (low 16 bits)
    db 0x00             ; Base (mid 8 bits)
    db 10010010b        ; Access
    db 00000000b        ; Granularity
    db 0x00             ; Base (high 8 bits)

.user_data_64:
    dw 0x0000           ; Limit
    dw 0x0000           ; Base (low 16 bits)
    db 0x00             ; Base (mid 8 bits)
    db 11110010b        ; Access
    db 00000000b        ; Granularity
    db 0x00             ; Base (high 8 bits)

.gdt_end:
