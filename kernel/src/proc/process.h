#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include <stddef.h>

#define MAX_TASK_NAME 64
#define KERNEL_STACK_SIZE 0x1000

enum __attribute__((packed)) TASK_STATUS {
    READY   = 0b00000000,
    WAITING = 0b00000001,
    RUNNING = 0b00000010
};

enum __attribute__((packed)) TASK_PRIORITY {
    HIGH    = 0b00000000,
    MEDIUM  = 0b00000001,
    LOW     = 0b00000010
};

struct task_regs {
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
    uint64_t rbp;
    uint64_t rsp;
    uint64_t rip;
    uint64_t ss;
    uint64_t cs;
    uint64_t fs;
    uint64_t rflags;
    uint64_t cr3;
};


typedef struct process {
    uint64_t kernel_top;
    uint64_t cr3;
    struct process* next;
    enum TASK_STATUS status;
} tcb_t; 

void init_multitasking();
void init_kernel_cpu_info();
tcb_t* create_process(void (*main)());
void add_process(tcb_t* process);
void test_proc_entry();

#endif