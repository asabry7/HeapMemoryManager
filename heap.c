/************************************************************************************************/
/************************************************************************************************/
/************************************************************************************************/
/**************************      SWC:        heap.c                 *****************************/
/**************************      Author:     Abdelrahman Sabry      *****************************/
/**************************      Date:       27 Aug                 *****************************/
/**************************      Version:    1                      *****************************/
/************************************************************************************************/
/************************************************************************************************/
/************************************************************************************************/


/**************************      Included Librariea and headers      ****************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* sbrk */
#include <unistd.h>

#include "heap.h"


/**************************             Global Variables             ****************************/

char* HeapMemory;
char* ProgramBreak;
FreeBlock * Head = NULL;


/**************************             Helper functions             ****************************/

int SearchForCorruption()
{
    FreeBlock* Temp = Head;
    int count = 1;
    while( Temp )
    {
        /* Check the order of Free Blocks */
        if( ( Temp->NextFreeBlock && Temp > Temp->NextFreeBlock) || ( Temp->PreviousFreeBlock && Temp < Temp->PreviousFreeBlock ))
        {
            printf("Block %d is in wrong order\n", count);
            return CORRUPTED_LIST;
        }

        /* Check for overflow in size*/
        if( Temp->BlockSize > 9999999999 || Temp->BlockSize < 0 )
        {
            printf("Block %d has impossible size %ld\n", count, Temp->BlockSize);
            return CORRUPTED_LIST;
        }

        /* Check if Free blocks overlapped */
        if( Temp->NextFreeBlock  &&  ( char* )Temp + Temp->BlockSize > ( char* )Temp->NextFreeBlock )
        {
            printf("Block %d overlapped with the next block\n", count);
            return CORRUPTED_LIST;
        }

        count++;
        Temp = Temp->NextFreeBlock;
    }

    return PROPPER_LIST;
}


void PrintList()
{
    #if DEBUG == ON

    if(Head == NULL)
    {
        printf( "Linked list is empty\n");
        return;
    }

    FreeBlock* Temp = Head;
    printf( "\n\n========================================        Free List Content        =======================================\n");
    
    // Print table header
    printf( "================================================================================================================\n");
    printf( "| %-10s | %-10s | %-10s | %-10s | %-10s |\n", "Node", "Size", "Address", "Next Node", "Prev Node");
    printf( "================================================================================================================\n");
    
    for(int i = 1; Temp != NULL; i++)
    {
        int Next = Temp->NextFreeBlock == NULL ?        -1 : (char*) (Temp->NextFreeBlock) - HeapMemory;
        int Prev = Temp->PreviousFreeBlock == NULL ?    -1 : (char*) (Temp->PreviousFreeBlock) - HeapMemory;
        printf( "| %-10d | %-10ld | %-10ld | %-10d | %-10d |\n", 
                i, 
                Temp->BlockSize, 
                (char*) Temp - HeapMemory, 
                Next, 
                Prev);
        
        Temp = Temp->NextFreeBlock;

    }
    
    printf( "================================================================================================================\n");

    #endif

}



void* SplitFreeBlock(FreeBlock** CurrentBlock, size_t Requested_Size) {
    size_t remainingSize = (*CurrentBlock)->BlockSize - Requested_Size - METADATA_SIZE;

    // Calculate the address for the remaining block after the split
    FreeBlock* remainingBlock = (FreeBlock*)((char*)(*CurrentBlock) + METADATA_SIZE + Requested_Size);

    // Set the size for the remaining block
    remainingBlock->BlockSize = remainingSize;
    
    // Adjust the current block to the requested size
    (*CurrentBlock)->BlockSize = Requested_Size;

    // Update the links in the free list

    remainingBlock->NextFreeBlock = (*CurrentBlock)->NextFreeBlock;
    remainingBlock->PreviousFreeBlock = (*CurrentBlock)->PreviousFreeBlock;

    if ((*CurrentBlock)->NextFreeBlock) {
        (*CurrentBlock)->NextFreeBlock->PreviousFreeBlock = remainingBlock;
    }

    if ((*CurrentBlock)->PreviousFreeBlock) {
        (*CurrentBlock)->PreviousFreeBlock->NextFreeBlock = remainingBlock;
    }

    // If the current block is the head, update the head pointer
    if (*CurrentBlock == Head) {
        Head = remainingBlock;
    }


    assert( SearchForCorruption() == PROPPER_LIST );

    return (void*)((char*)(*CurrentBlock) + METADATA_SIZE);
}


void* RemoveFreeBlock(FreeBlock ** CurrentBlock)
{

    /* Case 1: only 1 node in the linked list */
    if((*CurrentBlock)->PreviousFreeBlock == NULL && (*CurrentBlock)->NextFreeBlock == NULL )
    {
        Head = NULL;
    }

    /* Case 2: First block */
    else if( Head == (*CurrentBlock) )
    {
        Head = (*CurrentBlock)->NextFreeBlock;
        if( Head )
        {
            Head->PreviousFreeBlock = NULL;
        }
        
    }

    /* Case 3: Last block */
    else if( (*CurrentBlock)->NextFreeBlock == NULL )
    {
        (*CurrentBlock)->PreviousFreeBlock->NextFreeBlock = NULL;
    }

    /* Case 4: middle block */
    else if ( (*CurrentBlock)->NextFreeBlock && (*CurrentBlock)->PreviousFreeBlock )
    {
        (*CurrentBlock)->PreviousFreeBlock->NextFreeBlock = (*CurrentBlock)->NextFreeBlock;
        (*CurrentBlock)->NextFreeBlock->PreviousFreeBlock = (*CurrentBlock)->PreviousFreeBlock;
    }

    assert( SearchForCorruption() == PROPPER_LIST );

    return ( void* )( ( char* )(*CurrentBlock) + METADATA_SIZE ); 

}

void AddFreeBlock(FreeBlock ** deletedBlock)
{
    
    if( (*deletedBlock) == NULL )
    {
        printf("Adding a NULL node\n");
        
        return;
    }

    /* Case 1: Linked list is empty */
    if( Head == NULL )
    {
        Head = *deletedBlock;
        (*deletedBlock)->NextFreeBlock = NULL;
        (*deletedBlock)->PreviousFreeBlock = NULL;
        return;
    }

    /* Case 2 : if deleted block's location is lower than the current Head or it is the first block in heap */
    if( (*deletedBlock) == ( FreeBlock* )HeapMemory || (*deletedBlock) < Head )
    {
        (*deletedBlock)->NextFreeBlock = Head;
        (*deletedBlock)->PreviousFreeBlock = NULL;
        Head->PreviousFreeBlock = (*deletedBlock);
        Head = (*deletedBlock);
        return;
    }

    /* Set the iterator node: */
    FreeBlock * CurrentBlock = Head;

    while( CurrentBlock )
    {
        /* if the Block is already in free list => return */
        if( (*deletedBlock) == CurrentBlock )
        {
            return;
        }

        /* Case 3 : Last block in the Heap */
        else if( CurrentBlock->NextFreeBlock == NULL )
        {
            CurrentBlock->NextFreeBlock = (*deletedBlock);
            (*deletedBlock)->PreviousFreeBlock = CurrentBlock;
            (*deletedBlock)->NextFreeBlock = NULL;
            break;
        }

        /* Case 4: Middle node */
        else if( (*deletedBlock) < CurrentBlock->NextFreeBlock && (*deletedBlock) > CurrentBlock)
        {
            CurrentBlock->NextFreeBlock->PreviousFreeBlock = (*deletedBlock);
            (*deletedBlock)->NextFreeBlock = CurrentBlock->NextFreeBlock;
            (*deletedBlock)->PreviousFreeBlock = CurrentBlock;
            CurrentBlock->NextFreeBlock = (*deletedBlock);
            break;
        }

        CurrentBlock = CurrentBlock->NextFreeBlock;
    }


    assert( SearchForCorruption() == PROPPER_LIST );

}

void CombineAdjacentFreeBlocks(FreeBlock **deletedBlock) 
{
    FreeBlock *prevBlock = (*deletedBlock)->PreviousFreeBlock;
    FreeBlock *nextBlock = (*deletedBlock)->NextFreeBlock;

    /* Case 1: Combine with next block if adjacent */
    if (nextBlock && ((FreeBlock*)((char*)(*deletedBlock) + (*deletedBlock)->BlockSize + METADATA_SIZE) == nextBlock)) {
        (*deletedBlock)->BlockSize += nextBlock->BlockSize + METADATA_SIZE;
        (*deletedBlock)->NextFreeBlock = nextBlock->NextFreeBlock;
        if (nextBlock->NextFreeBlock) {
            nextBlock->NextFreeBlock->PreviousFreeBlock = *deletedBlock;
        }

        /* Update next pointer: */
        nextBlock = (*deletedBlock)->NextFreeBlock;
    }

    /* Case 2: Combine with previous block if adjacent */
    if (prevBlock && ((FreeBlock*)((char*)prevBlock  + prevBlock->BlockSize + METADATA_SIZE ) == (*deletedBlock))) {
        prevBlock->BlockSize += (*deletedBlock)->BlockSize + METADATA_SIZE;
        prevBlock->NextFreeBlock = nextBlock;
        if (nextBlock) {
            nextBlock->PreviousFreeBlock = prevBlock;
        }
    }


    assert( SearchForCorruption() == PROPPER_LIST );

}

void Heap_Init()
{

    HeapMemory = sbrk(0);
    ProgramBreak = HeapMemory;

    /* Increase Program Break */
    ProgramBreak = sbrk( STEP_SIZE ) + STEP_SIZE; 

    FreeBlock* newHead = ( FreeBlock* )HeapMemory;

    /* Assign the heap size to the new head */
    newHead->BlockSize = ProgramBreak - HeapMemory - METADATA_SIZE; 

    printf("Heap intial size: %ld\n", newHead->BlockSize);
    newHead->PreviousFreeBlock = NULL;
    newHead->NextFreeBlock = NULL;

    /* Update head: */
    Head = newHead;
}

void * HmmAlloc(size_t Requested_Size)
{
    /* If called first time => call the init function */
    static int FirstTime = 1;
    if (FirstTime)
    {
        Heap_Init();
        FirstTime = 0;
    }

    void *ptr = NULL;

    /* if the requested size is zero => return a pointer to a block with minimum valid size */
    if (Requested_Size == 0)
    {
        return HmmAlloc( MINIMUM_BLOCK_SIZE );
    }

    /* Align size to 8 */
    Requested_Size = ((Requested_Size + 7) / 8) * 8;

    /* Set the iterator block: */
    FreeBlock *CurrentBlock = Head;

    while (1)
    {
        /* Case 1: Linked List is empty or no suitable block is found => extend the heap by increasing ProgramBreak */
        if (CurrentBlock == NULL)
        {
            ProgramBreak = sbrk( STEP_SIZE ) + STEP_SIZE;

            /* If reached the end of the heap => return NULL*/
            if (ProgramBreak > HeapMemory + HEAP_SIZE) 
            {
                printf("Heap is full\n");
                return NULL;
            }

            /* Add new free block to the heap */
            FreeBlock *newBlock = (FreeBlock *)(ProgramBreak - STEP_SIZE);
            newBlock->BlockSize = STEP_SIZE - METADATA_SIZE;
            newBlock->NextFreeBlock = NULL;
            newBlock->PreviousFreeBlock = NULL;

            AddFreeBlock(&newBlock);

            /* Reset CurrentBlock to the head of the updated free list */
            CurrentBlock = Head;
            continue;
        }

        /* Case 2 : Current block is larger than the required size and the remaining after splitting is a valid block */
        if (CurrentBlock->BlockSize > (Requested_Size + MINIMUM_BLOCK_SIZE + METADATA_SIZE) && 
            (char*)CurrentBlock + CurrentBlock->BlockSize + METADATA_SIZE <= ProgramBreak)
        {
            /* Split the acquired block */
            ptr = SplitFreeBlock(&CurrentBlock, Requested_Size);
            break;
        }

        /* Case 3 : Current block is larger than the required size BUT the remaining after splitting is NOT a valid block */
        else if (CurrentBlock->BlockSize - Requested_Size < MINIMUM_BLOCK_SIZE + METADATA_SIZE && 
                 (char*)CurrentBlock + CurrentBlock->BlockSize + METADATA_SIZE <= ProgramBreak)
        {
            /* Take the whole block (No splitting) */
            ptr = RemoveFreeBlock(&CurrentBlock);
            break;
        }

        /* Case 4 : Current block size is exactly the same as the requested size */
        else if (CurrentBlock->BlockSize == Requested_Size && 
                 (char*)CurrentBlock + CurrentBlock->BlockSize + METADATA_SIZE <= ProgramBreak)
        {
            ptr = RemoveFreeBlock(&CurrentBlock);
            break;
        }

        CurrentBlock = CurrentBlock->NextFreeBlock;
    }

    /* If the aquired pointer points to invalid address (outside the boundaries of heap) => return NULL */
    if ((char*)ptr < HeapMemory || (char*)ptr > ProgramBreak)
    {
        ptr = NULL;
    }

    assert(SearchForCorruption() == PROPPER_LIST);

    return ptr;
}



void HmmFree(void *ptr)
{
    if (ptr == NULL)
    {
        return;
    }

    FreeBlock* deletedBlock = (FreeBlock*)((char*)ptr - METADATA_SIZE);

    /* Add the node in the linked list */
    AddFreeBlock(&deletedBlock);

    /* Combine adjacent Free blocks */
    CombineAdjacentFreeBlocks(&deletedBlock);

   

    assert(SearchForCorruption() == PROPPER_LIST);
    return;
}
