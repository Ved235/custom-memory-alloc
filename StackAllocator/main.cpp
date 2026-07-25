#include <iostream>
#include "StackAllocator.h"
using namespace std;

int main(){
    size_t size = 128; 
    StackAllocator allocator(size);
    
    void* ptr1 = allocator.Allocate(17);
    cout << "New allocated memory: " << ptr1 << endl;
    void* ptr2 = allocator.Allocate(8);
    cout << "New allocated memory: " << ptr2 << endl;
    cout << "Allocated size: " << allocator.GetAllocatedSize() << " bytes" << endl;

    allocator.Free(ptr2);
    cout << "Freed memory at: " << ptr2 << endl;
    cout << "Allocated size after free: " << allocator.GetAllocatedSize() << " bytes" << endl;

    allocator.Free(ptr1);
    cout << "Freed memory at: " << ptr1 << endl;
    cout << "Allocated size after free: " << allocator.GetAllocatedSize() << " bytes" << endl;

    allocator.Reset();
    return 0;
}