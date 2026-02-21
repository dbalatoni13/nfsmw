#ifndef EAGL4ANIM_EAGL4SUPPORTDEF_H
#define EAGL4ANIM_EAGL4SUPPORTDEF_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

typedef void *(*DynamicUserCallback)(const char *, bool &);

namespace EAGL4Internal {

typedef void *(*MallocFunctionType)(size_t, const char *);
typedef void (*FreeFunctionType)(void *, size_t);
typedef void *(*NewFunctionType)(size_t, const char *);
typedef void (*DeleteFunctionType)(void *, size_t);

extern MallocFunctionType EAGL4Malloc;
extern FreeFunctionType EAGL4Free;

// total size: 0x4
struct SymbolEntry {
    // void *operator new(size_t size) {}

    // void *operator new(size_t size, const char *msg) {}

    // void operator delete(void *ptr, size_t size) {}

    // void *operator new[](size_t size) {}

    // void *operator new[](size_t size, const char *msg) {}

    // void operator delete[](void *ptr, size_t size) {}

    // void *operator new(size_t, void *ptr) {}

    void *Address; // offset 0x0, size 0x4
};

// total size: 0xC
struct FunctionEntry {
    // void *operator new(size_t size) {}

    // void *operator new(size_t size, const char *msg) {}

    // void operator delete(void *ptr, size_t size) {}

    // void *operator new[](size_t size) {}

    // void *operator new[](size_t size, const char *msg) {}

    // void operator delete[](void *ptr, size_t size) {}

    // void *operator new(size_t, void *ptr) {}

    DynamicUserCallback Address; // offset 0x0, size 0x4
    FunctionEntry *Prev;         // offset 0x4, size 0x4
    FunctionEntry *Next;         // offset 0x8, size 0x4
};

}; // namespace EAGL4Internal

#endif
