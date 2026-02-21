#ifndef EAGL4ANIM_EAGL4SUPPORTSYMPOOL_H
#define EAGL4ANIM_EAGL4SUPPORTSYMPOOL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "eagl4supportdef.h"

namespace EAGL4 {

// total size: 0x14
class SymbolPool {
  public:
    typedef unsigned int Position;

    // static void *operator new(size_t size) {}

    // static void *operator new(size_t size, const char *msg) {}

    // static void operator delete(void *ptr, size_t size) {}

    // static void *operator new[](size_t size) {}

    // static void *operator new[](size_t size, const char *msg) {}

    // static void operator delete[](void *ptr, size_t size) {}

    // static void *operator new(size_t, void *ptr) {}

    SymbolPool();

    ~SymbolPool();

    void FreeMemory();

    unsigned int HashFunction(const char *Name);

    void Insert(const char *Name, void *value);

    const char *AddSymbol(const char *Name, void *Address);

    void RemoveSymbol(const char *Name);

    void AddFunction(DynamicUserCallback Address);

    void RemoveFunction(DynamicUserCallback Address);

    void *Search(const char *Name, bool &Valid);

    void Empty();

    unsigned int GenerateCRC() const;

    bool IsSymbolTableValid() const;

    Position GetFirstPosition() const;

    void GetNextPosition(Position &pos) const;

    bool IsPositionAtEnd(Position pos) const;

    const char *GetSymbolName(Position pos) const;

    void *GetSymbolAddr(Position pos) const;

    // int GetSize() const {}

  private:
    unsigned int mTableSize;                    // offset 0x0, size 0x4
    unsigned int mTableLength;                  // offset 0x4, size 0x4
    EAGL4Internal::SymbolEntry **mpSymbolTable; // offset 0x8, size 0x4
    unsigned int mSymbolTableCRC32;             // offset 0xC, size 0x4
    EAGL4Internal::FunctionEntry *mpFunctions;  // offset 0x10, size 0x4
};

}; // namespace EAGL4

#endif
