#include <iostream>
#include <sys/mman.h>
#include <cstddef>
#include "FreeListAllocator.h"
using namespace std;

FreeListAllocator::FreeListAllocator(size_t size) : freeList(nullptr), totalSize(size), start(nullptr)
{
    start = static_cast<char *>(mmap(nullptr, totalSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
    if (start == MAP_FAILED)
    {
        cout << "Memory allocation failed" << endl;
        throw std::bad_alloc();
    }
    freeList = reinterpret_cast<FreeBlock *>(start);
    freeList->size = totalSize;
    freeList->next = nullptr;
    //cout << "FreeListAllocator started at: " << static_cast<void *>(start) << endl;
}

void *FreeListAllocator::Allocate(size_t size)
{
    constexpr size_t alignment = alignof(std::max_align_t);
    FreeBlock *it = freeList;
    FreeBlock *prev = nullptr;
    size = (size + alignment - 1) & ~(alignment - 1); // Align size to the nearest multiple of alignment
    while (it != nullptr)
    {
        uintptr_t blockAddress = reinterpret_cast<uintptr_t>(it) + sizeof(AllocatedBlock);
        uintptr_t remainder = blockAddress % alignment;
        uintptr_t alignedAddress;
        if (remainder != 0)
        {
            alignedAddress = blockAddress + (alignment - remainder);
        }
        else
        {
            alignedAddress = blockAddress;
        }
        size_t consumed = (alignedAddress - reinterpret_cast<uintptr_t>(it)) + size;
        if (it->size >= consumed)
        {
            size_t remaining = it->size - consumed;
            if (remaining >= sizeof(AllocatedBlock) + alignment)
            {
                AllocatedBlock *header = reinterpret_cast<AllocatedBlock *>(alignedAddress - sizeof(AllocatedBlock));
                header->size = consumed;
                header->padding = alignedAddress - blockAddress;
                FreeBlock *newBlock = reinterpret_cast<FreeBlock *>(alignedAddress + size);
                newBlock->size = remaining;
                newBlock->next = it->next;

                if (!prev)
                {
                    freeList = newBlock;
                }
                else
                {
                    prev->next = newBlock;
                }
                return reinterpret_cast<void *>(alignedAddress);
            }
            else
            {
                AllocatedBlock *header = reinterpret_cast<AllocatedBlock *>(alignedAddress - sizeof(AllocatedBlock));
                header->size = it->size;
                header->padding = alignedAddress - blockAddress;
                if (!prev)
                {
                    freeList = it->next;
                }
                else
                {
                    prev->next = it->next;
                }
                return reinterpret_cast<void *>(alignedAddress);
            }
        }
        prev = it;
        it = it->next;
    }
    cout << "Insufficient memory" << endl;
    throw std::bad_alloc();
}

void FreeListAllocator::Free(void *ptr)
{
    if (ptr == nullptr)
        return;
    AllocatedBlock *temp = reinterpret_cast<AllocatedBlock *>(reinterpret_cast<char *>(ptr) - sizeof(AllocatedBlock));
    FreeBlock *blockToFree = reinterpret_cast<FreeBlock *>(reinterpret_cast<char *>(temp) - temp->padding);
    blockToFree->size = temp->size;
    FreeBlock *prev = nullptr;
    FreeBlock *curr = freeList;
    while (curr && reinterpret_cast<uintptr_t>(curr) < reinterpret_cast<uintptr_t>(blockToFree))
    {
        prev = curr;
        curr = curr->next;
    }
    blockToFree->next = curr;
    if (prev)
    {
        prev->next = blockToFree;
    }
    else
    {
        freeList = blockToFree;
    }

    uintptr_t end = reinterpret_cast<uintptr_t>(blockToFree) + blockToFree->size;
    if (curr && reinterpret_cast<uintptr_t>(curr) == end)
    {
        blockToFree->size += curr->size;
        blockToFree->next = curr->next;
    }

    if (prev && reinterpret_cast<uintptr_t>(prev) + prev->size == reinterpret_cast<uintptr_t>(blockToFree))
    {
        prev->size += blockToFree->size;
        prev->next = blockToFree->next;
    }
}

FreeListAllocator::~FreeListAllocator()
{
    if (start)
    {
        munmap(start, totalSize);
    }
}