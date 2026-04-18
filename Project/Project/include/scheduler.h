#ifndef SCHEDULER_H
#define SCHEDULER_H
#include "queue.h"
#include "os_core.h"

extern Queue* ready_queue;
extern Queue* blocked_queue;
extern PCB* curr_process;

int schedule_RR();   // [cite: 103]
int schedule_HRRN(); // [cite: 100]
int schedule_MLFQ();
void update_waiting_times(Queue* ready_q);
void update_mlfq_waiting_times(Queue** queues);
void update_memory_view(PCB* p);

Queue** get_mlfq_queues();
#endif
