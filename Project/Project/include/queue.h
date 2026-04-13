#ifndef QUEUE_H
#define QUEUE_H

#include "os_core.h"

void enqueue(Queue* q, PCB* process);
PCB* dequeue(Queue* q);
bool is_empty(Queue* q);
void print_queue(Queue* q, const char* queue_name); // [cite: 124]

#endif