#include "../include/mutex.h"
#include "../include/os_core.h"
#include "../include/scheduler.h"
#include "../include/queue.h"

Mutex file_mutex;
Mutex input_mutex;
Mutex output_mutex;

// Initialize the global mutexes defined in os_core.h
void initialize_mutexes() {
    output_mutex.blocked_queue = createQueue(); 
    output_mutex.is_locked = false;
    output_mutex.owner_pid = -1;
    strcpy(output_mutex.resource_name, RESOURCE_USER_OUTPUT);

    input_mutex.blocked_queue = createQueue();
    input_mutex.is_locked = false;
    input_mutex.owner_pid = -1;
    strcpy(input_mutex.resource_name, RESOURCE_USER_INPUT);

    file_mutex.blocked_queue = createQueue();
    file_mutex.is_locked = false;
    file_mutex.owner_pid = -1;
    strcpy(file_mutex.resource_name, RESOURCE_FILE);

    printf("[Mutex] All mutexes initialized successfully.\n");
}

Mutex* get_mutex(char* resourceName) {
    if (strcmp(resourceName, RESOURCE_USER_INPUT) == 0)  return &input_mutex;
    if (strcmp(resourceName, RESOURCE_USER_OUTPUT) == 0) return &output_mutex;
    if (strcmp(resourceName, RESOURCE_FILE) == 0)        return &file_mutex;
    
    printf("[Mutex] ERROR: unknown resource '%s'\n", resourceName);
    return NULL;
}

// Unified naming to semWait to match interpreter.c
int semWait(char* resourceName, PCB* process) {
    Mutex* mutex = get_mutex(resourceName);
    if (mutex == NULL) return 1;

    // Resource available OR already owned by this process (Handover from semSignal)
    if (mutex->is_locked == false || mutex->owner_pid == process->pid) {
        mutex->is_locked = true;
        mutex->owner_pid = process->pid;
        printf("[Mutex] Process %d acquired %s\n", process->pid, mutex->resource_name);
        return 1; // It owns it, let it execute the instruction!
    }

    // Resource busy: Block process
    process->state = BLOCKED;
    
    update_memory_view(process);
    enqueue(mutex->blocked_queue, process);

    Queue* global_blocked = get_blocked_queue();
    enqueue(global_blocked, process);

    printf("[Mutex] Process %d BLOCKED waiting for %s\n", process->pid, mutex->resource_name);
    return 0; 
}

// Unified naming to semSignal to match interpreter.c
void semSignal(char* resourceName, int pid) {
    Mutex* mutex = get_mutex(resourceName);
    if (mutex == NULL) return;

    if (mutex->owner_pid != pid) {
        printf("[Mutex] ERROR: P%d tried to release %s but isn't the owner\n", pid, mutex->resource_name);
        return;
    }

    printf("[Mutex] P%d released %s\n", pid, mutex->resource_name);

    if (is_empty(mutex->blocked_queue)) {
        mutex->is_locked = false;
        mutex->owner_pid = -1;
    } else {
        PCB* nextProcess = dequeue(mutex->blocked_queue);

        // Remove from general blocked queue
        Queue* global_blocked = get_blocked_queue();
        remove_from_queue(&global_blocked, nextProcess);

        nextProcess->state = READY;
        update_memory_view(nextProcess);

        // --- FIXED FOR MLFQ ---
        if (current_policy == MLFQ) {
            // Unblocked processes go to high priority (Queue 0)
            nextProcess->priorityLevel = 0; 
            enqueue(get_mlfq_queues()[0], nextProcess);
        } else {
            enqueue(get_ready_queue(), nextProcess);
        }

        mutex->owner_pid = nextProcess->pid;
        printf("[Mutex] P%d unblocked and now owns %s\n", nextProcess->pid, mutex->resource_name);
    }
}

void print_mutex_state(Mutex* mutex) {
    if(mutex == NULL) return;
    printf("[Mutex '%s'] Locked: %d | Owner PID: %d\n",
        mutex->resource_name,
        mutex->is_locked,
        mutex->owner_pid);
    print_queue(mutex->blocked_queue, "Waiting List");
}