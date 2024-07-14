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

tcb_t* get_next_task() {
    tcb_t* tmp = current_task;

    while (tmp->status != READY)
        tmp = tmp->next;

    return tmp;
}

void schedule() {
    tcb_t* next = get_next_task();
    switch_to_task(next);
}