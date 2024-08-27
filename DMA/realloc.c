#include "heap.h"
#include <string.h> 

void *realloc(void *old_ptr, size_t new_size)
{
    if(old_ptr == NULL)
    {
        return HmmAlloc(new_size);
    }

    if(new_size == 0)
    {
        HmmFree(old_ptr);
        return NULL;
    }

    void *new_ptr = HmmAlloc(new_size);
    if (new_ptr == NULL)
    {
        return NULL; // Return NULL if allocation fails
    }

    /* Copy content from old to new ptr */
    size_t old_size = (( FreeBlock* )old_ptr)->BlockSize;

    size_t copy_size = (old_size < new_size) ? old_size : new_size;
    
    memcpy(new_ptr, old_ptr, copy_size);

    /* Free the old ptr */
    HmmFree(old_ptr);

    /* Return the new ptr */
    return new_ptr;
    
}
