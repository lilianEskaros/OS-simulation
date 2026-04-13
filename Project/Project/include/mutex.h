#ifndef MUTEX_H
#define MUTEX_H

#include "os_core.h"

void semWait(Mutex* m, PCB* process);   // [cite: 75]
void semSignal(Mutex* m, PCB* process); // [cite: 77]

#endif