#ifndef FECHUNK_H
#define FECHUNK_H

#include <types.h>
#include "Speed/Indep/Src/FEng/FETypes.h"

#define NESTED_FECHUNK 0x80000000 // :12
#define DATA_FECHUNK 0x00000000   // :13

// total size: 0x8
// Decl: 28
class FEChunk {
  private:
    u32 ID;   // offset 0x0, size 0x4, Decl: 30
    u32 Size; // offset 0x4, size 0x4, Decl: 31

  public:
    FEChunk(u32 id, u32 size) {}
    FEChunk() {} // Decl: 35

    u32 GetID() { // Decl: 37
        return FEngGetu32(ID);
    }
    u32 GetSize() { // Decl: 38
        return FEngGetu32(Size);
    }
    bool IsNestedChunk() { // Decl: 39
        return (ID & NESTED_FECHUNK) != 0;
    }
    bool IsDataChunk() { // Decl: 40
        return (ID & NESTED_FECHUNK) == 0;
    }
    u32 CountChildren() { // Decl: 41
        u32 count = 0;
        FEChunk *pChild = GetFirstChunk();
        while (pChild < GetLastChunk()) {
            count++;
            pChild = pChild->GetNext();
        }
        return count;
    }
    char *GetData() { // Decl: 42
        return reinterpret_cast<char *>(this) + sizeof(*this);
    }

    FEChunk *GetFirstChunk() { // Decl: 44
        return reinterpret_cast<FEChunk *>(reinterpret_cast<char *>(this) + sizeof(*this));
    }
    FEChunk *GetLastChunk() { // Decl: 45
        return reinterpret_cast<FEChunk *>(reinterpret_cast<char *>(this) + FEngGetu32(Size) + sizeof(*this));
    }
    FEChunk *GetNext() { // Decl: 46
        return GetLastChunk();
    }
};

#endif
