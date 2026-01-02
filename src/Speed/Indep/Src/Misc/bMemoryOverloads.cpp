#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include <cstddef>

void *operator new(size_t size) {
#if DEBUG
    // TODO
    return bWareMalloc(size, nullptr, 0, 0);
#else
    return bMalloc(size, 0);
#endif
}

void *operator new[](size_t size) {
#if DEBUG
    // TODO
    return bWareMalloc(size, nullptr, 0, 0);
#else
    return bMalloc(size, 0);
#endif
}

void operator delete(void *ptr) {
    bFree(ptr);
}

void operator delete[](void *ptr) {
    bFree(ptr);
}
