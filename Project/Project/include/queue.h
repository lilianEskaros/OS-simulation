#ifndef QUEUE_H
#define QUEUE_H

#include "os_core.h"
#include <stdio.h>
#include <stdlib.h>


bool is_empty(Queue* q);
Queue* createQueue();
void enqueue(Queue *q, PCB *p);
PCB* dequeue(Queue *q);
int isEmpty(Queue *q);
void print_queue(Queue* q, const char* queue_name); // Required for evaluation output [cite: 124]
PCB* find_and_remove_best_hrrn(Queue* q);
void remove_from_queue(Queue** q_ptr, PCB* process_to_remove);
void freeQueue(Queue* q);
#endif