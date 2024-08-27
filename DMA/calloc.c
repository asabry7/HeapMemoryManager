#include <string.h> 
#include "heap.h"

void *calloc(size_t nmemb, size_t size)
{
    void * ptr = HmmAlloc(nmemb * size);
    if (ptr != NULL) {
        /* Initialize allocated block with 0 */
        memset(ptr, 0, nmemb * size); 
    }
    
    return ptr;
}
