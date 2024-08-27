#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#include "heap.h"

#define NUM_ALLOCS          10000
#define MAX_ITERATIONS      100000
#define MAX_SIZE            10240
int FailCounter = 0;

extern char HeapMemory[HEAP_SIZE];


void random_alloc_free_test(FILE* logFile) {

    srand((unsigned int)time(NULL));
    char* pointers[NUM_ALLOCS] = {NULL};
    
    for (int i = 0; i < MAX_ITERATIONS; ++i) {
        printf("Allocating number %d:\n", i);  
        int index = rand() % NUM_ALLOCS;
        size_t size = (size_t)(rand() % MAX_SIZE) + 1;

        if (pointers[index] == NULL) {
            // Allocate memory

            pointers[index] = (char*)HmmAlloc(size);
            
            if (pointers[index] != NULL) 
            {
                printf("Allocated memory of size %zu at address %ld\n", size, (char*)pointers[index]-HeapMemory);

            } 
            
            else 
            {
                printf( "Allocation failed for size %zu\n", size);
                FailCounter++;

            }
        } else {
            // Free memory
            printf("Freeing memory at address %ld\n", (char*)pointers[index]-HeapMemory);
            printf("Before Free value:%d\n", *pointers[index]);
            HmmFree(pointers[index]);
            pointers[index] = NULL;
        }
    }
    
    // Free remaining allocated memory
    for (int i = 0; i < NUM_ALLOCS; ++i) {
        printf("Freeing number %d:\n", i);
        if (pointers[i] != NULL) {
            printf("Freeing remaining memory at address %ld\n", (char*)pointers[i]-HeapMemory);
            printf("Before Free value:%d\n", *pointers[i]);
            *pointers[i] = -1;
            HmmFree(pointers[i]);
            pointers[i] = NULL;
        }
    }

    printf("Linked list after freeing\n");

}

int main() {
    FILE* logFile = fopen("test_log.txt", "w");
    if (logFile == NULL) {
        printf("Failed to open log file.\n");
        return 1;
    }

    printf("Starting random allocation and deallocation test...\n");
    
    random_alloc_free_test(logFile);

    PrintList();
    printf("Fail Percentage: %f%%\n", (100.0 * FailCounter) / (float)MAX_ITERATIONS);    
    fclose(logFile);
    
    return 0;
}


/**
 * Starting random allocation and deallocation test...
 * Allocating number 0:
 * Allocated memory of size 8200 at address 24
 * Allocating number 1:
 * Allocated memory of size 1864 at address 8248
 * Allocating number 2:
 * Allocated memory of size 6400 at address 10136
 * Allocating number 3:
 * Allocated memory of size 9400 at address 16560
 * Allocating number 4:
 * Allocated memory of size 6192 at address 25984
 * Allocating number 5:
 * Freeing memory at address 24
 * Before Free value:0
 * Allocating number 7:
 * Allocated memory of size 8584 at address 33576
 */

// int main()
// {
//     void* pointers[10] = {NULL};
//     pointers[0] = HmmAlloc(0);
//     if( pointers[0] )
//         printf"allocated 1 size %ld\n", ((FreeBlock*)pointers[0])->BlockSize);
//     PrintList();

    
//     return 0;
// }