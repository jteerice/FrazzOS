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


struct process {
    uintptr_t kernel_top;
    uintptr_t cr3;
    uint8_t ring;
    struct process* next;
    enum TASK_STATUS status;
    enum TASK_PRIORITY priority;
    size_t id;
    char name[MAX_TASK_NAME];
    size_t cpu_time;
    struct task_regs regs;
};

void init_multitasking();
void init_kernel_cpu_info();
struct process* create_process(char* name, void (*main)(), uint8_t ring, enum TASK_PRIORITY priority);
void switch_to_task(struct process* next_proc, uintptr_t current_rsp);
void add_process(struct process* process);
void test_proc_entry();

#endif