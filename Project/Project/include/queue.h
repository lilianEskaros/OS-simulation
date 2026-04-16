#ifndef QUEUE_H
#define QUEUE_H

#include "os_core.h"
#include <stdio.h>
#include <stdlib.h>

// Process States 
typedef enum {
    READY,
    RUNNING,
    BLOCKED,
    FINISHED
} ProcessState;

// Node for the Linked List Queue
typedef struct Node {
    PCB *process;          // Pointer to the process's PCB
    struct Node *next;     // Pointer to the next node in the queue
} Node;

// Queue Structure
typedef struct {
    Node *head;
    Node *tail;
    int size;
} Queue;

Queue* createQueue();
void enqueue(Queue *q, PCB *p);
PCB* dequeue(Queue *q);
int isEmpty(Queue *q);
void print_queue(Queue* q, const char* queue_name); // Required for evaluation output [cite: 124]
PCB* find_and_remove_best_hrrn(Queue* q);
void remove_from_queue(Queue** q_ptr, PCB* process_to_remove);
void freeQueue(Queue* q);
#endif