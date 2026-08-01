#include <iostream>
#include <chrono>
#include "FreeListAllocator/FreeListAllocator.h"
#include "LinearAllocator/LinearAllocator.h"
#include "StackAllocator/StackAllocator.h"
#include "PoolAllocator/PoolAllocator.h"

using namespace std;
using Clock = chrono::steady_clock;

constexpr size_t allocSize = 1024 * 1024 * 30;
constexpr int trials = 20000;
constexpr int allocationSize = 1024;

template <typename Func>
void benchmark(Func &&func, const string &name)
{
    auto start = Clock::now();
    func();
    auto end = Clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start).count();
    cout << name << " took " << duration << " microseconds." << endl;
};

void FreeListAllocatorBenchmark()
{
    FreeListAllocator allocator(allocSize);
    volatile void *sink;
    void *pointers[trials];

    for (int i = 0; i < trials; ++i)
    {
        pointers[i] = allocator.Allocate(allocationSize);
        sink = pointers[i]; // Prevent optimization
    }

    for (int i = 0; i < trials; ++i)
    {
        allocator.Free(pointers[i]);
    }
}

void LinearAllocatorBenchmark()
{
    LinearAllocator allocator(allocSize);
    volatile void *sink;
    void *pointers[trials];

    for (int i = 0; i < trials; ++i)
    {
        pointers[i] = allocator.Allocate(allocationSize);
        sink = pointers[i];
    }

    allocator.Reset();
}

void StandardAllocatorBenchmark()
{
    volatile void *sink;
    void *pointers[trials];

    for (int i = 0; i < trials; ++i)
    {
        pointers[i] = ::operator new(allocationSize);
        sink = pointers[i];
    }

    for (int i = 0; i < trials; ++i)
    {
        ::operator delete(pointers[i]);
    }
}

void StackAllocatorBenchmark()
{
    StackAllocator allocator(allocSize);
    volatile void *sink;
    void *pointers[trials];

    for (int i = 0; i < trials; ++i)
    {
        pointers[i] = allocator.Allocate(allocationSize);
        sink = pointers[i]; // Prevent optimization
    }

    for (int i = trials - 1; i >= 0; --i)
    {
        allocator.Free(pointers[i]);
    }
}

void PoolAllocatorBenchmark()
{
    PoolAllocator allocator(allocSize, allocationSize);
    volatile void *sink;
    void *pointers[trials];

    for (int i = 0; i < trials; ++i)
    {
        pointers[i] = allocator.Allocate();
        sink = pointers[i]; // Prevent optimization
    }

    for (int i = 0; i < trials; ++i)
    {
        allocator.Deallocate(pointers[i]);
    }
}

int main()
{
    benchmark(StandardAllocatorBenchmark, "StandardAllocator Benchmark");
    benchmark(FreeListAllocatorBenchmark, "FreeListAllocator Benchmark");
    benchmark(LinearAllocatorBenchmark, "LinearAllocator Benchmark");
    benchmark(StackAllocatorBenchmark, "StackAllocator Benchmark");
    benchmark(PoolAllocatorBenchmark, "PoolAllocator Benchmark");
}
