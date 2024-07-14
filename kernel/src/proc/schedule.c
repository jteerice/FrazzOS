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

static tcb_t* get_next_task() {
    tcb_t* tmp = current_task->next;

    while(tmp->status != READY) {
        // No ready processes, return the current_process
        if (tmp == current_task)
            return tmp;
        tmp = tmp->next;
    }

    current_task->status = READY;
    tmp->status = RUNNING;

    return tmp;
}

void schedule() {
    tcb_t* next = get_next_task();
    switch_to_task(next);
}