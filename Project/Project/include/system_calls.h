#ifndef SYSTEM_CALLS_H
#define SYSTEM_CALLS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/memory.h"



void syscall_print(int processId, char* variableName, MemoryWord* memory, int lowerBound, int upperBound);
char* syscall_take_input(int processId);
char* syscall_read_file(int processId, char* filename);
void syscall_write_file(int processId, char* filename, char* data);
char* syscall_read_memory(int processId, char* variableName, MemoryWord* memory, int lowerBound, int upperBound);
void syscall_write_memory(int processId, char* variableName, char* value, MemoryWord* memory, int lowerBound, int upperBound);

#endif
