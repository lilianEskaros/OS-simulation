#include "../include/scheduler.h"
#include "../include/queue.h"
#include "../include/interpreter.h"
#include "../include/os_core.h"
#include "../include/memory.h" 

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

        
        if (curr_process->mem_start == -1) {
            printf("[Swapper] P%d is on the disk! Finding a victim to make room...\n", curr_process->pid);
            
            
            PCB* victim = NULL;
            Node* temp = ready_queue->head;
            
            while (temp != NULL) {
                if (temp->process->mem_start != -1) {
                    victim = temp->process;
                    break; 
                }
                temp = temp->next;
            }

            
            if (victim == NULL) {
                Queue* blocked_queue = get_blocked_queue();
                temp = blocked_queue->head;
                while (temp != NULL) {
                    if (temp->process->mem_start != -1) {
                        victim = temp->process;
                        break;
                    }
                    temp = temp->next;
                }
            }

            
            if (victim != NULL) {
                swap_to_disk(victim);
            } else {
                printf("[Swapper] ERROR: No victim found to swap out!\n");
            }

            
            swap_from_disk(curr_process);
        }
        // ---------------------------------

        curr_process->state = RUNNING;

        printf("Selected Process: P%d\n", curr_process->pid);
        update_memory_view(curr_process);
        print_queue(ready_queue, "Ready Queue");
    }

    
    int can_continue = execute_instruction(curr_process);
    
    
    curr_process->slice_used++; 

    update_memory_view(curr_process);
    update_waiting_times(ready_queue);

    
    if (can_continue == 0 || curr_process->state == FINISHED || curr_process->state == BLOCKED) {
        
        curr_process->slice_used = 0; 
        curr_process = NULL; 
    }
    else if (curr_process->slice_used >= INSTRUCTIONS_PER_SLICE) { 
        
        curr_process->state = READY;
        curr_process->slice_used = 0; 
        
        update_memory_view(curr_process);
        enqueue(ready_queue, curr_process);
        curr_process = NULL;
    }
}

void schedule_HRRN() {
    Queue* ready_queue = get_ready_queue();
    if (curr_process == NULL) {
        if (is_empty(ready_queue)) return;

       
        curr_process = find_and_remove_best_hrrn(ready_queue);

        
        if (curr_process->mem_start == -1) {
            printf("[Swapper] P%d is on the disk! Finding a victim to make room...\n", curr_process->pid);
            
           
            PCB* victim = NULL;
            Node* temp = ready_queue->head;
            
            while (temp != NULL) {
                if (temp->process->mem_start != -1) {
                    victim = temp->process;
                    break; 
                }
                temp = temp->next;
            }

            
            if (victim == NULL) {
                Queue* blocked_queue = get_blocked_queue();
                temp = blocked_queue->head;
                while (temp != NULL) {
                    if (temp->process->mem_start != -1) {
                        victim = temp->process;
                        break;
                    }
                    temp = temp->next;
                }
            }

            
            if (victim != NULL) {
                swap_to_disk(victim);
            } else {
                printf("[Swapper] ERROR: No victim found to swap out!\n");
            }

            
            swap_from_disk(curr_process);
        }
        // ---------------------------------


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
    
    
    if (curr_process == NULL) {
        for (int i = 0; i < 4; i++) {
            if (!is_empty(my_queues[i])) {
                curr_process = dequeue(my_queues[i]);

            
                if (curr_process->mem_start == -1) {
                    printf("[Swapper] P%d is on the disk! Finding a victim to make room...\n", curr_process->pid);
                    
                    PCB* victim = NULL;
                    
                    
                    for (int q_idx = 0; q_idx < 4; q_idx++) {
                        Node* temp = my_queues[q_idx]->head;
                        while (temp != NULL) {
                            if (temp->process->mem_start != -1 && temp->process->pid != curr_process->pid) {
                                victim = temp->process;
                                break;
                            }
                            temp = temp->next;
                        }
                        if (victim != NULL) break; 
                    }

                    
                    if (victim == NULL) {
                        Queue* blocked_queue = get_blocked_queue();
                        Node* temp = blocked_queue->head;
                        while (temp != NULL) {
                            if (temp->process->mem_start != -1) {
                                victim = temp->process;
                                break;
                            }
                            temp = temp->next;
                        }
                    }

                    
                    if (victim != NULL) {
                        swap_to_disk(victim);
                    } else {
                        printf("[Swapper] ERROR: No victim found to swap out!\n");
                    }

                    
                    swap_from_disk(curr_process);
                }
                // ---------------------------------

                curr_process->state = RUNNING;
                update_memory_view(curr_process);
                printf("Selected P%d from Queue %d\n", curr_process->pid, i);
                break; 
            }
        }
        if (curr_process == NULL) return; 
    }

    
    int can_continue = execute_instruction(curr_process);
    
    
    curr_process->slice_used++; 
    
    update_memory_view(curr_process);
    update_mlfq_waiting_times(my_queues);

    
    int current_level = curr_process->priorityLevel;
    int max_quantum = (1 << current_level); 

    if (can_continue == 0 || curr_process->state == FINISHED || curr_process->state == BLOCKED) {
        curr_process->slice_used = 0; 
        curr_process = NULL;
    }
    else if (curr_process->slice_used >= max_quantum) {
        
        curr_process->state = READY;
        curr_process->slice_used = 0; 
        
        
        if (current_level < 3) {
            curr_process->priorityLevel++;
            printf("Demoting P%d to Queue %d\n", curr_process->pid , curr_process->priorityLevel);
        }
        
        update_memory_view(curr_process);
        
        enqueue(my_queues[curr_process->priorityLevel], curr_process);
        curr_process = NULL;
    }
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