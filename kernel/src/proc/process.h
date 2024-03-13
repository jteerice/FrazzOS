#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include <stddef.h>

#define MAX_TASK_NAME 64

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

struct pcb {
    uintptr_t kernel_top;
    uintptr_t pml4;
    struct pcb* next;
    enum TASK_STATUS status;
    enum TASK_PRIORITY priority;
    size_t id;
    char name[MAX_TASK_NAME];
    size_t cpu_time;
}__attribute__((packed));

void init_multitasking();

#endif