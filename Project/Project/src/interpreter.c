#include "../include/interpreter.h"
#include "../include/memory.h"       // For allocate_memory
#include "../include/queue.h"        // For enqueue
#include "../include/mutex.h"        // For semWait / semSignal
#include "../include/system_calls.h" // For syscall_take_input, etc.
#include "../include/os_core.h"

// Declare the external PID generator from main.c
extern int generatePID(); 

void parse_and_load_program(const char* filename, int arrival_time) {
    PCB* pcb = (PCB*)malloc(sizeof(PCB));
    if (pcb == NULL) {
        printf("Error: Memory allocation failed for new PCB.\n");
        return;
    }

    pcb->pid = generatePID();
    pcb->state = READY;
    pcb->arrival_time = arrival_time;
    pcb->waiting_time = 0;

    // --- SWAPPING LOGIC ADDED HERE ---
    if (!allocate_memory(pcb, filename)) {
        printf("[Swapper] Memory full! Attempting to swap out a process to disk...\n");
        
        PCB* victim = NULL;
        int queue_size = ready_queue->size;
        
        // Cycle through the queue to find a process that is ACTUALLY in RAM
        for (int i = 0; i < queue_size; i++) {
            PCB* temp = dequeue(ready_queue);
            
            // If it is in RAM (mem_start is not -1) and we haven't found a victim yet
            if (temp->mem_start != -1 && victim == NULL) {
                victim = temp; 
            } else {
                // Otherwise, put it right back in the queue
                enqueue(ready_queue, temp);
            }
        }
        
        if (victim != NULL) {
            swap_to_disk(victim);
            
            // Put the victim back in the queue (it is now safely stored on disk)
            enqueue(ready_queue, victim);
            
            // Try allocating the new process again now that RAM is free!
            if (!allocate_memory(pcb, filename)) {
                printf("[Swapper] CRITICAL: Still not enough space for P%d after swap.\n", pcb->pid);
                free(pcb);
                return;
            }
        } else {
            printf("[Swapper] CRITICAL: Memory full and no victim processes in RAM to swap.\n");
            free(pcb);
            return;
        }
    }
    // ---------------------------------

    int index = pcb->mem_start;

    sprintf(memory[index].name, "P%d_pid", pcb->pid);
    sprintf(memory[index].value, "%d", pcb->pid);
    index++;

    sprintf(memory[index].name, "P%d_pc", pcb->pid);
    sprintf(memory[index].value, "%d", pcb->mem_start + 3);
    index++;

    sprintf(memory[index].name, "P%d_state", pcb->pid);
    sprintf(memory[index].value, "READY");
    index++;

    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file\n");
        free(pcb);
        return;
    }

    char line[100];
    int instruction_count = 0;

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        if (strlen(line) == 0) continue;

        strcpy(memory[index].name, "instruction");
        strcpy(memory[index].value, line);

        index++;
        instruction_count++;
    }

    fclose(file);

    for (int i = 0; i < VARIABLES_PER_PROCESS; i++) {
        strcpy(memory[index].name, "var");
        strcpy(memory[index].value, "Empty");
        index++;
    }

    pcb->instruction_end= (pcb->mem_start + 3) + (instruction_count - 1);
    pcb->pc = pcb->mem_start + 3;  
    pcb->burst_time = instruction_count;

    enqueue(ready_queue, pcb); 
}

int execute_instruction(PCB* process) {
    // CHECK FIRST: Are we out of code?
    if (process->pc > process->instruction_end) {
        process->state = FINISHED;
        printf("Process %d finished execution.\n", process->pid);
        deallocate_memory(process); // ADDED: Free RAM for the Swapper
        return 0;
    }

    char line[100];
    printf("Executing P%d: %s\n", process->pid, memory[process->pc].value);

    strcpy(line, memory[process->pc].value);
    
    if (strlen(line) == 0) {
        process->pc++;
        return 1;
    }

    char* command = strtok(line, " ");
    
   if (strcmp(command, "semWait") == 0) {
        char* resource = strtok(NULL, " ");
        if (semWait(resource, process) == 0) {
            return -1; // Special code: Blocked, do not increment clock
        }
    }

    else if (strcmp(command, "semSignal") == 0) {
        char* resource = strtok(NULL, " ");
        semSignal(resource, process->pid); // Added process->pid
    }

    else if (strcmp(command, "assign") == 0) {
        char* var = strtok(NULL, " ");
        char* value = strtok(NULL, " ");

        if (strcmp(value, "input") == 0) {
            char* input = syscall_take_input(process->pid);

            syscall_write_memory(
                process->pid, var, input,
                memory, process->mem_start, process->mem_end
            );
        }

        else if (strcmp(value, "readFile") == 0) {
            char* filenameVar = strtok(NULL, " ");

            char* filename = syscall_read_memory(
                process->pid, filenameVar,
                memory, process->mem_start, process->mem_end
            );

            char* fileContent = syscall_read_file(process->pid, filename);

            syscall_write_memory(
                process->pid, var, fileContent,
                memory, process->mem_start, process->mem_end
            );
        }

        else {
            syscall_write_memory(
                process->pid, var, value,
                memory, process->mem_start, process->mem_end
            );
        }
    }

    else if (strcmp(command, "print") == 0) {
        char* var = strtok(NULL, " ");

        syscall_print(
            process->pid, var,
            memory, process->mem_start, process->mem_end
        );
    }

    else if (strcmp(command, "printFromTo") == 0) {
        char* x = strtok(NULL, " ");
        char* y = strtok(NULL, " ");

        char* startStr = syscall_read_memory(
            process->pid, x,
            memory, process->mem_start, process->mem_end
        );

        char* endStr = syscall_read_memory(
            process->pid, y,
            memory, process->mem_start, process->mem_end
        );

        int start = atoi(startStr);
        int end = atoi(endStr);

        for (int i = start; i <= end; i++) {
            printf("%d\n", i); // acceptable unless required syscall
        }
    }

    else if (strcmp(command, "writeFile") == 0) {
        char* fileVar = strtok(NULL, " ");
        char* dataVar = strtok(NULL, " ");

        char* filename = syscall_read_memory(
            process->pid, fileVar,
            memory, process->mem_start, process->mem_end
        );

        char* data = syscall_read_memory(
            process->pid, dataVar,
            memory, process->mem_start, process->mem_end
        );

        syscall_write_file(process->pid, filename, data);
    }

    process->pc++;

    // FINAL CHECK: Did that last increment push us past the end?
    if (process->pc > process->instruction_end) {
        process->state = FINISHED;
        printf("Process %d finished execution.\n", process->pid);
        deallocate_memory(process); // ADDED: Free RAM for the Swapper
        return 0; 
    }
    return 1; // successful can execute more instructions if time slice allows 
}