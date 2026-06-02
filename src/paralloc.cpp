#include <cstdlib>
#include <cstdint>

namespace paralloc{

    void* buffer;

    uint8_t map[4096];
    
    uint8_t begin[4];
    uint8_t end[4];

    inline void init(){
        buffer = malloc(4096);

        begin[0] = 0;
        end[0] = 2047;
        
        begin[1] = 2048;
        end[1] = 3071;

        begin[2] = 3072;
        end[2] = 3583;

        begin[3] = 3584;
        end[3] = 4095;

        connect(2);
        connect(4);
        connect(8);
        connect(16);
    }

    inline void connect(uint8_t size){
        int idx = begin[size];
        int endIdx = end[size];
        while(idx < endIdx){
            map[idx] = idx + size;
            idx += size;
        }
    }

    inline void* malloc(size_t size){
        int idx = __builtin_ctz(size) - 1;
    }
}