#include <iostream>
#include <sys/mman.h>
#include <cstdint>
#include "LinearAllocator.h"
using namespace std;

LinearAllocator::LinearAllocator(size_t size) : totalSize(size)
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

void *LinearAllocator::Allocate(size_t bytes)
{
    constexpr size_t alignment = alignof(std::max_align_t);
    uintptr_t currentAddress = reinterpret_cast<uintptr_t>(current);
    int remainder = currentAddress % alignment;
    uintptr_t alignedAddress;
    if (remainder != 0)
    {
        alignedAddress = currentAddress + (alignment - remainder);
    }
    else
    {
        alignedAddress = currentAddress;
    }
    char *alignedPtr = reinterpret_cast<char *>(alignedAddress);
    if (alignedPtr + bytes > end)
    {
        cout << "Insufficient memory" << endl;
        return nullptr;
    }
    current = alignedPtr + bytes;
    return alignedPtr;
}

size_t LinearAllocator::GetAllocatedSize() const
{
    return current - start;
}

void LinearAllocator::Reset()
{
    current = start;
}

LinearAllocator::~LinearAllocator()
{
    if (start != nullptr)
    {
        munmap(start, end - start);
    }
}
