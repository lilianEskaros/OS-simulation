#include "../include/system_calls.h"
#include "../include/memory.h"
#include "../include/mutex.h"
#include <stdio.h>

void perform_system_call(const char* type, const char* arg1, const char* arg2) {
    if (strcmp(type, "print") == 0) {
        // Logic to print data on the screen [cite: 22]
    } 
    else if (strcmp(type, "readFile") == 0) {
        // Logic to read data of any file from the disk [cite: 20]
    }
    else if (strcmp(type, "writeFile") == 0) {
        // Logic to write text output to a file [cite: 21]
    }
    // ... handle take input, read memory, write memory [cite: 26-28]
}