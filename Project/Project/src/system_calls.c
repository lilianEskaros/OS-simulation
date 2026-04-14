#include "../include/system_calls.h"
void syscall_print(int processId, char* variableName, MemoryWord* memory, int lowerBound, int upperBound) {
    char* value = syscall_read_memory(processId, variableName, memory, lowerBound, upperBound);
    if(value == NULL){
        printf("[Process %d] ERROR: variable '%s' not found\n", processId, variableName);
        return;
    }
    printf("[Process %d] OUTPUT: %s\n", processId, value);
}

char* syscall_take_input(int processId) {
    printf("Please enter a value: ");
    fflush(stdout);
    char* input = (char*)malloc(MAX_VALUE_LENGTH * sizeof(char));
    if(input == NULL){
        return NULL;
    }
    fgets(input, MAX_VALUE_LENGTH, stdin);
    int len = strlen(input);
    if(len > 0 && input[len-1] == '\n'){
        input[len-1] = '\0';
    }
    return input;
}

char* syscall_read_file(int processId, char* filename) {
    FILE* file = fopen(filename, "r");
    if(file == NULL){
        printf("[Process %d] ERROR: could not open file '%s'\n", processId, filename);
        return NULL;
    }
    char* content = (char*)malloc(1024 * sizeof(char));
    if(content == NULL){
        fclose(file);
        return NULL;
    }
    content[0] = '\0';
    char line[MAX_VALUE_LENGTH];
    while(fgets(line, sizeof(line), file) != NULL){
        strcat(content, line);
    }
    fclose(file);
    return content;
}

void syscall_write_file(int processId, char* filename, char* data) {
    FILE* file = fopen(filename, "w");
    if(file == NULL){
        printf("[Process %d] ERROR: could not create file '%s'\n", processId, filename);
        return;
    }
    fprintf(file, "%s", data);
    fclose(file);
}

char* syscall_read_memory(int processId, char* variableName, MemoryWord* memory, int lowerBound, int upperBound) {
    for(int i = lowerBound; i <= upperBound; i++){
        if(strcmp(memory[i].name, variableName) == 0){
            return memory[i].value;
        }
    }
    return NULL;
}

void syscall_write_memory(int processId, char* variableName, char* value, MemoryWord* memory, int lowerBound, int upperBound) {
    // First check if variable already exists and update it
    for(int i = lowerBound; i <= upperBound; i++){
        if(strcmp(memory[i].name, variableName) == 0){
            strcpy(memory[i].value, value);
            return;
        }
    }
    // Variable doesn't exist, find an empty slot
    for(int i = lowerBound; i <= upperBound; i++){
        if(memory[i].name[0] == '\0'){
            strcpy(memory[i].name, variableName);
            strcpy(memory[i].value, value);
            return;
        }
    }
    printf("[Process %d] ERROR: no memory space for variable '%s'\n", processId, variableName);
}
