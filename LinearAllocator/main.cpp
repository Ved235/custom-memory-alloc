#include <iostream>
#include "LinearAllocator.h"
using namespace std;
int main()
{
    size_t size = 128;
    LinearAllocator allocator(size);

    cout << "New allocated memory: " << static_cast<void *>(allocator.Allocate(17)) << endl;
    cout << "New allocated memory: " << static_cast<void *>(allocator.Allocate(8)) << endl;
    cout << "Allocated size: " << allocator.GetAllocatedSize() << " bytes" << endl;
    allocator.Reset();
    return 0;
}