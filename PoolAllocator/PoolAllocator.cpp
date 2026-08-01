#include <iostream>
#include <sys/mman.h>
#include <cstdint>
#include "PoolAllocator.h"
using namespace std;

PoolAllocator::PoolAllocator(size_t totalSize, size_t blockSize) : totalSize(totalSize), blockSize(blockSize), offset(0), start(nullptr), freeList(nullptr)
{
    if (blockSize % alignof(std::max_align_t) != 0)
    {
        this->blockSize += alignof(std::max_align_t) - (blockSize % alignof(std::max_align_t));
    }
    if (blockSize <= sizeof(FreeBlock) || blockSize > totalSize)
    {
        throw std::invalid_argument("Block size is invalid");
    }
    start = static_cast<char *>(mmap(nullptr, totalSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
    if (start == MAP_FAILED)
    {
        cout << "Memory allocation failed" << endl;
        throw std::bad_alloc();
    }
};

void *PoolAllocator::Allocate()
{
    if (freeList == nullptr)
    {
        char *address = start + offset * blockSize;
        if (address + blockSize > start + totalSize)
        {
            cout << "Insufficient memory" << endl;
            return nullptr;
        }
        ++offset;
        return address;
    }

    FreeBlock *block = freeList;
    freeList = freeList->next;
    return reinterpret_cast<void *>(block);
};

void PoolAllocator::Deallocate(void *ptr)
{
    FreeBlock *block = reinterpret_cast<FreeBlock *>(ptr);
    block->next = freeList;
    freeList = block;
};

PoolAllocator::~PoolAllocator()
{
    if (start != nullptr)
    {
        munmap(start, totalSize);
    }
};
