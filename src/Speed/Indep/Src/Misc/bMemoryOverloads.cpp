#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include <cstddef>

void *operator new(size_t size) {
    return bMalloc(size, 0);
}

void *operator new[](size_t size) {
    return bMalloc(size, 0);
}

void operator delete(void *ptr) {
    bFree(ptr);
}

void operator delete[](void *ptr) {
    bFree(ptr);
}
