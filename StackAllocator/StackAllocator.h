#pragma once
#include <cstddef>

class StackAllocator {
    private:
        struct Header {
            char* previous;
        };

        char* start;
        char* current;
        char* end;
        size_t totalSize;

    public:
        StackAllocator(size_t size);
        void* Allocate(size_t bytes);
        void Free(void* ptr);
        size_t GetAllocatedSize() const;
        void Reset();
        StackAllocator(const StackAllocator&) = delete;
        StackAllocator& operator=(const StackAllocator&) = delete;
        ~StackAllocator();
};