#include <stdio.h>
#include "../include/os_core.h"
#include "../include/memory.h"

int clock_cycle = 0;

int main() {
    printf("Starting Full Memory Lifecycle Test...\n");
    initialize_memory();

    PCB p1;
    p1.pid = 1;
    p1.state = READY;
    p1.arrival_time = 0;

    printf("\n>>> STEP 1: INITIAL LOAD <<<\n");
    if (allocate_memory(&p1, "programs/Program 1.txt")) {
        
        printf("\n>>> STEP 2: SWAPPING OUT TO DISK <<<\n");
        swap_to_disk(&p1);
        print_memory_state(); // Should print nothing!

        printf("\n>>> STEP 3: SWAPPING BACK IN FROM DISK <<<\n");
        swap_from_disk(&p1);
        print_memory_state(); // All 14 words should be perfectly restored!
    }

    printf("\nSimulation test complete.\n");
    return 0;
}