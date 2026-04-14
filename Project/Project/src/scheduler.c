#include "../include/scheduler.h"
#include "../include/queue.h"
#include "../include/interpreter.h"


PCB* curr_process = NULL;
int time_slice_c = 0;

void schedule_RR() {
    Queue* ready_queue = get_ready_queue();
    Queue* blocked_queue = get_blocked_queue();
    // 1. If no process is running, pick one
    if (curr_process == NULL) {
        if (is_empty(ready_queue)) return;
        
        curr_process = dequeue(ready_queue);
        curr_process->state = RUNNING;
        time_slice_c = 0;
        printf("Selected Process: P%d\n", curr_process->processID);
        print_queue(ready_queue, "Ready Queue");
    }

    // 2. Execute
    execute_next_instruction(curr_process);
    time_slice_c++;

    // 3. Preempt if: Time slice finished OR process finished/blocked
    if (curr_process->state == FINISHED || curr_process->state == BLOCKED) {
        curr_process = NULL; 
    } 
    else if (time_slice_c >= 2) { 
        // Process is still RUNNING but time is up
        curr_process->state = READY;
        enqueue(ready_queue, curr_process);
        curr_process = NULL;
    }
}

void schedule_HRRN() {
   if (curr_process == NULL) {
        if (is_empty(ready_queue)) return;

        // Non-preemptive selection [cite: 100, 102]
        curr_process = find_and_remove_best_hrrn(ready_queue);
        curr_process->state = RUNNING;
        
        printf("Selected Process (HRRN): P%d\n", curr_process->processID); 
        print_queue(ready_queue, "Ready Queue"); 
    }
    execute_next_instruction(curr_process);

    // Only reset when finished or blocked because it's non-preemptive 
    if (curr_process->state == FINISHED || curr_process->state == BLOCKED) {
        curr_process = NULL;
    }
}

void schedule_MLFQ() {
    Queue** my_queues = get_mlfq_queues();
    // 1. Pick a process if none is running
    if (curr_process == NULL) {
        for (int i = 0; i < 4; i++) {
            if (!is_empty(my_queues[i])) {
                curr_process = dequeue(my_queues[i]);
                curr_process->state = RUNNING;
                time_slice_c = 0; // Reset counter for new slice
                
                printf("Selected P%d from Queue %d\n", curr_process->processID, i);
                break; 
            }
        }
        if (curr_process == NULL) return; // All queues empty
    }

    // 2. Execute 1 instruction
    execute_next_instruction(curr_process);
    time_slice_c++;

    // 3. Logic for Quantum and Demotion
    int current_level = curr_process->priorityLevel;
    int max_quantum = (1 << current_level); // This calculates 2^i // bitwise left shift

    if (curr_process->state == FINISHED || curr_process->state == BLOCKED) {
        curr_process = NULL;
    } 
    else if (time_slice_c >= max_quantum) {
        // Quantum exhausted! 
        curr_process->state = READY;
        
        // Demote if not already at the bottom (Queue 3)
        if (current_level < 3) {
            curr_process->priorityLevel++;
            printf("Demoting P%d to Queue %d\n", curr_process->processID, curr_process->priorityLevel);
        }
        
        // Add to the back of the appropriate queue
        enqueue(my_queues[curr_process->priorityLevel], curr_process);
        curr_process = NULL;
    }
}
