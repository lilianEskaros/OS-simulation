#include "../include/scheduler.h"
#include "../include/queue.h"
#include "../include/interpreter.h"
#include "../include/os_core.h"
#include "../include/memory.h" // ADDED: needed for swap_from_disk

PCB* curr_process = NULL;

void update_memory_view(PCB* p) {
    if (p->mem_start == -1) return; 
    
    sprintf(memory[p->mem_start + 1].value, "%d", p->pc);
    
    const char* state_str;
    switch(p->state) {
        case READY: state_str = "READY"; break;
        case RUNNING: state_str = "RUNNING"; break;
        case BLOCKED: state_str = "BLOCKED"; break;
        case FINISHED: state_str = "FINISHED"; break;
        default: state_str = "UNKNOWN"; break;
    }
    strcpy(memory[p->mem_start + 2].value, state_str);
    sprintf(memory[p->mem_start + 6].value, "%d", p->waiting_time);
    sprintf(memory[p->mem_start + 7].value, "%d", p->priorityLevel);
}

void schedule_RR() {
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

        printf("Selected Process: P%d\n", curr_process->pid);
        update_memory_view(curr_process);
        print_queue(ready_queue, "Ready Queue");
    }

    // 2. Execute
    int can_continue = execute_instruction(curr_process);
    
    // TRACK QUANTUM IN THE PCB
    curr_process->slice_used++; 

    update_memory_view(curr_process);
    update_waiting_times(ready_queue);

    // 3. Preempt if: Time slice finished OR process finished/blocked
    if (can_continue == 0 || curr_process->state == FINISHED || curr_process->state == BLOCKED) {
        // If it blocked, it's already handled by semWait/Interpreter
        curr_process->slice_used = 0; // Reset so it gets a fresh slice next time
        curr_process = NULL; 
    }
    else if (curr_process->slice_used >= INSTRUCTIONS_PER_SLICE) { 
        // Process is still RUNNING but time is up
        curr_process->state = READY;
        curr_process->slice_used = 0; // Reset before sending back to the queue
        
        update_memory_view(curr_process);
        enqueue(ready_queue, curr_process);
        curr_process = NULL;
    }
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
                update_memory_view(curr_process);
                printf("Selected P%d from Queue %d\n", curr_process->pid, i);
                break; 
            }
        }
        if (curr_process == NULL) return; // All queues empty
    }

    // 2. Execute 1 instruction
    int can_continue = execute_instruction(curr_process);
    
    // TRACK QUANTUM IN THE PCB
    curr_process->slice_used++; 
    
    update_memory_view(curr_process);
    update_mlfq_waiting_times(my_queues);

    // 3. Logic for Quantum and Demotion
    int current_level = curr_process->priorityLevel;
    int max_quantum = (1 << current_level); // This calculates 2^i // bitwise left shift

    if (can_continue == 0 || curr_process->state == FINISHED || curr_process->state == BLOCKED) {
        curr_process->slice_used = 0; // Reset quantum on block/finish
        curr_process = NULL;
    }
    else if (curr_process->slice_used >= max_quantum) {
        // Quantum exhausted! 
        curr_process->state = READY;
        curr_process->slice_used = 0; // Reset quantum before demotion
        
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
}