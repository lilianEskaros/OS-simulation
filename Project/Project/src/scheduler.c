#include "../include/scheduler.h"
#include "../include/queue.h"
#include "../include/interpreter.h"


PCB* curr_process = NULL;
int time_slice_c = 0;

void schedule_RR() {
    Queue* ready_queue = get_ready_queue();
    Queue* ready_queue = get_blocked_queue();
    // 1. If no process is running, pick one
    if (curr_process == NULL) {
        if (is_empty(ready_queue)) return;
        
        curr_process = dequeue(ready_queue);
        curr_process->state = RUNNING;
        time_slice_c = 0;
        printf("Selected Process: P%d\n", curr_process->processID);
        print_queue(ready_queue, "Ready Queue");
    }

    // 2. Execute
    execute_next_instruction(curr_process);
    time_slice_c++;

    // 3. Preempt if: Time slice finished OR process finished/blocked
    if (curr_process->state == FINISHED || curr_process->state == BLOCKED) {
        curr_process = NULL; 
    } 
    else if (time_slice_c >= 2) { 
        // Process is still RUNNING but time is up
        curr_process->state = READY;
        enqueue(ready_queue, curr_process);
        curr_process = NULL;
    }
}

void schedule_HRRN() {
    // Highest Response Ratio Next logic [cite: 100]
}