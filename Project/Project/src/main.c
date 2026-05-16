
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


#include "../include/os_core.h"
#include "../include/memory.h"
#include "../include/queue.h"
#include "../include/mutex.h"
#include "../include/scheduler.h"
#include "../include/interpreter.h"


Queue* ready_queue = NULL;
Queue* general_blocked_queue = NULL;
Queue* mlfq_queues[4]; 
int clock_cycle = 0;


extern PCB* curr_process; 


Policy current_policy ;


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


    int main(int argc, char *argv[]) {
        setvbuf(stdout, NULL, _IONBF, 0);
    
    current_policy = RR; 

    
    if (argc > 1) {
        int policy_choice = atoi(argv[1]);
        if (policy_choice == 1) current_policy = RR;
        else if (policy_choice == 2) current_policy = HRRN;
        else if (policy_choice == 3) current_policy = MLFQ;
    }

    printf("==========================================\n");
    printf("        OS Simulator Initialization       \n");
    printf("==========================================\n");

    
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

    int total_processes_arrived = 0;
    const int TOTAL_PROCESSES = 3;

    
    while (1) {
        
        bool newly_arrived = false;
        
        
        if (clock_cycle == 0) {
            printf("\n[Time %d] Loading Program 1 into Memory...\n", clock_cycle);
            parse_and_load_program("../programs/Program 1.txt", 0);
            total_processes_arrived++;
            newly_arrived = true;
        }
        if (clock_cycle == 1) {
            printf("\n[Time %d] Loading Program 2 into Memory...\n", clock_cycle);
            parse_and_load_program("../programs/Program 2.txt", 1);
            total_processes_arrived++;
            newly_arrived = true;
        }
        if (clock_cycle == 4) {
            printf("\n[Time %d] Loading Program 3 into Memory...\n", clock_cycle);
            parse_and_load_program("../programs/Program 3.txt", 4);
            total_processes_arrived++;
            newly_arrived = true;
        }

        
        if (newly_arrived && current_policy == MLFQ) {
            while (!is_empty(ready_queue)) {
                PCB* p = dequeue(ready_queue);
                p->priorityLevel = 0; // Ensure they start at highest priority
                enqueue(mlfq_queues[0], p);
            }
        }
        // -------------------------------------------------

        
        bool all_mlfq_empty = true;
        for (int i = 0; i < 4; i++) {
            if (!is_empty(mlfq_queues[i])) {
                all_mlfq_empty = false;
                break;
            }
        }

        if (total_processes_arrived == TOTAL_PROCESSES && curr_process == NULL && 
            is_empty(ready_queue) && is_empty(general_blocked_queue) && all_mlfq_empty) {
            break; 
        }

        printf("\n--- Clock Cycle: %d ---\n", clock_cycle);

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

        
        if (clock_cycle > 200) {
            printf("\n[!] Simulation halted at 200 cycles (Infinite Loop Protection).\n");
            break;
        }
    }

    printf("\n==========================================\n");
    printf("            Simulation Complete           \n");
    printf("==========================================\n");

    print_memory_state();

    freeQueue(ready_queue);
    freeQueue(general_blocked_queue);
    for (int i = 0; i < 4; i++) {
        freeQueue(mlfq_queues[i]);
    }

    return 0;
}