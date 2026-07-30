#include <iostream>
#include <sys/mman.h>
#include <cstdint>
#include "PoolAllocator.h"
using namespace std;

PoolAllocator::PoolAllocator(size_t totalSize, size_t blockSize) : totalSize(totalSize), blockSize(blockSize), start(nullptr), freeList(nullptr)
{
    if(blockSize <= sizeof(FreeBlock))
    {
        cout << "Block size must be greater than " << sizeof(FreeBlock) << " bytes" << endl;
        throw std::invalid_argument("Block size too small");
    }
    if (blockSize % alignof(std::max_align_t) != 0)
    {
        this->blockSize += alignof(std::max_align_t) - (blockSize % alignof(std::max_align_t));
        cout << "Block size adjusted to " << this->blockSize << " for alignment" << endl;
    }
    start = static_cast<char *>(mmap(nullptr, totalSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
    if (start == MAP_FAILED)
    {
        cout << "Memory allocation failed" << endl;
        throw std::bad_alloc();
    }
    freeList = reinterpret_cast<FreeBlock *>(start);
    size_t numBlocks = totalSize / this->blockSize;
    for (size_t i = 0; i < numBlocks; ++i)
    {
        FreeBlock *block = reinterpret_cast<FreeBlock *>(start + i * this->blockSize);
        if (i == numBlocks - 1)
        {
            block->next = nullptr;
        }
        else
        {
            block->next = reinterpret_cast<FreeBlock *>(start + (i + 1) * this->blockSize);
        }
    }
};

void *PoolAllocator::Allocate()
{
    if (freeList == nullptr)
    {
        cout << "Insufficient memory" << endl;
        return nullptr;
    }
    FreeBlock *block = freeList;
    freeList = freeList->next;
    return block;
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
