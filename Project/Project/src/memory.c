
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../include/memory.h"
#include "../include/os_core.h"
#include "../include/queue.h"
#include "../include/scheduler.h"

extern PCB* curr_process; 
MemoryWord memory[MEMORY_SIZE]; 

void initialize_memory() {
    for (int i = 0; i < MEMORY_SIZE; i++) {
        strcpy(memory[i].name, "Empty");
        strcpy(memory[i].value, "Empty");
    }
}

bool allocate_memory(PCB* process, const char* filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not open file %s\n", filename);
        return false;
    }

    int lines_of_codes = 0;
    char buffer[256];
    
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0;
        buffer[strcspn(buffer, "\r")] = 0;
        if (strlen(buffer) == 0) continue; 
        lines_of_codes++;
    }
    fclose(file);

    
    int total_words_needed = VARIABLES_PER_PROCESS + 8 + lines_of_codes;
    int start_index = -1;
    int consecutive_empty = 0;

    for (int i = 0; i < MEMORY_SIZE; i++) {
        if (strcmp(memory[i].name, "Empty") == 0) {
            if (consecutive_empty == 0) start_index = i;
            consecutive_empty++;
            if (consecutive_empty == total_words_needed) break; 
        } else {
            consecutive_empty = 0; 
        }
    } 

    if (consecutive_empty < total_words_needed) {
        return false;
    }

    process->mem_start = start_index;
    process->mem_end = start_index + total_words_needed - 1;
    process->pc = start_index + 8; // Code starts after the 8-word PCB
    process->instruction_end = process->pc + lines_of_codes - 1;
    process->burst_time = lines_of_codes;

    printf("Allocated memory for Process %d from index %d to %d.\n", process->pid, process->mem_start, process->mem_end);

    int current_slot = start_index;
    
    
    strcpy(memory[current_slot].name, "PCB_PID");
    sprintf(memory[current_slot].value, "%d", process->pid);
    current_slot++;

    strcpy(memory[current_slot].name, "PCB_PC");
    sprintf(memory[current_slot].value, "%d", process->pc);
    current_slot++;

    strcpy(memory[current_slot].name, "PCB_State");
    strcpy(memory[current_slot].value, "READY");
    current_slot++;

    strcpy(memory[current_slot].name, "PCB_MemStart");
    sprintf(memory[current_slot].value, "%d", process->mem_start);
    current_slot++;

    strcpy(memory[current_slot].name, "PCB_MemEnd");
    sprintf(memory[current_slot].value, "%d", process->mem_end);
    current_slot++;

    strcpy(memory[current_slot].name, "PCB_Burst");
    sprintf(memory[current_slot].value, "%d", process->burst_time);
    current_slot++;

    strcpy(memory[current_slot].name, "PCB_Wait");
    sprintf(memory[current_slot].value, "%d", process->waiting_time);
    current_slot++;

    strcpy(memory[current_slot].name, "PCB_Priority");
    sprintf(memory[current_slot].value, "%d", process->priorityLevel);
    current_slot++;

    file = fopen(filename, "r"); 
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0; 
        buffer[strcspn(buffer, "\r")] = 0; 
        if (strlen(buffer) == 0) continue; 

        strcpy(memory[current_slot].name, "Instruction");
        strcpy(memory[current_slot].value, buffer);
        current_slot++;
    }
    fclose(file);

    while (current_slot <= process->mem_end) {
        strcpy(memory[current_slot].name, "Variable");
        strcpy(memory[current_slot].value, "Uninitialized");
        current_slot++;
    }

    return true;
}

void deallocate_memory(PCB* process) {
    if (process->mem_start == -1) return; 
    for(int i = process->mem_start; i <= process->mem_end; i++) {
        strcpy(memory[i].name, "Empty");
        strcpy(memory[i].value, "Empty");
    }
    process->mem_start = -1;
    process->mem_end = -1;
    printf("Process %d terminated. Memory successfully deallocated.\n", process->pid);
}

void swap_to_disk(PCB* process) {
    if (process->mem_start == -1) return;
    
    char disk_filename[50]; 
    sprintf(disk_filename, "process_%d.txt", process->pid);
    FILE* file = fopen(disk_filename, "w");
    
    if (file == NULL) {
        printf("Error: Could not create disk file %s. Swapping aborted, but memory freed to prevent hang.\n", disk_filename);
        for(int i=process->mem_start; i<= process->mem_end; i++){
            strcpy(memory[i].name, "Empty");
            strcpy(memory[i].value, "Empty");
        }
        process->mem_start = -1;
        process->mem_end = -1;
        return;
    }
    
    for (int i = process->mem_start; i <= process->mem_end; i++){
        fprintf(file, "%s=%s\n", memory[i].name, memory[i].value);
    }
    fclose(file);
    
    for(int i=process->mem_start; i<= process->mem_end; i++){
        strcpy(memory[i].name, "Empty");
        strcpy(memory[i].value, "Empty");
    }
    
    process->mem_start = -1;
    process->mem_end = -1;
    printf("Process %d swapped out to disk (%s).\n", process->pid, disk_filename);
}

void swap_from_disk(PCB* process) {
    char disk_filename[50];
    sprintf(disk_filename, "process_%d.txt", process->pid); // Use local directory
    FILE* file = fopen(disk_filename, "r");
    if (file == NULL) {
        printf("Error: Could not find swap file %s.\n", disk_filename);
        return;
    }

    int total_words_needed = 0;
    char buffer[256]; 
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        total_words_needed++;
    }

    int start_index = -1;
    int failsafe = 0;
    
    while (true) {
        failsafe++;
        if (failsafe > 10) {
            printf("[Swapper] Failsafe triggered. Aborting infinite loop.\n");
            return;
        }

        int consecutive_empty = 0;
        start_index = -1;
        
        for (int i = 0; i < MEMORY_SIZE; i++) {
            if (strcmp(memory[i].name, "Empty") == 0) {
                if (consecutive_empty == 0) start_index = i;
                consecutive_empty++;
                if (consecutive_empty == total_words_needed) break;
            } else {
                consecutive_empty = 0;
            }
        }

        if (consecutive_empty >= total_words_needed) break; 

        printf("[Swapper] Memory full! Finding a victim to bring P%d back...\n", process->pid);
        PCB* victim = NULL;
        
        Queue* rq = get_ready_queue();
        int q_size = rq->size;
        for (int i = 0; i < q_size; i++) {
            PCB* temp = dequeue(rq);
            if (temp->mem_start != -1 && victim == NULL) victim = temp; 
            else enqueue(rq, temp);
        }

        if (victim == NULL && current_policy == MLFQ) {
            Queue** mlfq = get_mlfq_queues();
            for (int j = 0; j < 4; j++) {
                int mq_size = mlfq[j]->size;
                for (int i = 0; i < mq_size; i++) {
                    PCB* temp = dequeue(mlfq[j]);
                    if (temp->mem_start != -1 && victim == NULL) victim = temp;
                    else enqueue(mlfq[j], temp);
                }
            }
        }

        if (victim == NULL) {
            Queue* bq = get_blocked_queue();
            int bq_size = bq->size;
            for (int i = 0; i < bq_size; i++) {
                PCB* temp = dequeue(bq);
                if (temp->mem_start != -1 && victim == NULL) victim = temp; 
                else enqueue(bq, temp);
            }
        }

        if (victim == NULL && curr_process != NULL && curr_process->mem_start != -1) {
            victim = curr_process;
            curr_process = NULL; 
            printf("[Swapper] Preempted running process P%d for swapping.\n", victim->pid);
        }

        if (victim != NULL) {
            swap_to_disk(victim);
            if (victim->state == BLOCKED) {
                enqueue(get_blocked_queue(), victim);
            } else {
                victim->state = READY; 
                if (current_policy == MLFQ) {
                    enqueue(get_mlfq_queues()[victim->priorityLevel], victim);
                } else {
                    enqueue(get_ready_queue(), victim); 
                }
            }
        } else {
            printf("[Swapper] CRITICAL: No victim available in RAM to swap out!\n");
            fclose(file);
            return;
        }
    }

    process->mem_start = start_index;
    process->mem_end = start_index + total_words_needed - 1;
    
    rewind(file);
    int current_slot = start_index;
    
    int old_mem_start = -1;
    int old_pc = -1;

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0;
        buffer[strcspn(buffer, "\r")] = 0;
        char* equals_sign = strchr(buffer, '='); 
        if (equals_sign != NULL) {
            *equals_sign = '\0'; 
            strcpy(memory[current_slot].name, buffer);
            strcpy(memory[current_slot].value, equals_sign + 1);

            // Extract old layout variables
            if (strcmp(buffer, "PCB_MemStart") == 0) old_mem_start = atoi(equals_sign + 1);
            if (strcmp(buffer, "PCB_PC") == 0) old_pc = atoi(equals_sign + 1);
        }
        current_slot++;
    }
    
    fclose(file);
    remove(disk_filename); 

    
    int shift_offset = start_index - old_mem_start;
    process->pc = old_pc + shift_offset;
    process->instruction_end = (start_index + 8) + (process->burst_time - 1);

    
    sprintf(memory[start_index + 1].value, "%d", process->pc);
    sprintf(memory[start_index + 3].value, "%d", process->mem_start);
    sprintf(memory[start_index + 4].value, "%d", process->mem_end);
    
    
    sprintf(memory[start_index + 6].value, "%d", process->waiting_time);
    sprintf(memory[start_index + 7].value, "%d", process->priorityLevel);

    printf("Process %d swapped in from disk to memory slots %d to %d.\n", process->pid, process->mem_start, process->mem_end);
}

void print_memory_state() {
    printf("\n--- current Memory State ---\n");
    for(int i=0 ; i<MEMORY_SIZE; i++) {
        if(strcmp(memory[i].name, "Empty") != 0) {
            printf("Memory Slot %d: %s = %s\n", i, memory[i].name, memory[i].value);
        }
    }
    printf("----------------------------\n");
}