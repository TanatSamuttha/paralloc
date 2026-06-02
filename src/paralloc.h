#ifndef PARALLOC_H
#define PARALLOC_H

#include <cstdint>

namespace paralloc{
    extern void* buffer;
    extern int16_t map[4096];
    extern int16_t headOf2;

    void init();
    void connect(int8_t size);
}

#endif