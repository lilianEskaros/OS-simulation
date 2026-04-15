// os_core.h
#ifndef OS_CORE_H
#define OS_CORE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// --- Configuration Constants ---
#define MEMORY_SIZE 40           // [cite: 32] // need to check lily
#define MAX_PROCESSES 10
#define INSTRUCTIONS_PER_SLICE 2 // For Round Robin [cite: 104] // need to check lina 
#define VARIABLES_PER_PROCESS 3  // [cite: 39] // need to check chris 
#define MAX_VALUE_LENGTH 256 
#define MAX_FILE_BUFFER 2048 // storage buffer hold the entire content of a file in a single string 


#define MUTEX_FILE "file" // need to check mickeys part in main use these 
#define MUTEX_INPUT "userInput"
#define MUTEX_OUTPUT "userOutput"

// --- Enums ---
typedef enum {
    READY,
    RUNNING,
    BLOCKED,
    FINISHED
} ProcessState;

// --- Data Structures ---
typedef struct {
    int pid;             // [cite: 53]
    ProcessState state;  // [cite: 54]
    int pc;              // Program Counter [cite: 55]
    int mem_start;       // Memory Boundaries [cite: 56]
    int mem_end;         // Memory Boundaries [cite: 56]
    int instruction_end; // deh 3ashan ne3raf n set finished state lel process
    int arrival_time;    // [cite: 37]
    int burst_time;      // Required for HRRN [cite: 101]
    int waiting_time;    // Required for HRRN [cite: 101]
    int priorityLevel;   // Used if implementing MLFQ [cite: 107]
} PCB;

typedef struct {
    char name[MAX_VALUE_LENGTH];
    char value[MAX_VALUE_LENGTH];
} MemoryWord;
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

typedef struct {
    char resource_name[20]; 
    bool is_locked;
    int owner_pid;          
    Queue blocked_queue;    // [cite: 90]
} Mutex;

// --- Global Variables (Extern Definitions) ---
extern MemoryWord memory[MEMORY_SIZE];
extern Queue ready_queue;              
extern Queue general_blocked_queue;    

extern Mutex file_mutex;               
extern Mutex input_mutex;              
extern Mutex output_mutex;             

extern int clock_cycle;

#endif // OS_CORE_H