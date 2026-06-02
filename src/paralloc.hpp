#include <cstdlib>
#include <cstdint>

namespace paralloc{

    void* buffer;

    int16_t map[4096];
    
    int16_t begin[17];
    int16_t end[17];

    inline void init(){
        buffer = malloc(4096);

        begin[2] = 0;
        end[2] = 2047;
        
        begin[4] = 2048;
        end[4] = 3071;

        begin[8] = 3072;
        end[8] = 3583;

        begin[16] = 3584;
        end[16] = 4095;

        connect(2);
        connect(4);
        connect(8);
        connect(16);
    }

    inline void connect(int8_t size){
        int idx = begin[size];
        int endIdx = end[size];
        while(idx < endIdx){
            map[idx] = idx + size;
            idx += size;
        }
    }

}