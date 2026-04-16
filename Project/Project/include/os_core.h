#ifndef OS_CORE_H
#define OS_CORE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// --- Configuration Constants ---
#define MEMORY_SIZE 40           
#define MAX_PROCESSES 10
#define INSTRUCTIONS_PER_SLICE 2 
#define VARIABLES_PER_PROCESS 3  
#define MAX_VALUE_LENGTH 256 
#define MAX_FILE_BUFFER 2048 

#define RESOURCE_FILE "file"
#define RESOURCE_USER_INPUT "userInput"
#define RESOURCE_USER_OUTPUT "userOutput"

// --- Enums ---
typedef enum {
    READY,
    RUNNING,
    BLOCKED,
    FINISHED
} ProcessState;

// --- Data Structures ---
typedef struct {
    int pid;             
    ProcessState state;  
    int pc;               // Program Counter
    int mem_start;       // Memory Boundaries
    int mem_end;         // Memory Boundaries
    int arrival_time;    
    int burst_time;      // Required for HRRN
    int waiting_time;    // Required for HRRN
    int priorityLevel;   // Used for MLFQ
    int instruction_end; // Address of the last instruction in memory
} PCB;

typedef struct {
    char name[MAX_VALUE_LENGTH];
    char value[MAX_VALUE_LENGTH];
} MemoryWord;

typedef struct Node {
    PCB *process;          
    struct Node *next;     
} Node;

typedef struct {
    Node *head;
    Node *tail;
    int size;
} Queue;

typedef struct {
    char resource_name[20]; 
    bool is_locked;
    int owner_pid;          
    Queue* blocked_queue;    // Fixed: Changed from Queue* to Queue for consistency with mutex.c
} Mutex;

// --- Global Variables (Extern Definitions) ---
extern MemoryWord memory[MEMORY_SIZE];

// Change these to pointers if your scheduler.c/main.c allocates them with createQueue()
extern Queue* ready_queue;               
extern Queue* general_blocked_queue;    

extern Mutex file_mutex;               
extern Mutex input_mutex;               
extern Mutex output_mutex;              

extern int clock_cycle;

// Function Prototypes for synchronization across files
void update_memory_view(PCB* p);
Queue* get_ready_queue();
Queue* get_blocked_queue();

#endif // OS_CORE_H/