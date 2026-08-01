# Custom Memory Allocators

This repository contains implementations of various custom memory allocators, including:

- **Linear Allocator** — Bump pointer allocation with bulk reset
- **Stack Allocator** — LIFO push/pop style allocation
- **Pool Allocator** — Fixed size block reuse
- **FreeList Allocator** — First-fit with variable size allocation and coalescing. Implemented using linked list
- **Standard Allocator** — Baseline using `new`/`delete`

## Benchmark

A custom benchmark is included to compare allocator throughput. The benchmark performs the following sequence:

1. **Allocate** 10,000 blocks with random sizes between 16–1000 bytes
2. **Deallocate** ~25% of the blocks at random indices
3. **Re-allocate** another 10,000 blocks
4. **Cleanup** remaining allocated blocks

Memory is touched after each allocation to force OS page commits. Results are reported in **MOPS** (Million Operations Per Second).

## Results

| Allocator   | MOPS   | Time (µs) | Total Ops |
|-------------|--------|-----------|-----------|
| Pool (64B)  | 121.79 | 246.33    | 30,000    |
| Linear      | 68.34  | 292.67    | 20,002    |
| Stack       | 34.28  | 656.33    | 22,501    |
| Standard    | 15.81  | 1897.33   | 30,000    |
| FreeList    | 1.02   | 29432.67  | 30,000    |

*Note: Pool uses 64-byte fixed blocks. Linear and Stack workloads include bulk `Reset()` operations rather than individual deallocations*