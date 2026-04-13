#include "../include/os_core.h"
#include "../include/queue.h"
#include "../include/memory.h"
#include "../include/interpreter.h"
#include "../include/mutex.h"
#include "../include/scheduler.h"

// --- Global Variable Definitions ---
MemoryWord memory[MEMORY_SIZE];
Queue ready_queue;              
Queue general_blocked_queue;    

Mutex file_mutex;               
Mutex input_mutex;              
Mutex output_mutex;             

int clock_cycle = 0;

int main() {
    // 1. Initialization
    initialize_memory();
    
    // Initialize Mutexes [cite: 82]
    strcpy(file_mutex.resource_name, "file");
    strcpy(input_mutex.resource_name, "userInput");
    strcpy(output_mutex.resource_name, "userOutput");
    file_mutex.is_locked = input_mutex.is_locked = output_mutex.is_locked = false;

    // 2. Load Programs (Based on arrival times)
    // parse_and_load_program("program_1.txt", 0); [cite: 59, 112]
    // parse_and_load_program("program_2.txt", 1); [cite: 60, 113]
    // parse_and_load_program("program_3.txt", 4); [cite: 62, 114]

    // 3. Simulation Loop
    while (!is_empty(&ready_queue) || !is_empty(&general_blocked_queue)) {
        
        printf("\n--- Clock Cycle: %d ---\n", clock_cycle);
        print_memory_state(); 
        
        // Call your chosen scheduler
        // schedule_RR(); 

        clock_cycle++;
    }

    return 0;
}