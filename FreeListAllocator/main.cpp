#include <iostream>
#include "FreeListAllocator.h"
using namespace std;

int main(){
    FreeListAllocator allocator(80);
    auto* ptr = allocator.Allocate(20);
    cout << "Allocated 20 bytes at: " << static_cast<void*>(ptr) << endl;
    auto* ptr2 = allocator.Allocate(16);
    cout << "Allocated 16 bytes at: " << static_cast<void*>(ptr2) << endl;
    allocator.Free(ptr);
    allocator.Free(ptr2);
    return 0;
}