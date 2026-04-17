#ifndef MUTEX_H
#define MUTEX_H

#include "../include/os_core.h"

// Resource name constants — must match exactly what program files use
#define RESOURCE_USER_OUTPUT "userOutput"
#define RESOURCE_USER_INPUT  "userInput"
#define RESOURCE_FILE        "file"

// Function declarations
void initialize_mutexes();
int semWait(char* resourceName, PCB* process);
void semSignal(char* resourceName, int pid);
Mutex* get_mutex(char* resourceName);
void print_mutex_state(Mutex* mutex);

#endif
