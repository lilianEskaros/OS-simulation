#include "../include/mutex.h"

void initialize_mutexes() {
    // Set up userOutput mutex
    strcpy(output_mutex.resource_name, RESOURCE_USER_OUTPUT);
    output_mutex.is_locked = false;
    output_mutex.owner_pid = -1;
    // blocked_queue is a direct Queue struct, not a pointer
    // so we initialize its fields directly
    output_mutex.blocked_queue.front = NULL;
    output_mutex.blocked_queue.rear = NULL;

    // Set up userInput mutex
    strcpy(input_mutex.resource_name, RESOURCE_USER_INPUT);
    input_mutex.is_locked = false;
    input_mutex.owner_pid = -1;
    input_mutex.blocked_queue.front = NULL;
    input_mutex.blocked_queue.rear = NULL;

    // Set up file mutex
    strcpy(file_mutex.resource_name, RESOURCE_FILE);
    file_mutex.is_locked = false;
    file_mutex.owner_pid = -1;
    file_mutex.blocked_queue.front = NULL;
    file_mutex.blocked_queue.rear = NULL;

    printf("[Mutex] All mutexes initialized\n");
}

Mutex* get_mutex(char* resourceName) {
    if(strcmp(resourceName, RESOURCE_USER_OUTPUT) == 0) return &output_mutex;
    if(strcmp(resourceName, RESOURCE_USER_INPUT) == 0)  return &input_mutex;
    if(strcmp(resourceName, RESOURCE_FILE) == 0)        return &file_mutex;
    printf("[Mutex] ERROR: unknown resource '%s'\n", resourceName);
    return NULL;
}

int sem_wait(Mutex* mutex, PCB* process) {
    if(mutex == NULL) return 0;

    // Resource is free — give it to this process
    if(mutex->is_locked == false) {
        mutex->is_locked = true;
        mutex->owner_pid = process->pid;
        printf("[Mutex] Process %d acquired '%s'\n", process->pid, mutex->resource_name);
        return 1;
    }

    // Resource is taken — block this process
    process->state = BLOCKED;
    printf("[Mutex] Process %d BLOCKED waiting for '%s'\n", process->pid, mutex->resource_name);

    // Add to THIS mutex's blocked queue
    enqueue(&mutex->blocked_queue, process);

    // Add to the general blocked queue
    enqueue(&general_blocked_queue, process);

    return 0;
}

PCB* sem_signal(Mutex* mutex, int pid) {
    if(mutex == NULL) return NULL;

    // Safety check — only owner can release
    if(mutex->owner_pid != pid) {
        printf("[Mutex] ERROR: Process %d tried to release '%s' but doesn't own it\n", pid, mutex->resource_name);
        return NULL;
    }

    printf("[Mutex] Process %d released '%s'\n", pid, mutex->resource_name);

    // Nobody waiting — just unlock
    if(is_empty(&mutex->blocked_queue)) {
        mutex->is_locked = false;
        mutex->owner_pid = -1;
        return NULL;
    }

    // Wake up next waiting process
    PCB* nextProcess = dequeue(&mutex->blocked_queue);
    mutex->owner_pid = nextProcess->pid;
    nextProcess->state = READY;

    printf("[Mutex] Process %d unblocked and acquired '%s'\n", nextProcess->pid, mutex->resource_name);
    return nextProcess;
}

void print_mutex_state(Mutex* mutex) {
    if(mutex == NULL) return;
    printf("[Mutex '%s'] Locked: %d | Owner PID: %d\n",
        mutex->resource_name,
        mutex->is_locked,
        mutex->owner_pid);
    print_queue(&mutex->blocked_queue, "  Waiting");
}
