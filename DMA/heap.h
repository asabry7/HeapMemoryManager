/************************************************************************************************/
/************************************************************************************************/
/************************************************************************************************/
/**************************      @SWC:         heap.h                ****************************/
/**************************      @author:     Abdelrahman Sabry      ****************************/
/**************************      @date:       17 Aug                 ****************************/
/**************************      @version:    1                      ****************************/
/************************************************************************************************/
/************************************************************************************************/
/************************************************************************************************/

#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <string.h>

#define ON                      1   
#define OFF                     0   
#define DEBUG                   OFF

#define HEAP_SIZE               500 * 1024 * 1024   // 500 MB
#define STEP_SIZE               1   * 1024 * 1024   //   1 MB          

#define HEAP_IS_FULL            1
#define CORRUPTED_LIST          2
#define PROPPER_LIST            3

#define METADATA_SIZE           sizeof( FreeBlock )
#define MINIMUM_BLOCK_SIZE      8

/* Define a structure to represent a free block in memory */
typedef struct FreeBlock {
    /* Size of the free memory block (without meta data) */
    size_t BlockSize;
    /* Pointer to the next free block in the list */
    struct FreeBlock* NextFreeBlock;
    /* Pointer to the previous free block in the list */
    struct FreeBlock* PreviousFreeBlock;
} FreeBlock;



/**
 * @brief Checks the free block list for corruption.
 *
 * This function iterates through the free block linked list and checks for any corruption,
 * such as blocks being out of order, having invalid sizes, or overlapping with each other.
 *
 * @return int Returns `CORRUPTED_LIST` if corruption is found, otherwise returns `PROPPER_LIST`.
 */
int SearchForCorruption();

/**
 * @brief Prints the current free block list.
 *
 * This function prints the free block list, displaying each block's size, address,
 * and links to the previous and next free blocks. It is enabled only if debugging is on.
 */
void PrintList();

/**
 * @brief Splits a free block into two blocks.
 *
 * This function splits a given free block into two blocks: one block with the requested size and 
 * another block with the remaining size. It adjusts the links in the free block list accordingly.
 *
 * @param CurrentBlock Pointer to the pointer of the block to be split.
 * @param Requested_Size The size of the block to be allocated.
 * @return void* A pointer to the allocated block.
 */
void* SplitFreeBlock(FreeBlock** CurrentBlock, size_t Requested_Size);

/**
 * @brief Removes a free block from the free list.
 *
 * This function removes a given free block from the linked list of free blocks. It adjusts the 
 * links of the neighboring blocks to maintain the integrity of the list.
 *
 * @param CurrentBlock Pointer to the pointer of the block to be removed.
 * @return void* A pointer to the memory location of the removed block.
 */
void* RemoveFreeBlock(FreeBlock ** CurrentBlock);

/**
 * @brief Adds a block back to the free list.
 *
 * This function adds a given block back to the linked list of free blocks, maintaining
 * the list in sorted order based on memory addresses.
 *
 * @param deletedBlock Pointer to the pointer of the block to be added.
 */
void AddFreeBlock(FreeBlock ** deletedBlock);

/**
 * @brief Combines adjacent free blocks.
 *
 * This function merges a given free block with its adjacent free blocks, if they are contiguous in memory,
 * to form a larger free block and reduce fragmentation.
 *
 * @param deletedBlock Pointer to the pointer of the block to be combined.
 */
void CombineAdjacentFreeBlocks(FreeBlock **deletedBlock);


/**
 * @brief Initializes the heap memory for allocation.
 * 
 * This function sets up the initial heap structure by creating the first 
 * free block that spans the entire allocated heap space. It increases the 
 * program break by the predefined step size and sets the head of the free 
 * block list to this new block.
 */
void Heap_Init();


/**
 * @brief Allocates memory from the heap.
 * 
 * This function searches for a suitable free block to satisfy the requested 
 * allocation size. If necessary, it will extend the heap by increasing the 
 * program break. The function handles splitting blocks, taking the entire 
 * block if it cannot be split, or extending the heap when no suitable block 
 * is found.
 * 
 * @param Requested_Size The size of memory to allocate.
 * @return void* A pointer to the allocated memory, or NULL if the allocation fails.
 */
void * HmmAlloc(size_t Requested_Size);


/**
 * @brief Frees allocated memory and returns it to the free list.
 * 
 * This function deallocates the memory pointed to by `ptr` and adds the 
 * corresponding block back to the free list. It also combines adjacent free 
 * blocks to reduce fragmentation and maintain a proper free list structure.
 * 
 * @param ptr Pointer to the memory to be freed.
 */
void HmmFree(void *ptr);



#endif
