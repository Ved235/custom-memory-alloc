#pragma once
#include <cstddef>

class LinearAllocator
{
private:
    char *start;
    char *current;
    char *end;
    size_t totalSize;

public:
    LinearAllocator(size_t size);
    void *Allocate(size_t bytes);
    size_t GetAllocatedSize() const;
    void Reset();
    LinearAllocator(const LinearAllocator &) = delete;
    LinearAllocator &operator=(const LinearAllocator &) = delete;
    ~LinearAllocator();
};