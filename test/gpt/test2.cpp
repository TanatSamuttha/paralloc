#include <iostream>
#include "paralloc.h"

struct A{
    char data[8];
};

int main(){

    Paralloc alloc;

    A* p1 = alloc.alloc<A>();
    std::cout << "p1 = " << (void*)p1 << '\n';

    A* p2 = alloc.alloc<A>();
    std::cout << "p2 = " << (void*)p2 << '\n';

    alloc.free(p1);
    alloc.free(p2);

    std::cout << "done\n";
}