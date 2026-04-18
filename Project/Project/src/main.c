/*#include <stdio.h>
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
Policy current_policy = HRRN;

// --- Core Helper Functions ---

// 1. PID Generator expected by interpreter.c
int current_pid = 1;
int generatePID() {
    return current_pid++;
}

// 2. Queue Getters expected by scheduler.c and mutex.c
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
    printf("       OS Simulator Initialization        \n");
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
    // (Create these dummy .txt files in your directory to test)
    printf("\n[Time 0] Loading programs into Memory...\n");
    
    // We assume program files exist. If they don't, it will print your error.
    parse_and_load_program("../programs/Program 1.txt", 0);
    parse_and_load_program("../programs/Program 2.txt", 1);
    parse_and_load_program("../programs/Program 3.txt", 4); 

    // --- FIX: TRANSFER PROCESSES TO MLFQ IF ACTIVE ---
    if (current_policy == MLFQ) {
        while (!is_empty(ready_queue)) {
            PCB* p = dequeue(ready_queue);
            p->priorityLevel = 0; // Ensure they start at highest priority
            enqueue(mlfq_queues[0], p);
        }
    }
    // -------------------------------------------------

    printf("\n==========================================\n");
    printf("          Starting Execution Loop         \n");
    printf("==========================================\n");

    // 3. The CPU Clock Cycle Loop
    while (1) {
        // Exit Condition: No process running, and all queues are empty
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

        printf("\n--- Clock Cycle: %d ---\n", clock_cycle);

        // ----------------------------------------------------
        // Pick ONE scheduler algorithm here to test at a time:
        // ----------------------------------------------------
        
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

        // --- RUBRIC OUTPUT REQUIREMENTS ADDED HERE ---
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
        // ---------------------------------------------

        clock_cycle++;

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
Policy current_policy = MLFQ;

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
    
    parse_and_load_program("../programs/Program 1.txt", 0);
    parse_and_load_program("../programs/Program 2.txt", 1);
    parse_and_load_program("../programs/Program 3.txt", 4); 

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
    while (1) {
        // Exit Condition
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
    }

    return 0;
}