section .data

global isr_stub_table
isr_stub_table:
%assign i 0
%rep 32
    dq isr_stub_%+i
%assign i i+1
%endrep

idt:
    dw 0
    dq 0

section .text

global load_idt
load_idt:
    push rbp
    mov rbp, rsp
    mov [idt], si
    mov [idt+2], rdi
    lidt [idt]
    mov rsp, rbp
    pop rbp
    sti
    ret

global timer_irq
extern timer_irq_handler
timer_irq:
    call timer_irq_handler
    iretq

global keyboard_irq
extern keyboard_irq_handler
keyboard_irq:
    call keyboard_irq_handler 
    iretq

global divide_zero_exception
extern divide_by_zero_exception_handler
divide_zero_exception:
    call divide_by_zero_exception_handler
    iretq

%macro isr_err_stub 1
isr_stub_%+%1:
    call general_exception_handler
    iretq
%endmacro

%macro isr_no_err_stub 1
isr_stub_%+%1:
    call general_exception_handler
    iretq
%endmacro

extern general_exception_handler
isr_no_err_stub 0
isr_no_err_stub 1
isr_no_err_stub 2
isr_no_err_stub 3
isr_no_err_stub 4
isr_no_err_stub 5
isr_no_err_stub 6
isr_no_err_stub 7
isr_err_stub    8
isr_no_err_stub 9
isr_err_stub    10
isr_err_stub    11
isr_err_stub    12
isr_err_stub    13
isr_err_stub    14
isr_no_err_stub 15
isr_no_err_stub 16
isr_err_stub    17
isr_no_err_stub 18
isr_no_err_stub 19
isr_no_err_stub 20
isr_no_err_stub 21
isr_no_err_stub 22
isr_no_err_stub 23
isr_no_err_stub 24
isr_no_err_stub 25
isr_no_err_stub 26
isr_no_err_stub 27
isr_no_err_stub 28
isr_no_err_stub 29
isr_err_stub    30
isr_no_err_stub 31

