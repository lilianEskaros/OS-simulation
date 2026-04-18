/*#include "../include/scheduler.h"
#include "../include/queue.h"
#include "../include/interpreter.h"
#include "../include/os_core.h"
#include "../include/memory.h" // ADDED: needed for swap_from_disk


PCB* curr_process = NULL;
int time_slice_c = 0;

void update_memory_view(PCB* p) {
    // Word 1: PC (stored at mem_start + 1)
    sprintf(memory[p->mem_start + 1].value, "%d", p->pc);
    
    // Word 2: State (stored at mem_start + 2)
    const char* state_str;
    switch(p->state) {
        case READY: state_str = "READY"; break;
        case RUNNING: state_str = "RUNNING"; break;
        case BLOCKED: state_str = "BLOCKED"; break;
        case FINISHED: state_str = "FINISHED"; break;
        default: state_str = "UNKNOWN"; break;
    }
    strcpy(memory[p->mem_start + 2].value, state_str);
}

int schedule_RR() {
    Queue* ready_queue = get_ready_queue();

    if (curr_process == NULL) {
        if (is_empty(ready_queue)) return;
        
        curr_process = dequeue(ready_queue);

        // --- SWAP CHECK ADDED HERE ---
        if (curr_process->mem_start == -1) {
            swap_from_disk(curr_process);
        }
        // -----------------------------

        curr_process->state = RUNNING;
        time_slice_c = 0;

        printf("Selected Process: P%d\n", curr_process->pid);
        update_memory_view(curr_process);
        print_queue(ready_queue, "Ready Queue");
    }

    // 2. Execute
    int status = execute_instruction(curr_process);

    if (status == -1) { // Process blocked
        curr_process = NULL;
        return 0; // Zero cycles consumed
    }
    time_slice_c++;

    update_memory_view(curr_process);
    update_waiting_times(ready_queue);

    // 3. Preempt if: Time slice finished OR process finished/blocked
    // Preemption Logic
    if (status == 0 || curr_process->state == FINISHED ) {
        // If it blocked, it's already handled by semWait/Interpreter
        curr_process = NULL; 
    }
    else if (time_slice_c >= 2) { 
        // Process is still RUNNING but time is up
        curr_process->state = READY;
        update_memory_view(curr_process);
        enqueue(ready_queue, curr_process);
        curr_process = NULL;
    }
    return 1;
}

void schedule_HRRN() {
    Queue* ready_queue = get_ready_queue();
   if (curr_process == NULL) {
        if (is_empty(ready_queue)) return;

        // Non-preemptive selection
        curr_process = find_and_remove_best_hrrn(ready_queue);

        // --- SWAP CHECK ADDED HERE ---
        if (curr_process->mem_start == -1) {
            swap_from_disk(curr_process);
        }
        // -----------------------------

        curr_process->state = RUNNING;
        update_memory_view(curr_process);

        printf("Selected Process (HRRN): P%d\n", curr_process->pid); 
        print_queue(ready_queue, "Ready Queue"); 
    }

    int can_continue = execute_instruction(curr_process);
    update_memory_view(curr_process);
    update_waiting_times(ready_queue);

    if (can_continue == 0 || curr_process->state == FINISHED || curr_process->state == BLOCKED) {
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

                // --- SWAP CHECK ADDED HERE ---
                if (curr_process->mem_start == -1) {
                    swap_from_disk(curr_process);
                }
                // -----------------------------

                curr_process->state = RUNNING;
                time_slice_c = 0; // Reset counter for new slice
                update_memory_view(curr_process);
                printf("Selected P%d from Queue %d\n", curr_process->pid, i);
                break; 
            }
        }
        if (curr_process == NULL) return; // All queues empty
    }

    // 2. Execute 1 instruction
    int can_continue = execute_instruction(curr_process);
    time_slice_c++;
    update_memory_view(curr_process);
    update_mlfq_waiting_times(my_queues);

    // 3. Logic for Quantum and Demotion
    int current_level = curr_process->priorityLevel;
    int max_quantum = (1 << current_level); // This calculates 2^i // bitwise left shift

    if (can_continue == 0 || curr_process->state == FINISHED || curr_process->state == BLOCKED) {
        curr_process = NULL;
    }
    else if (time_slice_c >= max_quantum) {
        // Quantum exhausted! 
        curr_process->state = READY;
        
        // Demote if not already at the bottom (Queue 3)
        if (current_level < 3) {
            curr_process->priorityLevel++;
            printf("Demoting P%d to Queue %d\n", curr_process->pid , curr_process->priorityLevel);
        }
        
        update_memory_view(curr_process);
        // Add to the back of the appropriate queue
        enqueue(my_queues[curr_process->priorityLevel], curr_process);
        curr_process = NULL;
    }
}

void update_waiting_times(Queue* ready_q) {
    if (ready_q == NULL || ready_q->head == NULL) return;

    Node* curr = ready_q->head;
    while (curr != NULL) {
        // Every process in the queue just waited for 1 clock cycle
        curr->process->waiting_time++;
        curr = curr->next;
    }
}

void update_mlfq_waiting_times(Queue** queues) {
    for (int i = 0; i < 4; i++) {
        update_waiting_times(queues[i]);
    }
}*/
#include "../include/scheduler.h"
#include "../include/queue.h"
#include "../include/interpreter.h"
#include "../include/os_core.h"
#include "../include/memory.h"

PCB* curr_process = NULL;
int time_slice_c = 0;

void update_memory_view(PCB* p) {
    if (p->mem_start == -1) return; // Cannot update view if swapped out
    
    // Word 1: PC
    sprintf(memory[p->mem_start + 1].value, "%d", p->pc);
    
    // Word 2: State
    const char* state_str;
    switch(p->state) {
        case READY: state_str = "READY"; break;
        case RUNNING: state_str = "RUNNING"; break;
        case BLOCKED: state_str = "BLOCKED"; break;
        case FINISHED: state_str = "FINISHED"; break;
        default: state_str = "UNKNOWN"; break;
    }
    strcpy(memory[p->mem_start + 2].value, state_str);
}

// FIX: Functions now return int. 1 = Cycle passed, 0 = No cycle.
int schedule_RR() {
    Queue* ready_queue = get_ready_queue();

    if (curr_process == NULL) {
        if (is_empty(ready_queue)) return 0;
        
        curr_process = dequeue(ready_queue);

        if (curr_process->mem_start == -1) {
            swap_from_disk(curr_process);
        }

        curr_process->state = RUNNING;
        time_slice_c = 0;

        printf("Selected Process: P%d\n", curr_process->pid);
        update_memory_view(curr_process);
    }

    // Execute instruction and catch the return status
    // status 1 = success, 0 = finished, -1 = blocked
    int status = execute_instruction(curr_process);

    if (status == -1) {
        // Process blocked on semWait - this attempt is "free"
        curr_process = NULL; 
        return 0; 
    }

    // If we are here, a real instruction was executed.
    time_slice_c++;
    update_memory_view(curr_process);
    update_waiting_times(ready_queue);

    if (status == 0 || curr_process->state == FINISHED) {
        curr_process = NULL; 
    }
    else if (time_slice_c >= 2) { 
        curr_process->state = READY;
        update_memory_view(curr_process);
        enqueue(ready_queue, curr_process);
        curr_process = NULL;
    }
    return 1; // Work done, increment clock
}

int schedule_HRRN() {
    Queue* ready_queue = get_ready_queue();
    if (curr_process == NULL) {
        if (is_empty(ready_queue)) return 0;

        curr_process = find_and_remove_best_hrrn(ready_queue);

        if (curr_process->mem_start == -1) {
            swap_from_disk(curr_process);
        }

        curr_process->state = RUNNING;
        update_memory_view(curr_process);

        printf("Selected Process (HRRN): P%d\n", curr_process->pid); 
    }

    int status = execute_instruction(curr_process);

    if (status == -1) {
        curr_process = NULL;
        return 0; 
    }

    update_memory_view(curr_process);
    update_waiting_times(ready_queue);

    if (status == 0 || curr_process->state == FINISHED) {
        curr_process = NULL;
    }
    return 1;
}

int schedule_MLFQ() {
    Queue** my_queues = get_mlfq_queues();
    
    if (curr_process == NULL) {
        for (int i = 0; i < 4; i++) {
            if (!is_empty(my_queues[i])) {
                curr_process = dequeue(my_queues[i]);

                if (curr_process->mem_start == -1) {
                    swap_from_disk(curr_process);
                }

                curr_process->state = RUNNING;
                time_slice_c = 0;
                update_memory_view(curr_process);
                printf("Selected P%d from Queue %d\n", curr_process->pid, i);
                break; 
            }
        }
        if (curr_process == NULL) return 0;
    }

    int status = execute_instruction(curr_process);

    if (status == -1) {
        curr_process = NULL;
        return 0;
    }

    time_slice_c++;
    update_memory_view(curr_process);
    update_mlfq_waiting_times(my_queues);

    int current_level = curr_process->priorityLevel;
    int max_quantum = (1 << current_level); 

    if (status == 0 || curr_process->state == FINISHED) {
        curr_process = NULL;
    }
    else if (time_slice_c >= max_quantum) {
        curr_process->state = READY;
        
        if (current_level < 3) {
            curr_process->priorityLevel++;
            printf("Demoting P%d to Queue %d\n", curr_process->pid , curr_process->priorityLevel);
        }
        
        update_memory_view(curr_process);
        enqueue(my_queues[curr_process->priorityLevel], curr_process);
        curr_process = NULL;
    }
    return 1;
}

void update_waiting_times(Queue* ready_q) {
    if (ready_q == NULL || ready_q->head == NULL) return;

    Node* curr = ready_q->head;
    while (curr != NULL) {
        curr->process->waiting_time++;
        curr = curr->next;
    }
}

void update_mlfq_waiting_times(Queue** queues) {
    for (int i = 0; i < 4; i++) {
        update_waiting_times(queues[i]);
    }
}
