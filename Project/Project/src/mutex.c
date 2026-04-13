#include "../include/mutex.h"
#include "../include/queue.h"

void semWait(Mutex* m, PCB* process) {
    // If locked, block process and add to general & mutex queues [cite: 90]
}

void semSignal(Mutex* m, PCB* process) {
    // Release lock and move a blocked process back to ready queue
}