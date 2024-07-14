#include "schedule.h"
#include "devices/ioapic.h"

extern tcb_t* current_task;

extern void switch_to_task_asm(tcb_t* next, uint64_t* kernel_top);

static int switch_to_task(tcb_t* next) {

    if (next == NULL) {
        return EINVARG;
    }

    switch_to_task_asm(next, &current_task->kernel_top);
    return 0;
}

void schedule() {
    switch_to_task(current_task->next);
}