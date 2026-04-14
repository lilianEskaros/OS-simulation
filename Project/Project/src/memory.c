#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/memory.h"

MemoryWord memory[40]; 

void initialize_memory() {
    for (int i = 0; i < 40; i++) {
        strcpy(memory[i].name, "Empty");
        strcpy(memory[i].value, "Empty");
    }
}

bool allocate_memory(PCB* process, const char* filename) {
    FILE*file = fopen(filename, "r");
    if (file == NULL){
        printf("Error: Could not open file %s\n", filename);
        return false;
    }
    int lines_of_codes = 0;
    char buffer[100];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        lines_of_codes++;
    }
    fclose(file);
    int total_words_needed = VARIABLES_PER_PROCESS + 3 +lines_of_codes;
    printf("Process %d needs %d memory words.\n" , process->pid, total_words_needed);
    int start_index = -1;
    int consecutive_empty = 0;
    for (int i = 0; i < 40; i++) {
        if (strcmp(memory[i].name , "Empty")== 0) {
            if (consecutive_empty == 0) {
                start_index = i;
            }
            consecutive_empty++;
            if (consecutive_empty == total_words_needed) {
                break;
            }
        } else {
            consecutive_empty = 0;
        }
    }
        if (consecutive_empty < total_words_needed) {
            printf("Not enough memory for Process %d. Needed: %d, Available: %d , swapping needed \n", process->pid, total_words_needed, consecutive_empty);
            return false;
        }
        process->mem_start = start_index;
        process->mem_end = start_index + total_words_needed - 1;
        printf("Allocated memory for Process %d from index %d to %d.\n", process->pid, process->mem_start, process->mem_end);

        int current_slot= start_index;
        process->pc = start_index + 3;
        strcpy(memory[current_slot].name, "PCB_PID");
        sprintf(memory[current_slot].value, "%d", process->pid);
        current_slot++;
        strcpy(memory[current_slot].name, "PCB_PC");
        sprintf(memory[current_slot].value, "%d", process->pc);
        current_slot++;
        strcpy(memory[current_slot].name, "PCB_State");
        strcpy(memory[current_slot].value, "Ready");
        current_slot++;

        file = fopen(filename, "r");
        while (fgets(buffer,sizeof(buffer),file)!=NULL){
            buffer[strcspn(buffer, "\n")] = 0;
            buffer[strcspn(buffer, "\r")] = 0;
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


void swap_to_disk(PCB* process) {
    // Unload process and store data on disk [cite: 42]
}

void swap_from_disk(PCB* process) {
    // Read process data back from disk [cite: 44]
}

void print_memory_state() {
    printf("\n--- current Memory State ---\n");
    for(int i=0 ; i<40; i++) {
        if(strcmp(memory[i].name, "Empty") != 0) {
            printf("Memory Slot %d: %s = %s\n", i, memory[i].name, memory[i].value);
        }
    }
    printf("----------------------------\n");
}