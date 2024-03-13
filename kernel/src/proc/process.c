#include "process.h"
#include "devices/ioapic.h"
#include "klibc/string.h"
#include "klibc/io.h"
#include "klibc/alloc.h"

typedef struct pcb process_t;
process_t* current_task;
size_t process_id_tracker = 0;

extern void switch_to_task_asm(struct pcb* next_proc);

void init_multitasking() {
    current_task = malloc(sizeof(process_t));
    asm volatile ("mov %%rsp, %0" : "=r" (current_task->kernel_top) : : "memory");
    asm volatile ("mov %%cr3, %0" : "=r" (current_task->pml4) : : "memory");
    current_task->next = NULL;
    current_task->status = RUNNING;
    current_task->priority = HIGH;
    current_task->id = process_id_tracker;
    strncpy(current_task->name, "KERNEL", MAX_TASK_NAME);
    current_task->cpu_time = 0;
    process_id_tracker++;
}

void switch_to_task(struct pcb* next_proc) {
    if (next_proc == NULL) {
        return;
    }
    mask_all_irq();

    switch_to_task_asm(next_proc);

    unmask_all_irq();
}