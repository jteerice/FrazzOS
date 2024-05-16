#include "schedule.h"

extern tcb_t* current_task;

extern void switch_to_task_asm(tcb_t* next, uint64_t* kernel_top);

int switch_to_task(tcb_t* next) {

    if (next == NULL) {
        return EINVARG;
    }

    switch_to_task_asm(next, &current_task->kernel_top);
    return 0;
}