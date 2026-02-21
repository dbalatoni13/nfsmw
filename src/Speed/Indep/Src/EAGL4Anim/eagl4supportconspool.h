#ifndef EAGL4ANIM_EAGL4SUPPORTCONSPOOL_H
#define EAGL4ANIM_EAGL4SUPPORTCONSPOOL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "eagl4supportsympool.h"

// TODO
namespace EAGL4 {
class DynamicLoader;
}

typedef void (*Constructor)(void *, class EAGL4::DynamicLoader *, const char *);
typedef void (*Destructor)(void *);

// TODO the namespaces of these are wrong
// total size: 0x8
struct DestructorEntry {
    Destructor d; // offset 0x0, size 0x4
    void *data;   // offset 0x4, size 0x4
};

typedef void *(*RuntimeAllocConstructor)(const char *, class DynamicLoader *, int &, bool &, const char *);
typedef void (*RuntimeAllocDestructor)(void *, int);

// TODO wrong namespace
// total size: 0x10
struct RuntimeAllocDestructorEntry {
    // void *operator new(size_t size) {}

    // void *operator new(size_t size, const char *msg) {}

    void operator delete(void *ptr, size_t size) {
        EAGL4Internal::EAGL4Free(ptr, size);
    }

    // void *operator new[](size_t size) {}

    // void *operator new[](size_t size, const char *msg) {}

    // void operator delete[](void *ptr, size_t size) {}

    // void *operator new(size_t, void *ptr) {}

    RuntimeAllocDestructorEntry(RuntimeAllocDestructor d, void *data, int auxData)
        : d(d),       //
          data(data), //
          auxData(auxData) {}

    RuntimeAllocDestructor d;          // offset 0x0, size 0x4
    void *data;                        // offset 0x4, size 0x4
    int auxData;                       // offset 0x8, size 0x4
    RuntimeAllocDestructorEntry *next; // offset 0xC, size 0x4
};

namespace EAGL4 {

// total size: 0x28
class ConstructorPool {
  public:
    ConstructorPool() {}

    ~ConstructorPool();

    void RemoveType(const char *type);

    void AddType(const char *type, Constructor c, Destructor d);

    Constructor FindConstructor(const char *type);

    Destructor FindDestructor(const char *type);

    void Empty();

    void FreeMemory();

  private:
    SymbolPool conspool; // offset 0x0, size 0x14
    SymbolPool despool;  // offset 0x14, size 0x14
};

// total size: 0x28
class RuntimeAllocConstructorPool {
  public:
    RuntimeAllocConstructorPool() {}

    ~RuntimeAllocConstructorPool();

    void RemoveType(const char *type);

    void AddType(const char *type, Constructor c, Destructor d);

    RuntimeAllocConstructor FindConstructor(const char *type);

    RuntimeAllocDestructor FindDestructor(const char *type);

    void Empty();

    void FreeMemory();

  private:
    SymbolPool conspool; // offset 0x0, size 0x14
    SymbolPool despool;  // offset 0x14, size 0x14
};

}; // namespace EAGL4

#endif
