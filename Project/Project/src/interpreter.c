#include "../include/interpreter.h"

void parse_and_load_program(const char* filename, int arrival_time) {
    PCB pcb;

    pcb.pid = generatePID();
    pcb.state = READY;
    pcb.arrival_time = arrival_time;
    pcb.waiting_time = 0;

    // Step 1: allocate memory
    if (!allocate_memory(&pcb, filename)) {
        // swapping later
        return;
    }

    int index = pcb.mem_start;

    sprintf(memory[index].name, "P%d_pid", pcb.pid);
    sprintf(memory[index].value, "%d", pcb.pid);
    index++;

    sprintf(memory[index].name, "P%d_pc", pcb.pid);
    sprintf(memory[index].value, "%d", pcb.mem_start + 3);
    index++;

    sprintf(memory[index].name, "P%d_state", pcb.pid);
    sprintf(memory[index].value, "READY");
    index++;


    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file\n");
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

    pcb.pc = pcb.mem_start + 3;  
    pcb.burst_time = instruction_count;

    enqueue(ready_queue, pcb);
}

void execute_instruction(PCB* process) {
    char line[100];
    strcpy(line, memory[process->pc].value);

    if (strlen(line) == 0) {
        process->pc++;
        return;
    }

    char* command = strtok(line, " ");

    if (strcmp(command, "semWait") == 0) {
        char* resource = strtok(NULL, " ");
        semWait(resource, process);
    }

    else if (strcmp(command, "semSignal") == 0) {
        char* resource = strtok(NULL, " ");
        semSignal(resource);
    }

    else if (strcmp(command, "assign") == 0) {
        char* var = strtok(NULL, " ");
        char* value = strtok(NULL, " ");

        if (strcmp(value, "input") == 0) {
            char input[50];
            printf("Please enter a value: ");
            scanf("%s", input);
            set_variable(process, var, input);
        }

        else if (strcmp(value, "readFile") == 0) {
            char* filenameVar = strtok(NULL, " ");
            char* filename = get_variable(process, filenameVar);
            char* fileContent = readFile(filename);

            set_variable(process, var, fileContent);
        }

        else {
            set_variable(process, var, value);
        }
    }

    else if (strcmp(command, "print") == 0) {
        char* var = strtok(NULL, " ");
        printf("%s\n", get_variable(process, var));
    }

    else if (strcmp(command, "printFromTo") == 0) {
        char* x = strtok(NULL, " ");
        char* y = strtok(NULL, " ");

        int start = atoi(get_variable(process, x));
        int end = atoi(get_variable(process, y));

        for (int i = start; i <= end; i++) {
            printf("%d\n", i);
        }
    }

    else if (strcmp(command, "writeFile") == 0) {
        char* fileVar = strtok(NULL, " ");
        char* dataVar = strtok(NULL, " ");

        char* filename = get_variable(process, fileVar);
        char* data = get_variable(process, dataVar);

        writeFile(filename, data);
    }

    process->pc++;
}
