#ifndef FENG_FECHUNK_H
#define FENG_FECHUNK_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// total size: 0x4
struct FETag {
    unsigned short ID;   // offset 0x0, size 0x2
    unsigned short Size; // offset 0x2, size 0x2

    inline unsigned short GetID() { return ID; }
    inline unsigned short GetSize() { return Size; }
    inline unsigned char* Data() { return reinterpret_cast<unsigned char*>(this) + 4; }
    inline unsigned long Getu32(unsigned long Index) { return reinterpret_cast<unsigned long*>(Data())[Index]; }
    inline int Geti32(unsigned long Index) { return reinterpret_cast<int*>(Data())[Index]; }
    inline unsigned short Getu16(unsigned long Index) { return reinterpret_cast<unsigned short*>(Data())[Index]; }
    inline short Geti16(unsigned long Index) { return reinterpret_cast<short*>(Data())[Index]; }
    inline float Getf32(unsigned long Index) { return reinterpret_cast<float*>(Data())[Index]; }
    inline FETag* Next() { return reinterpret_cast<FETag*>(Data() + Size); }
};

// total size: 0x8
struct FEChunk {
    unsigned long ID;   // offset 0x0, size 0x4
    unsigned long Size; // offset 0x4, size 0x4

    inline unsigned long GetID() { return ID; }
    inline unsigned long GetSize() { return Size; }
    inline bool IsNestedChunk() { return (ID & 0x10000) != 0; }
    inline bool IsDataChunk() { return (ID & 0x10000) == 0; }
    inline char* GetData() { return reinterpret_cast<char*>(this) + 8; }
    inline FEChunk* GetFirstChunk() { return reinterpret_cast<FEChunk*>(GetData()); }
    inline FEChunk* GetLastChunk() { return reinterpret_cast<FEChunk*>(reinterpret_cast<char*>(this) + Size); }
    inline FEChunk* GetNext() { return reinterpret_cast<FEChunk*>(reinterpret_cast<char*>(this) + Size + 8); }

    inline unsigned long CountChildren() {
        unsigned long count = 0;
        FEChunk* pChild = GetFirstChunk();
        while (pChild < GetLastChunk()) {
            count++;
            pChild = pChild->GetNext();
        }
        return count;
    }
};

#endif
