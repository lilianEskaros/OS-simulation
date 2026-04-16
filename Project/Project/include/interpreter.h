#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "os_core.h"

void parse_and_load_program(const char* filename, int arrival_time); // [cite: 12, 38]
int execute_instruction(PCB* process);                              // [cite: 126]


#endif