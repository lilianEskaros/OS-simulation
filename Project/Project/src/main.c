#include "../include/os_core.h"
#include "../include/queue.h"
#include "../include/memory.h"
#include "../include/interpreter.h"
#include "../include/mutex.h"
#include "../include/scheduler.h"

// --- Global Variable Definitions ---
MemoryWord memory[MEMORY_SIZE];

Queue* ready_queue;              
Queue* blocked_queue;    
Queue* mlfq_queues[4];

Mutex file_mutex;               
Mutex input_mutex;              
Mutex output_mutex;             

int clock_cycle = 0;


Queue* get_ready_queue() {
    return ready_queue;
}
Queue* get_blocked_queue() {
    return blocked_queue;
}
Queue** get_mlfq_queues() {
    return mlfq_queues; 
}
typedef enum { RR, HRRN, MLFQ } Policy;
Policy current_policy = RR;

int main() {
    // Initialization of memory
    initialize_memory();
    //Initialization of Queues
    ready_queue = createQueue(); //el queues el 3adeya
    blocked_queue = createQueue();
    //file_mutex.blocked_queue = createQueue();  // dol el mutex queues (lazem yet3mel el mutex datastructure el awel)
    //input_mutex.blocked_queue = createQueue();
    //output_mutex.blocked_queue = createQueue();
    for(int i = 0; i < 4; i++) {
        mlfq_queues[i] = createQueue();
    }

    Policy current_policy = RR;


    // Initialize Mutexes [cite: 82]
    strcpy(file_mutex.resource_name, "file");  //ME7TAGEEN NEZZABAT DOL WITH CONSTANTS
    strcpy(input_mutex.resource_name, "userInput");
    strcpy(output_mutex.resource_name, "userOutput");
    file_mutex.is_locked = input_mutex.is_locked = output_mutex.is_locked = false;

    // 2. Load Programs (Based on arrival times)
    // parse_and_load_program("program_1.txt", 0); [cite: 59, 112]
    // parse_and_load_program("program_2.txt", 1); [cite: 60, 113]
    // parse_and_load_program("program_3.txt", 4); [cite: 62, 114]

    // 3. Simulation Loop
    while (!is_empty(ready_queue) || !is_empty(blocked_queue) || curr_process != NULL) {
        
        printf("\n--- Clock Cycle: %d ---\n", clock_cycle);
        print_memory_state(); 
        // 1. Check for new process arrivals at this current clock_cycle
         check_and_load_arriving_processes(clock_cycle); // IS THIS IMPLEMENTED OR NOT 

         // 2. Call the Scheduler 
        // This is where the magic happens!
         switch (current_policy) {
            case RR:
                schedule_RR();
                break;
            case HRRN:
                schedule_HRRN();
                break;
            case MLFQ:
                schedule_MLFQ();
                break;
        }
        // 2. Call the Scheduler 
        // This is where the magic happens!
    

        // 3. Update system state
        update_waiting_times(ready_queue); // Important for HRRN! //IS THIS IMPLEMENTED OR NOT
    
        // Call your chosen scheduler
        // schedule_RR(); 

        clock_cycle++;
    }

    return 0;
}