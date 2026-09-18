#ifndef FECHUNK_H
#define FECHUNK_H

#include <types.h>
#include "Speed/Indep/Src/FEng/FETypes.h"

#define NESTED_FECHUNK 0x80000000 // :12
#define DATA_FECHUNK 0x00000000   // :13

// File: speed/indep/src/feng/FEChunk.h
// total size: 0x8
// Decl: speed/indep/src/feng/FEChunk.h:28
class FEChunk {
  private:
    u32 ID;   // offset 0x0, size 0x4, Decl: speed/indep/src/feng/FEChunk.h:30
    u32 Size; // offset 0x4, size 0x4, Decl: speed/indep/src/feng/FEChunk.h:31

  public:
    FEChunk(u32 id, u32 size) {}
    FEChunk() {} // Decl: speed/indep/src/feng/FEChunk.h:35

    u32 GetID() { // Decl: speed/indep/src/feng/FEChunk.h:37
        return FEngGetu32(ID);
    }
    u32 GetSize() { // Decl: speed/indep/src/feng/FEChunk.h:38
        return FEngGetu32(Size);
    }
    bool IsNestedChunk() { // Decl: speed/indep/src/feng/FEChunk.h:39
        return (ID & NESTED_FECHUNK) != 0;
    }
    bool IsDataChunk() { // Decl: speed/indep/src/feng/FEChunk.h:40
        return (ID & NESTED_FECHUNK) == 0;
    }
    u32 CountChildren() { // Decl: speed/indep/src/feng/FEChunk.h:41
        u32 count = 0;
        FEChunk *pChild = GetFirstChunk();
        while (pChild < GetLastChunk()) {
            count++;
            pChild = pChild->GetNext();
        }
        return count;
    }
    char *GetData() { // Decl: speed/indep/src/feng/FEChunk.h:42
        return reinterpret_cast<char *>(this) + sizeof(*this);
    }

    FEChunk *GetFirstChunk() { // Decl: speed/indep/src/feng/FEChunk.h:44
        return reinterpret_cast<FEChunk *>(reinterpret_cast<char *>(this) + sizeof(*this));
    }
    FEChunk *GetLastChunk() { // Decl: speed/indep/src/feng/FEChunk.h:45
        return reinterpret_cast<FEChunk *>(reinterpret_cast<char *>(this) + FEngGetu32(Size) + sizeof(*this));
    }
    FEChunk *GetNext() { // Decl: speed/indep/src/feng/FEChunk.h:46
        return GetLastChunk();
    }
};

#endif
