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

// Process Control Block (PCB) Structure 
typedef struct {
    int processID;          // ID assigned at creation [cite: 53]
    ProcessState state;     // Current state of the process [cite: 54]
    int programCounter;    // Location of the next instruction [cite: 55]
    int memStart;          // Start of memory boundary [cite: 56]
    int memEnd;            // End of memory boundary [cite: 56]
    
    // Additional fields for scheduling logic
    int arrivalTime;       // Used for HRRN and arrival checks [cite: 101, 111]
    int burstTime;         // Estimated total instructions (for HRRN) 
    int waitingTime;       // Time spent in Ready Queue 
    int priorityLevel;     // Used if implementing MLFQ [cite: 107]
} PCB;

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
void printQueue(Queue *q); // Required for evaluation output [cite: 124]

#endif