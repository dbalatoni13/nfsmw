#include "eagl4supportconspool.h"

#include <types.h>

namespace EAGL4 {

void ConstructorPool::AddType(const char *type, Constructor c, Destructor d) {
    conspool.AddSymbol(type, (void *)c);
    despool.AddSymbol(type, (void *)d);
}

Constructor ConstructorPool::FindConstructor(const char *type) {
    bool valid;
    Constructor c = (Constructor)conspool.Search(type, valid);
    if (!valid) {
        c = nullptr;
    }
    return c;
}

Destructor ConstructorPool::FindDestructor(const char *type) {
    bool valid;
    Destructor d = (Destructor)despool.Search(type, valid);
    if (!valid) {
        d = nullptr;
    }
    return d;
}

RuntimeAllocConstructor RuntimeAllocConstructorPool::FindConstructor(const char *type) {
    bool valid;
    RuntimeAllocConstructor c = (RuntimeAllocConstructor)conspool.Search(type, valid);
    if (!valid) {
        c = nullptr;
    }
    return c;
}

RuntimeAllocDestructor RuntimeAllocConstructorPool::FindDestructor(const char *type) {
    bool valid;
    RuntimeAllocDestructor d = (RuntimeAllocDestructor)despool.Search(type, valid);
    if (!valid) {
        d = nullptr;
    }
    return d;
}

}; // namespace EAGL4
