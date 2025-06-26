#include "DefragFixer.hpp"
#include <types.h>

void DefragFixer::Add(void *pmem, int mem_size, int movement_offset) {
    uintptr_t mem_low = (uintptr_t)pmem;
    uintptr_t mem_high = (uintptr_t)pmem + mem_size;

    if (this->NumRanges == 0) {
        this->MemLow = mem_low;
        this->MemHigh = mem_high;
    } else {
        if (mem_low < this->MemLow) {
            this->MemLow = mem_low;
        }
        if (mem_high > this->MemHigh) {
            this->MemHigh = mem_high;
        }
        Range *prev_range = &this->RangeTable[this->NumRanges - 1];
        if (prev_range->MovementOffset == movement_offset) {
            prev_range->MemHigh = mem_high;
            return;
        }
    }
    Range *range = &this->RangeTable[this->NumRanges];
    this->NumRanges = this->NumRanges + 1;
    range->MemLow = mem_low;
    range->MemHigh = mem_high;
    range->MovementOffset = movement_offset;
}

void *DefragFixer::Fix(void *pmem) {
    unsigned int mem = (uintptr_t)pmem;

    if ((mem < this->MemLow) || (mem >= this->MemHigh)) {
        return pmem;
    }
    for (int n = 0; n < this->NumRanges; n++) {
        Range *range = &this->RangeTable[n];
        if ((mem >= range->MemLow) && (mem < range->MemHigh)) {
            return reinterpret_cast<void *>(mem + range->MovementOffset);
        }
    }
    return pmem;
}
