#pragma once
#include <cstddef>

class PoolAllocator
{
private:
    size_t totalSize;
    size_t blockSize;
    size_t offset;
    struct FreeBlock
    {
        FreeBlock *next;
    };
    char *start;
    FreeBlock *freeList;

public:
    PoolAllocator(size_t totalSize, size_t blockSize);
    void *Allocate();
    void Deallocate(void *ptr);
    PoolAllocator(const PoolAllocator &) = delete;
    PoolAllocator &operator=(const PoolAllocator &) = delete;
    ~PoolAllocator();
};