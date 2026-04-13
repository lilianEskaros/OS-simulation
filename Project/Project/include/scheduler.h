#ifndef SCHEDULER_H
#define SCHEDULER_H
#include "queue.h"
#include "os_core.h"

extern Queue* ready_queue;
extern Queue* blocked_queue;
extern PCB* curr_process;

void schedule_RR();   // [cite: 103]
void schedule_HRRN(); // [cite: 100]

#endif