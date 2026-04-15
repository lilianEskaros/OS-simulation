#ifndef MEMORY_H
#define MEMORY_H

#include "os_core.h"

void initialize_memory();
bool allocate_memory(PCB* process, const char* filename); 
void deallocate_memory(PCB* process);
void swap_to_disk(PCB* process);                          
void swap_from_disk(PCB* process);                       
void print_memory_state();                                

#endif