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
Policy current_policy = MLFQ;

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

    /* 2. Load Programs 
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
    }*/
   /* // -------------------------------------------------

    printf("\n==========================================\n");
    printf("          Starting Execution Loop         \n");
    printf("==========================================\n");

    int programs_arrived = 0; // Track how many programs have been loaded
    int total_programs = 3;
    bool p1_req = false, p2_req = false, p3_req = false;
    
   while (1) {
        // --- NEW: ARIVAL LOGIC ---
        // Check if any program arrives at the current clock_cycle
        if (clock_cycle >= 0 && !p1_req) {
            if (parse_and_load_program("../programs/Program 1.txt", 0)) {
                p1_req = true; programs_arrived++;
            }
        }
        if (clock_cycle >= 1 && !p2_req) {
            if (parse_and_load_program("../programs/Program 2.txt", 1)) {
                p2_req = true; programs_arrived++;
            }
        }
        if (clock_cycle >= 4 && !p3_req) {
            if (parse_and_load_program("../programs/Program 3.txt", 4)) {
                p3_req = true; programs_arrived++;
            }
        }

        // --- MLFQ TRANSFER LOGIC ---
        // If MLFQ is active, move newly arrived processes from ready_queue to MLFQ Queue 0
        if (current_policy == MLFQ) {
            while (!is_empty(ready_queue)) {
                PCB* p = dequeue(ready_queue);
                p->priorityLevel = 0; 
                enqueue(mlfq_queues[0], p);
                printf("[Arrival] P%d added to MLFQ Queue 0 at Cycle %d\n", p->pid, clock_cycle);
            }
        }

        // Exit Condition: No process running, all queues empty, AND no more arrivals pending
        bool all_mlfq_empty = true;
        for (int i = 0; i < 4; i++) {
            if (!is_empty(mlfq_queues[i])) {
                all_mlfq_empty = false;
                break;
            }
        }

        // Added check for clock_cycle > 4 to ensure we don't exit before the last program arrives
        if (curr_process == NULL && is_empty(ready_queue) && 
            is_empty(general_blocked_queue) && all_mlfq_empty && clock_cycle > 4) {
            break; 
        }

        printf("\n--- Clock Cycle: %d ---\n", clock_cycle);

        // 3. Execution based on policy
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

        // 4. State Reporting
        printf("\n[End of Cycle %d State]\n", clock_cycle);
        if (current_policy == MLFQ) {
            for(int i=0; i<4; i++) {
                char q_name[20];
                sprintf(q_name, "MLFQ Queue %d", i);
                print_queue(mlfq_queues[i], q_name);
            }
        } else {
            print_queue(ready_queue, "Current Ready Queue");
        }
        print_queue(general_blocked_queue, "Current Blocked Queue");
        
        print_memory_state();
        printf("------------------------------------------\n");

        clock_cycle++;

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

    printf("\n==========================================\n");
    printf("          Starting Execution Loop         \n");
    printf("==========================================\n");

    int programs_arrived = 0; // Track how many programs have been loaded
    int total_programs = 3;
    bool p1_req = false, p2_req = false, p3_req = false;
    
    while (1) {
        // --- NEW: ARIVAL LOGIC (Queue Size Tracking) ---
        // Since parse_and_load_program is void, we check if it succeeded 
        // by seeing if it successfully added a new process to the ready_queue.

        if (clock_cycle >= 0 && !p1_req) {
            int old_size = ready_queue->size;
            parse_and_load_program("../programs/Program 1.txt", 0);
            if (ready_queue->size > old_size) {
                p1_req = true; 
                programs_arrived++;
            }
        }
        
        if (clock_cycle >= 1 && !p2_req) {
            int old_size = ready_queue->size;
            parse_and_load_program("../programs/Program 2.txt", 1);
            if (ready_queue->size > old_size) {
                p2_req = true; 
                programs_arrived++;
            }
        }
        
        if (clock_cycle >= 4 && !p3_req) {
            int old_size = ready_queue->size;
            parse_and_load_program("../programs/Program 3.txt", 4);
            if (ready_queue->size > old_size) {
                p3_req = true; 
                programs_arrived++;
            }
        }

        // --- MLFQ TRANSFER LOGIC ---
        // If MLFQ is active, move newly arrived processes from ready_queue to MLFQ Queue 0
        if (current_policy == MLFQ) {
            while (!is_empty(ready_queue)) {
                PCB* p = dequeue(ready_queue);
                p->priorityLevel = 0; 
                enqueue(mlfq_queues[0], p);
                printf("[Arrival] P%d added to MLFQ Queue 0 at Cycle %d\n", p->pid, clock_cycle);
            }
        }

        // Exit Condition: No process running, all queues empty, AND no more arrivals pending
        bool all_mlfq_empty = true;
        for (int i = 0; i < 4; i++) {
            if (!is_empty(mlfq_queues[i])) {
                all_mlfq_empty = false;
                break;
            }
        }

        // Added check to ensure we don't exit before the last program arrives
        if (curr_process == NULL && is_empty(ready_queue) && 
            is_empty(general_blocked_queue) && all_mlfq_empty && 
            programs_arrived == total_programs) {
            break; 
        }

        printf("\n--- Clock Cycle: %d ---\n", clock_cycle);

        // 3. Execution based on policy
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

        // 4. State Reporting
        printf("\n[End of Cycle %d State]\n", clock_cycle);
        if (current_policy == MLFQ) {
            for(int i=0; i<4; i++) {
                char q_name[20];
                sprintf(q_name, "MLFQ Queue %d", i);
                print_queue(mlfq_queues[i], q_name);
            }
        } else {
            print_queue(ready_queue, "Current Ready Queue");
        }
        print_queue(general_blocked_queue, "Current Blocked Queue");
        
        print_memory_state();
        printf("------------------------------------------\n");

        clock_cycle++;

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
}
