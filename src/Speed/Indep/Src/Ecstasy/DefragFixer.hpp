#ifndef ECSTASY_DEFRAG_FIXER_H
#define ECSTASY_DEFRAG_FIXER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

struct Range {
    // total size: 0xC
    uintptr_t MemLow;   // offset 0x0, size 0x4
    uintptr_t MemHigh;  // offset 0x4, size 0x4
    int MovementOffset; // offset 0x8, size 0x4
};

struct DefragFixer {
    // total size: 0x60C
    uintptr_t MemLow;      // offset 0x0, size 0x4
    uintptr_t MemHigh;     // offset 0x4, size 0x4
    int NumRanges;         // offset 0x8, size 0x4
    Range RangeTable[128]; // offset 0xC, size 0x600

    DefragFixer() {}
    void Init() {}

    void Add(void *pmem, int mem_size, int movement_offset);
    void *Fix(void *pmem);
};

#endif
