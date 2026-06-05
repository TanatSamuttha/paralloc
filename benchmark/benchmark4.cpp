#include <iostream>
#include <chrono>
#include <vector>
#include <iomanip>
#include "paralloc.h"

struct InputData    { uint64_t buttons; };                                     
struct Bullet       { float x, y, z, w; };                                     
struct Player       { char name[16]; int hp; int mp; float speed; };           
struct HeavyPayload { char chatMessage[128]; };                               

const int ITERATIONS = 10'000'000; 

inline uint32_t fast_rand(uint32_t& seed) {
    seed = seed * 1664525 + 1013904223;
    return seed;
}

double benchmarkMalloc() {
    uint32_t seed = 42;
    auto start = std::chrono::high_resolution_clock::now();

    Player* activePlayers[16] = {nullptr};
    Bullet* activeBullets[64] = {nullptr};

    for (int i = 0; i < ITERATIONS; ++i) {
        InputData* in1 = static_cast<InputData*>(std::malloc(sizeof(InputData)));
        InputData* in2 = static_cast<InputData*>(std::malloc(sizeof(InputData)));
        std::free(in1); std::free(in2);

        uint32_t pIdx = fast_rand(seed) % 16;
        if (activePlayers[pIdx] && (fast_rand(seed) % 100 < 20)) { 
            std::free(activePlayers[pIdx]);
            activePlayers[pIdx] = nullptr;
        } else if (!activePlayers[pIdx]) {
            activePlayers[pIdx] = static_cast<Player*>(std::malloc(sizeof(Player)));
        }

        uint32_t bIdx = fast_rand(seed) % 64;
        if (activeBullets[bIdx] && (fast_rand(seed) % 100 < 40)) { 
            std::free(activeBullets[bIdx]);
            activeBullets[bIdx] = nullptr;
        } else if (!activeBullets[bIdx]) {
            activeBullets[bIdx] = static_cast<Bullet*>(std::malloc(sizeof(Bullet)));
        }

        if (fast_rand(seed) % 100 < 1) {
            HeavyPayload* msg = static_cast<HeavyPayload*>(std::malloc(sizeof(HeavyPayload)));
            std::free(msg);
        }
    }

    for (int i = 0; i < 16; ++i) if (activePlayers[i]) std::free(activePlayers[i]);
    for (int i = 0; i < 64; ++i) if (activeBullets[i]) std::free(activeBullets[i]);

    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

double benchmarkParalloc() {
    uint32_t seed = 42;
    
    // 💡 ใช้ข้อดีของ Object-Based: แยก Pool ตามหน้าที่และประเภทของข้อมูล
    Paralloc inputPool;
    Paralloc playerPool;
    Paralloc bulletPool;
    Paralloc heavyPool; 

    auto start = std::chrono::high_resolution_clock::now();

    Player* activePlayers[16] = {nullptr};
    Bullet* activeBullets[64] = {nullptr};

    for (int i = 0; i < ITERATIONS; ++i) {
        // 1. จัดการ InputData ผ่าน inputPool
        InputData* in1 = inputPool.galloc<InputData>();
        InputData* in2 = inputPool.galloc<InputData>();
        inputPool.free(in1); inputPool.free(in2);

        // 2. จัดการ Player ผ่าน playerPool
        uint32_t pIdx = fast_rand(seed) % 16;
        if (activePlayers[pIdx] && (fast_rand(seed) % 100 < 20)) {
            playerPool.free(activePlayers[pIdx]);
            activePlayers[pIdx] = nullptr;
        } else if (!activePlayers[pIdx]) {
            activePlayers[pIdx] = playerPool.galloc<Player>();
        }

        // 3. จัดการ Bullet ผ่าน bulletPool
        uint32_t bIdx = fast_rand(seed) % 64;
        if (activeBullets[bIdx] && (fast_rand(seed) % 100 < 40)) {
            bulletPool.free(activeBullets[bIdx]);
            activeBullets[bIdx] = nullptr;
        } else if (!activeBullets[bIdx]) {
            activeBullets[bIdx] = bulletPool.galloc<Bullet>();
        }

        // 4. จัดการ HeavyPayload ผ่าน heavyPool
        if (fast_rand(seed) % 100 < 1) {
            HeavyPayload* msg = heavyPool.galloc<HeavyPayload>();
            heavyPool.free(msg);
        }
    }

    // 💡 ไม่ต้องลูปสั่ง pool.free() คืนค่าทีละตัวตอนท้ายฟังก์ชันเหมือนโค้ดเก่า!
    // เพราะทันทีที่จบฟังก์ชัน Destructor ของแต่ละ Pool จะคืนค่าหน่วยความจำยกก้อน (4096 bytes) ทันทีอยู่แล้ว

    auto end = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double, std::milli>(end - start).count();

    // รวมสถิติการใช้งานจากทุก Pool
    double totalParallocUsed = inputPool.parallocUsed + playerPool.parallocUsed + bulletPool.parallocUsed + heavyPool.parallocUsed;
    double totalMallocUsed = inputPool.mallocUsed + playerPool.mallocUsed + bulletPool.mallocUsed + heavyPool.mallocUsed;
    double totalAllocs = totalParallocUsed + totalMallocUsed;
    
    std::cout << "[Paralloc Pools ]\n";
    std::cout << "  -> Pool Success      : " << totalParallocUsed << " times (" << (totalParallocUsed / totalAllocs) * 100.0 << "%)\n";
    std::cout << "  -> Fallback (Malloc) : " << totalMallocUsed << " times (" << (totalMallocUsed / totalAllocs) * 100.0 << "%)\n";

    return elapsed;
}

int main() {
    std::cout << "Running Clean $O(1)$ Benchmark (" << ITERATIONS << " iterations)...\n";
    
    // อุ่นเครื่อง CPU 
    benchmarkMalloc(); 
    
    double tMalloc = benchmarkMalloc();
    std::cout << "[Standard Malloc] Time: " << std::fixed << std::setprecision(2) << tMalloc << " ms\n";

    double tParalloc = benchmarkParalloc();
    std::cout << "Time: " << std::fixed << std::setprecision(2) << tParalloc << " ms\n";
    
    std::cout << "----------------------------------------------------------------\n";
    std::cout << "[SUMMARY] Paralloc is " << (tMalloc / tParalloc) << "x faster than Standard Malloc!\n";
    return 0;
}