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
<<<<<<< Updated upstream
=======

        // Failsafe to prevent an accidental infinite loop during debugging
        if (clock_cycle > 200) {
            printf("\n[!] Simulation halted at 200 cycles (Infinite Loop Protection).\n");
            break;
        }
    }

    printf("\n==========================================\n");
    printf("             Simulation Complete          \n");
    printf("==========================================\n");

    // 4. Final state check
    print_memory_state();

    // 5. Clean up allocated heap memory
    freeQueue(ready_queue);
    freeQueue(general_blocked_queue);
    for (int i = 0; i < 4; i++) {
        freeQueue(mlfq_queues[i]);
    }

    return 0;
}*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Include your headers
#include "../include/os_core.h"
#include "../include/memory.h"
#include "../include/queue.h"
#include "../include/mutex.h"
#include "../include/scheduler.h"
#include "../include/interpreter.h"

// --- Global Variables Defined in os_core.h ---
Queue* ready_queue = NULL;
Queue* general_blocked_queue = NULL;
Queue* mlfq_queues[4]; 
int clock_cycle = 0;

// Need access to the running process to check when simulation ends
extern PCB* curr_process; 

// Definition of the global policy
Policy current_policy = RR;

// --- Core Helper Functions ---

int current_pid = 1;
int generatePID() {
    return current_pid++;
}

Queue* get_ready_queue() {
    return ready_queue;
}

Queue* get_blocked_queue() {
    return general_blocked_queue;
}

Queue** get_mlfq_queues() {
    return mlfq_queues;
}

// --- Main Simulation Loop ---

int main() {
    printf("==========================================\n");
    printf("        OS Simulator Initialization       \n");
    printf("==========================================\n");

    // 1. Initialize System Structures
    initialize_memory();
    initialize_mutexes();

    ready_queue = createQueue();
    general_blocked_queue = createQueue();
    for (int i = 0; i < 4; i++) {
        mlfq_queues[i] = createQueue();
    }

    // 2. Load Programs 
    printf("\n[Time 0] Loading programs into Memory...\n");
    
   // parse_and_load_program("../programs/Program 1.txt", 0);
   // parse_and_load_program("../programs/Program 2.txt", 1);
   // parse_and_load_program("../programs/Program 3.txt", 4); 

    if (current_policy == MLFQ) {
        while (!is_empty(ready_queue)) {
            PCB* p = dequeue(ready_queue);
            p->priorityLevel = 0; 
            enqueue(mlfq_queues[0], p);
        }
    }

    printf("\n==========================================\n");
    printf("           Starting Execution Loop         \n");
    printf("==========================================\n");

    // 3. The CPU Clock Cycle Loop
   // while (1) {
        // Exit Condition
     //   bool all_mlfq_empty = true;
       // for (int i = 0; i < 4; i++) {
         //   if (!is_empty(mlfq_queues[i])) {
           //     all_mlfq_empty = false;
             //   break;
            //}
        //}
        


while (1) {
        
        // --- DYNAMIC ARRIVAL LOGIC ---
        bool new_arrival = false;

        if (clock_cycle == 0) {
            printf("\n[Arrival] Loading Program 1 at Time 0...\n");
            parse_and_load_program("../programs/Program 1.txt", 0);
            new_arrival = true;
        }
        if (clock_cycle == 1) {
            printf("\n[Arrival] Loading Program 2 at Time 1...\n");
            parse_and_load_program("../programs/Program 2.txt", 1);
            new_arrival = true;
        }
        if (clock_cycle == 4) {
            printf("\n[Arrival] Loading Program 3 at Time 4...\n");
            parse_and_load_program("../programs/Program 3.txt", 4);
            new_arrival = true;
        }

        // If MLFQ is active, route the new arrivals to Queue 0
        if (new_arrival && current_policy == MLFQ) {
            while (!is_empty(ready_queue)) {
                PCB* p = dequeue(ready_queue);
                p->priorityLevel = 0; 
                enqueue(mlfq_queues[0], p);
            }
        }
        // -----------------------------

        // --- EXIT CONDITION ---
        bool all_mlfq_empty = true;
        for (int i = 0; i < 4; i++) {
            if (!is_empty(mlfq_queues[i])) {
                all_mlfq_empty = false;
                break;
            }
        }


        if (curr_process == NULL && is_empty(ready_queue) && 
            is_empty(general_blocked_queue) && all_mlfq_empty) {
            break; 
        }

        // --- THE FIX: Smart Clock Management ---
        // We use an integer to see if the scheduler actually did a cycle of work
        int work_done = 0;

        // Print the header for the cycle only if we are actually about to attempt work
        // (Optional: You can move this inside the work_done check if you want cleaner logs)
        // printf("\n--- Clock Cycle: %d ---\n", clock_cycle);

        switch (current_policy) {
            case RR:
                work_done = schedule_RR(); // schedule functions must now return int
                break;
            case HRRN:
                work_done = schedule_HRRN();
                break;
            case MLFQ:
                work_done = schedule_MLFQ();
                break;
        }

        // ONLY if an instruction was executed, we print the state and increment the clock
        if (work_done) {
            printf("\n--- Clock Cycle: %d ---\n", clock_cycle);
            printf("\n[End of Cycle %d State]\n", clock_cycle);
            
            if (current_policy == MLFQ) {
                print_queue(mlfq_queues[0], "MLFQ Queue 0");
                print_queue(mlfq_queues[1], "MLFQ Queue 1");
                print_queue(mlfq_queues[2], "MLFQ Queue 2");
                print_queue(mlfq_queues[3], "MLFQ Queue 3");
            } else {
                print_queue(ready_queue, "Current Ready Queue");
            }
            print_queue(general_blocked_queue, "Current Blocked Queue");
            
            print_memory_state();
            printf("------------------------------------------\n");

            clock_cycle++; 
        }

        // Failsafe
        if (clock_cycle > 200) {
            printf("\n[!] Simulation halted at 200 cycles.\n");
            break;
        }
    }

    printf("\n==========================================\n");
    printf("             Simulation Complete          \n");
    printf("==========================================\n");

    print_memory_state();

    // Clean up
    freeQueue(ready_queue);
    freeQueue(general_blocked_queue);
    for (int i = 0; i < 4; i++) {
        freeQueue(mlfq_queues[i]);
>>>>>>> Stashed changes
    }

    return 0;
}