#include <iostream>
#include <sys/mman.h>
#include <cstdint>
#include "StackAllocator.h"
using namespace std;

StackAllocator::StackAllocator(size_t size) : start(nullptr), current(nullptr), end(nullptr), totalSize(size)
{
    start = static_cast<char *>(mmap(nullptr, totalSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
    if (start == MAP_FAILED)
    {
        cout << "Memory allocation failed" << endl;
        throw std::bad_alloc();
    }
    current = start;
    end = start + totalSize;
}

void *StackAllocator::Allocate(size_t bytes)
{
    constexpr size_t alignment = alignof(std::max_align_t);
    uintptr_t afterHeaderAddress = reinterpret_cast<uintptr_t>(current) + sizeof(Header);
    char *alignedPtr = reinterpret_cast<char *>((afterHeaderAddress + alignment - 1) & ~(alignment - 1));

    if (alignedPtr + bytes > end)
    {
        cout << "Insufficient memory" << endl;
        throw std::bad_alloc();
    }

    Header *header = reinterpret_cast<Header *>(alignedPtr - sizeof(Header));
    header->previous = current;

    current = alignedPtr + bytes;
    return alignedPtr;
}

void StackAllocator::Free(void *ptr)
{
    if (current == start)
    {
        cout << "No memory to free" << endl;
        return;
    }

    Header *header = reinterpret_cast<Header *>(reinterpret_cast<char *>(ptr) - sizeof(Header));
    current = header->previous;
}

void StackAllocator::Reset()
{
    current = start;
}

size_t StackAllocator::GetAllocatedSize() const
{
    return current - start;
}

StackAllocator::~StackAllocator()
{
    if (start != nullptr)
    {
        munmap(start, end - start);
    }
}
