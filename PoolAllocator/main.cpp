#include <iostream>
#include "PoolAllocator.h"
using namespace std;

int main(){
    PoolAllocator pool(1024, 30);
    void* ptr1 = pool.Allocate();
    void* ptr2 = pool.Allocate();
    cout << ptr1 << " " << ptr2 << endl;
    cout << "Allocated two blocks" << endl;
    pool.Deallocate(ptr1);
    cout << "Deallocated one block" << endl;
    pool.Deallocate(ptr2);
    cout << "Deallocated another block" << endl;
    void* ptr3 = pool.Allocate();
    cout << ptr3 << endl;
    cout << "Allocated another block" << endl;
    return 0;
}