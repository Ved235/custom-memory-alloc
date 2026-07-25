#pragma once
#include <cstddef>

class FreeListAllocator
{
private:
    struct FreeBlock
    {
        size_t size;
        FreeBlock *next;
    };
    struct AllocatedBlock
    {
        size_t size;
        size_t padding;
    };

    FreeBlock *freeList;
    size_t totalSize;
    char *start;

public:
    FreeListAllocator(size_t size);
    void *Allocate(size_t size);
    void Free(void *ptr);
    ~FreeListAllocator();
    FreeListAllocator(const FreeListAllocator &) = delete;
    FreeListAllocator &operator=(const FreeListAllocator &) = delete;
};