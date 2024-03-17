#include "process.h"
#include "smp/smp.h"
#include "klibc/memory.h"
#include "devices/ioapic.h"
#include "klibc/string.h"
#include "klibc/io.h"
#include "klibc/alloc.h"

struct process* current_task;
size_t process_id_tracker = 0;

extern void switch_to_task_asm(struct process* next_proc);

struct process* create_process(char* name, void (*main)(), uint8_t ring, enum TASK_PRIORITY priority) {
    struct process* new_proc = malloc(sizeof(struct process));
    new_proc->kernel_top = malloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE;
    new_proc->id = process_id_tracker++;
    new_proc->cpu_time = 0;
    new_proc->cr3 = current_task->cr3;
    new_proc->priority = priority;
    new_proc->status = READY;
    strncpy(new_proc, name, MAX_TASK_NAME);
    memset(&new_proc->regs, 0, sizeof(struct task_regs));
    return new_proc;
}

void init_multitasking() {
    current_task = malloc(sizeof(struct process));
    asm volatile ("mov %%rsp, %0" : "=r" (current_task->kernel_top) : : "memory");
    asm volatile ("mov %%cr3, %0" : "=r" (current_task->cr3) : : "memory");
    current_task->next = NULL;
    current_task->status = RUNNING;
    current_task->priority = HIGH;
    current_task->id = process_id_tracker;
    strncpy(current_task->name, "KERNEL", MAX_TASK_NAME);
    current_task->cpu_time = 0;
    process_id_tracker++;
}

void switch_to_task(struct process* next_proc) {
    if (next_proc == NULL) {
        return;
    }
    mask_all_irq();

    switch_to_task_asm(next_proc);

    unmask_all_irq();
}

void init_kernel_cpu_info() {
    kprint("[KERNEL] Initializing Kernel CPU Info... ");
    new_cpu_local();
    get_cpu_local()->cpu_id = 0;
    kprint("Success\n");
}