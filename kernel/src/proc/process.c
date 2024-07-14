#include "process.h"
#include "schedule.h"
#include "status.h"
#include "smp/smp.h"
#include "klibc/memory.h"
#include "devices/ioapic.h"
#include "klibc/string.h"
#include "klibc/io.h"
#include "klibc/alloc.h"

tcb_t* current_task;

static void process_init_info(tcb_t* new_proc, void (*main)()) {
    *((uint64_t*)new_proc->kernel_top) = (uint64_t)main;
    new_proc->kernel_top -= 120;
    new_proc->cr3 = current_task->cr3;
    new_proc->status = READY;
    new_proc->time_slice = QUANTUM;
}

static int process_init_stack(tcb_t* new_proc) {
    new_proc->kernel_top = (uint64_t)malloc(KERNEL_STACK_SIZE);

    if (new_proc->kernel_top == (uint64_t)-1) {
        return ENOMEM;
    }

    memset((void*)new_proc->kernel_top, 0, KERNEL_STACK_SIZE);
    new_proc->kernel_top += KERNEL_STACK_SIZE;
    return 0;
}

tcb_t* create_process(void (*main)()){
    tcb_t* new_proc = (tcb_t*)malloc(sizeof(tcb_t));

    if (process_init_stack(new_proc) < 0) {
        kprint("[[[PANIC]]] NO MEMORY\n");
        for (;;) {
            asm ("hlt");
        }
    }

    process_init_info(new_proc, main);
    return new_proc;
}

void init_multitasking() {
    kprint("[KERNEL] Initializing Scheduler... ");
    current_task = (tcb_t*)malloc(sizeof(tcb_t));

    if ((uint64_t)current_task == (uint64_t)-1) {
        kprint("[[[PANIC]]] NO MEMORY\n");
        for (;;) {
            asm ("hlt");
        }
    }

    asm volatile ("mov %%rsp, %0" : "=r" (current_task->kernel_top) : : "memory");
    asm volatile ("mov %%cr3, %0" : "=r" (current_task->cr3) : : "memory");
    current_task->next = current_task;
    current_task->status = RUNNING;
    current_task->time_slice = QUANTUM;

    kprint("Success\n");

    unmask_irq(TIMER_IRQ);
}

void add_process(tcb_t* process) {
    tcb_t* tmp = current_task->next;
    current_task->next = process;
    process->next = tmp;
}

void init_kernel_cpu_info() {
    kprint("[KERNEL] Initializing Kernel CPU Info... ");
    new_cpu_local();
    get_cpu_local()->cpu_id = 0;
    kprint("Success\n");
}