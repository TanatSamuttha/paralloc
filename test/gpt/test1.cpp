#include <iostream>
#include <vector>
#include <random>
#include <unordered_set>
#include <cassert>

#include "paralloc.h"

struct A { char data[8]; };
struct B { char data[16]; };
struct C { char data[32]; };
struct D { char data[64]; };

struct Node{
    void* ptr;
    int type;
    uint64_t magic;
};

int main(){
    Paralloc alloc;

    std::mt19937 rng(123456);
    std::uniform_int_distribution<int> opDist(0, 1);
    std::uniform_int_distribution<int> typeDist(0, 3);

    std::vector<Node> alive;

    constexpr int OPERATIONS = 1000000;

    for(int step = 0; step < OPERATIONS; step++){

        bool doAlloc = alive.empty() || opDist(rng);

        if(doAlloc){

            int type = typeDist(rng);

            Node node;
            node.type = type;
            node.magic = rng();

            switch(type){
                case 0:{
                    A* p = alloc.alloc<A>();
                    *(uint64_t*)p = node.magic;
                    node.ptr = p;
                    break;
                }
                case 1:{
                    B* p = alloc.alloc<B>();
                    *(uint64_t*)p = node.magic;
                    node.ptr = p;
                    break;
                }
                case 2:{
                    C* p = alloc.alloc<C>();
                    *(uint64_t*)p = node.magic;
                    node.ptr = p;
                    break;
                }
                case 3:{
                    D* p = alloc.alloc<D>();
                    *(uint64_t*)p = node.magic;
                    node.ptr = p;
                    break;
                }
            }

            alive.push_back(node);
        }
        else{

            std::uniform_int_distribution<size_t>
                victimDist(0, alive.size() - 1);

            size_t idx = victimDist(rng);

            Node node = alive[idx];

            uint64_t stored =
                *(uint64_t*)node.ptr;

            if(stored != node.magic){
                std::cout
                    << "CORRUPTION DETECTED\n"
                    << "expected = "
                    << node.magic
                    << "\nactual   = "
                    << stored
                    << "\nstep     = "
                    << step
                    << '\n';

                return 1;
            }

            switch(node.type){
                case 0:
                    alloc.free((A*)node.ptr);
                    break;

                case 1:
                    alloc.free((B*)node.ptr);
                    break;

                case 2:
                    alloc.free((C*)node.ptr);
                    break;

                case 3:
                    alloc.free((D*)node.ptr);
                    break;
            }

            alive[idx] = alive.back();
            alive.pop_back();
        }

        if(step % 10000 == 0){

            for(auto& node : alive){

                uint64_t stored =
                    *(uint64_t*)node.ptr;

                if(stored != node.magic){

                    std::cout
                        << "LIVE OBJECT CORRUPTED\n"
                        << "step = "
                        << step
                        << '\n';

                    return 1;
                }
            }
        }
    }

    std::cout << "PASS\n";
}