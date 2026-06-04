#include <iostream>
#include <chrono>
#include <vector>
#include "paralloc.h"

// === ใส่ Code คลาส Paralloc ของคุณไว้ตรงนี้ ===
// (สมมติว่าคลาส Paralloc ถูกประกาศไว้ข้างบนนี้แล้ว)

// เคสสมมติในโลกจริง: ออบเจกต์ Particle หรือ Node ข้อมูลขนาด 32 bytes 
struct Particle {
    float position[4]; // 16 bytes
    float velocity[4]; // 16 bytes
};                     // Total = 32 bytes (ตรงกับ Index 2 ของ Paralloc)

const int TOTAL_ITERATIONS = 2'000'000; // จำนวนรอบจำลอง (เช่น 2 ล้านเฟรม)
const int BATCH_SIZE = 12;             // จำนวนชิ้นต่อรอบ (ต้องไม่เกินพูลขนาด 16 ชิ้นของไซส์ 32)

void benchmarkMalloc() {
    auto start = std::chrono::high_resolution_clock::now();
    
    // จำลองการทำงานลูปขนาดใหญ่
    for (int i = 0; i < TOTAL_ITERATIONS; ++i) {
        Particle* activeParticles[BATCH_SIZE];

        // 1. Alloc ชิ้นงานเล็กๆ ขึ้นมาในเฟรมนี้
        for (int j = 0; j < BATCH_SIZE; ++j) {
            activeParticles[j] = static_cast<Particle*>(std::malloc(sizeof(Particle)));
        }

        // 2. ทำงานกับออบเจกต์ชั่วคราว (Simulate Work)
        for (int j = 0; j < BATCH_SIZE; ++j) {
            activeParticles[j]->position[0] = 1.0f; 
        }

        // 3. เคลียร์เมมโมรี่เมื่อจบเฟรม
        for (int j = 0; j < BATCH_SIZE; ++j) {
            std::free(activeParticles[j]);
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "[Malloc Allocator]   Time taken: " << elapsed.count() << " ms\n";
}

void benchmarkParalloc() {
    Paralloc pool;
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < TOTAL_ITERATIONS; ++i) {
        Particle* activeParticles[BATCH_SIZE];

        // 1. Alloc จาก Paralloc (ตรงเข้า Pool ชิ้นที่ว่างทันที)
        for (int j = 0; j < BATCH_SIZE; ++j) {
            activeParticles[j] = pool.galloc<Particle>();
        }

        // 2. ทำงานกับออบเจกต์ชั่วคราว (Simulate Work)
        for (int j = 0; j < BATCH_SIZE; ++j) {
            activeParticles[j]->position[0] = 1.0f;
        }

        // 3. คืนพูลให้เอาไปใช้ใหม่ในรอบถัดไป
        for (int j = 0; j < BATCH_SIZE; ++j) {
            pool.free(activeParticles[j]);
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "[Paralloc Allocator] Time taken: " << elapsed.count() << " ms\n";
    std::cout << "  -> parallocUsed: " << pool.parallocUsed << ", mallocUsed: " << pool.mallocUsed << "\n";
}

int main() {
    std::cout << "Starting Memory Allocation Benchmark (Object Size: " << sizeof(Particle) << " bytes)...\n";
    std::cout << "--------------------------------------------------------\n";
    
    // รันเทียบกัน
    benchmarkMalloc();
    benchmarkParalloc();
    
    return 0;
}