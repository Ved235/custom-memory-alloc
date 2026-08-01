#include <iostream>
#include <chrono>
#include <vector>
#include <random>
#include <iomanip>
#include <algorithm>
#include "FreeListAllocator/FreeListAllocator.h"
#include "LinearAllocator/LinearAllocator.h"
#include "StackAllocator/StackAllocator.h"
#include "PoolAllocator/PoolAllocator.h"

using Clock = std::chrono::steady_clock;

constexpr size_t N = 10000;
constexpr size_t MIN_SIZE = 16;
constexpr size_t MAX_SIZE = 1000;
constexpr size_t MEMORY_SIZE = 128 * 1024 * 1024;
constexpr unsigned RNG_SEED = 0xABCD1234;

std::vector<size_t> allocSizes;
std::vector<size_t> deallocIndices;

inline void touch(void *p)
{
    if (!p)
        return;
    volatile char *c = static_cast<volatile char *>(p);
    c[0] = 1;
}

template <typename Alloc>
uint64_t workload_mixed(Alloc &alloc)
{
    std::vector<void *> ptrs(2 * N);
    uint64_t ops = 0;

    for (size_t i = 0; i < N; ++i)
    {
        ptrs[i] = alloc.Allocate(allocSizes[i]);
        touch(ptrs[i]);
        ++ops;
    }

    for (size_t idx : deallocIndices)
    {
        alloc.Free(ptrs[idx]);
        ptrs[idx] = nullptr;
        ++ops;
    }

    for (size_t i = 0; i < N; ++i)
    {
        size_t sz = allocSizes[N + i];
        ptrs[N + i] = alloc.Allocate(sz);
        touch(ptrs[N + i]);
        ++ops;
    }

    for (auto *p : ptrs)
    {
        if (p)
        {
            alloc.Free(p);
            ++ops;
        }
    }
    return ops;
}

uint64_t workload_linear(LinearAllocator &alloc)
{
    uint64_t ops = 0;

    for (size_t i = 0; i < N; ++i)
    {
        touch(alloc.Allocate(allocSizes[i]));
        ++ops;
    }

    alloc.Reset();
    ++ops;

    for (size_t i = 0; i < N; ++i)
    {
        touch(alloc.Allocate(allocSizes[N + i]));
        ++ops;
    }

    alloc.Reset();
    ++ops;
    return ops;
}

uint64_t workload_stack(StackAllocator &alloc)
{
    std::vector<void *> stack;
    stack.reserve(2 * N);
    uint64_t ops = 0;

    for (size_t i = 0; i < N; ++i)
    {
        void *p = alloc.Allocate(allocSizes[i]);
        touch(p);
        stack.push_back(p);
        ++ops;
    }

    for (size_t i = 0; i < N / 4; ++i)
    {
        alloc.Free(stack.back());
        stack.pop_back();
        ++ops;
    }

    for (size_t i = 0; i < N; ++i)
    {
        void *p = alloc.Allocate(allocSizes[N + i]);
        touch(p);
        ++ops;
    }

    alloc.Reset();
    ++ops;
    return ops;
}

uint64_t workload_pool(PoolAllocator &alloc)
{
    std::vector<void *> ptrs(2 * N, nullptr);
    uint64_t ops = 0;

    for (size_t i = 0; i < N; ++i)
    {
        ptrs[i] = alloc.Allocate();
        touch(ptrs[i]);
        ++ops;
    }

    for (size_t idx : deallocIndices)
    {
        alloc.Deallocate(ptrs[idx]);
        ptrs[idx] = nullptr;
        ++ops;
    }

    for (size_t i = 0; i < N; ++i)
    {
        ptrs[N + i] = alloc.Allocate();
        touch(ptrs[N + i]);
        ++ops;
    }

    for (auto *p : ptrs)
    {
        if (p)
        {
            alloc.Deallocate(p);
            ++ops;
        }
    }
    return ops;
}

struct Result
{
    std::string name;
    double mops;
    double time;
    uint64_t ops;
};

template <typename Func>
Result bench(const char *name, Func &&fn, int iters = 3)
{
    std::vector<double> times;
    uint64_t ops = 0;

    for (int i = 0; i < iters; ++i)
    {
        auto start = Clock::now();
        ops = fn();
        auto end = Clock::now();
        times.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());
    }

    double avg = std::accumulate(times.begin(), times.end(), 0.0) / times.size();

    return {name, (ops / avg), avg, ops};
}

int main()
{
    std::mt19937_64 rng(RNG_SEED);
    std::uniform_int_distribution<size_t> dist(MIN_SIZE, MAX_SIZE);

    allocSizes.resize(N * 2);
    for (auto &s : allocSizes)
        s = dist(rng);

    std::vector<size_t> idx(N);
    std::iota(idx.begin(), idx.end(), 0);
    std::shuffle(idx.begin(), idx.end(), rng);
    deallocIndices.assign(idx.begin(), idx.begin() + N / 4);

    std::cout << std::fixed << std::setprecision(2);

    auto r1 = bench("Standard", []() -> uint64_t
                    {
        struct Std { void* Allocate(size_t s) { return ::operator new(s); } void Free(void* p) { ::operator delete(p); } };
        Std a; return workload_mixed(a); });

    auto r2 = bench("FreeList", []() -> uint64_t
                    {
        FreeListAllocator a(MEMORY_SIZE);
        return workload_mixed(a); });

    auto r3 = bench("Linear", []() -> uint64_t
                    {
        LinearAllocator a(MEMORY_SIZE);
        return workload_linear(a); });

    auto r4 = bench("Stack", []() -> uint64_t
                    {
        StackAllocator a(MEMORY_SIZE);
        return workload_stack(a); });

    auto r5 = bench("Pool (64B)", []() -> uint64_t
                    {
        PoolAllocator a(MEMORY_SIZE, 64);
        return workload_pool(a); });

    auto print = [](const Result &r)
    {
        std::cout << std::left << std::setw(20) << r.name
                  << " MOPS: " << std::setw(8) << r.mops
                  << " Time: " << std::setw(8) << r.time
                  << " Ops: " << r.ops << "\n";
    };

    print(r1);
    print(r2);
    print(r3);
    print(r4);
    print(r5);
}