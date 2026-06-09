# palalloc
## pool-adaptive-linking memory-allocator

This project is under developping but it is mostly complete and ready to use. Palalloc is memory allocator that design to do every method in O1 time complexity trade off by lower data size flexibility than std::malloc.

## How to use
1. Add palalloc.h into your project folder and include it.
2. Create object in class of Palalloc and assign pages and maxsize into parameters (1 page size is 4096 bytes. Maxsize can't lower than 8 bytes and can't higher than (pages * 4096) / 2. And maxsize can only divisible by 2 but you don't have to worry because the program will automatically find the smallest size that still fit for your maxsize and divisible by 2).
