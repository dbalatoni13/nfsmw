#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include <cstddef>

void *operator new(size_t size) {
#ifdef MILESTONE_BUILD
    // TODO
    return bWareMalloc(size, nullptr, 0, 0);
#else
    return bMalloc(size, 0);
#endif
}

void *operator new[](size_t size) {
#ifdef MILESTONE_BUILD
    // TODO
    return bWareMalloc(size, nullptr, 0, 0);
#else
    return bMalloc(size, 0);
#endif
}

// STRIPPED
void *operator new(size_t size, int allocation_params) {}

// STRIPPED
void *operator new[](size_t size, int allocation_params) {}

void operator delete(void *ptr) {
    bFree(ptr);
}

void operator delete[](void *ptr) {
    bFree(ptr);
}

// STRIPPED
void operator delete(void *ptr, int allocation_params) {}

// STRIPPED
void operator delete(void *ptr, const char *file, int line) {}

// STRIPPED
void operator delete[](void *ptr, const char *file, int line) {}

// STRIPPED
void operator delete(void *ptr, const char *file, int line, int allocation_params) {}

// STRIPPED
void operator delete[](void *ptr, const char *file, int line, int allocation_params) {}
