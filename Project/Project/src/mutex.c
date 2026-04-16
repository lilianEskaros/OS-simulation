#include "../include/mutex.h"
#include "../include/os_core.h"
#include "../include/scheduler.h"

// Note: These should be declared in your main or global context
Mutex file_mutex;
Mutex input_mutex;
Mutex output_mutex;

void initialize_mutexes() {
    // Correct: assign pointer to pointer
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
}

Mutex* get_mutex(char* resourceName) {
    if (strcmp(resourceName, RESOURCE_USER_INPUT) == 0)  return &input_mutex;
    if (strcmp(resourceName, RESOURCE_USER_OUTPUT) == 0) return &output_mutex;
    if (strcmp(resourceName, RESOURCE_FILE) == 0)        return &file_mutex;
    
    printf("[Mutex] ERROR: unknown resource '%s'\n", resourceName);
    return NULL;
}

int sem_wait(Mutex* mutex, PCB* process) {
    if (mutex == NULL) return 1;

    // Resource available
    if (mutex->is_locked == false) {
        mutex->is_locked = true;
        mutex->owner_pid = process->pid;
        printf("[Mutex] Process %d acquired %s\n", process->pid, mutex->resource_name);
        return 1;
    }

    // Resource busy: Block process
    process->state = BLOCKED;
    
    // Requirement: Update the state in the memory view immediately
    update_memory_view(process);

    // Add to the specific mutex blocked queue
    enqueue(mutex->blocked_queue, process);

    // Add to the global blocked queue (used for general OS tracking/GUI)
    Queue* global_blocked = get_blocked_queue();
    enqueue(global_blocked, process);

    printf("[Mutex] Process %d BLOCKED waiting for %s\n", process->pid, mutex->resource_name);
    return 0; // Returning 0 tells the interpreter to stop the time slice
}

void sem_signal(Mutex* mutex, int pid) {
    if (mutex == NULL) return;

    // Only the owner can release the resource
    if (mutex->owner_pid != pid) {
        printf("[Mutex] ERROR: P%d tried to release %s but isn't the owner\n", 
                pid, mutex->resource_name);
        return;
    }

    printf("[Mutex] Process %d released %s\n", pid, mutex->resource_name);

    if (is_empty(mutex->blocked_queue)) {
        // No one waiting: unlock
        mutex->is_locked = false;
        mutex->owner_pid = -1;
    } 
    else {
        // Handover: Wake up the next process in line
        PCB* nextProcess = dequeue(mutex->blocked_queue);

        // 1. Remove it from the general blocked queue using your "Bypass" logic
        Queue* global_blocked = get_blocked_queue();
        remove_from_queue(&global_blocked, nextProcess);

        // 2. Set state back to READY
        nextProcess->state = READY;
        
        // 3. Update the memory view so the PCB block shows READY
        update_memory_view(nextProcess);

        // 4. Move process to the Scheduler's Ready Queue
        Queue* ready_q = get_ready_queue();
        enqueue(ready_q, nextProcess);

        // 5. Transfer ownership immediately to the unblocked process
        mutex->owner_pid = nextProcess->pid;

        printf("[Mutex] Process %d unblocked and now owns %s\n", 
                nextProcess->pid, mutex->resource_name);
    }
}

void print_mutex_state(Mutex* mutex) {
    if(mutex == NULL) return;
    printf("[Mutex '%s'] Locked: %d | Owner PID: %d\n",
        mutex->resource_name,
        mutex->is_locked,
        mutex->owner_pid);
    print_queue(&mutex->blocked_queue, "  Waiting");
}
