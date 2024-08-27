#include "heap.h"

void *malloc(size_t size)
{
    return HmmAlloc(size);
}
