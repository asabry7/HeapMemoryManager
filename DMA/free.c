#include "heap.h"

void free(void *ptr)
{
    HmmFree(ptr);
    return;
}