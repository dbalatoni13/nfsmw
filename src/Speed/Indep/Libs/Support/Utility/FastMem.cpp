#include "FastMem.h"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

void FastMemEmergencyInitialization(unsigned int &bytes, const char *&name, unsigned int &expansion, unsigned int &track);
void *bMalloc(int size, int flags);
void bFree(void *ptr);
void bBreak();

// total size: 0x10
class AllocDesc {
    unsigned int mIndex; // offset 0x0, size 0x4
    const char *mName;   // offset 0x4, size 0x4
    unsigned int mCount; // offset 0x8, size 0x4
    unsigned int mHigh;  // offset 0xC, size 0x4
};

FastMem::FastMem() {}

// STRIPPED
// FastMem::FastMem(EA::Allocator::IAllocator *allocator, unsigned int bytes, const char *name, unsigned int expansionsize, unsigned int trackingsize)
// {}

void FastMem::Init() {
    if (!mInited) {
        FastMemEmergencyInitialization(mBytes, mName, mExpansionSize, mTrackMax);
        for (unsigned int i = 0; i < 64; i++) {
            mFreeLists[i] = 0;
            mAlloc[i] = 0;
            mAvail[i] = 0;
        }
        mBlock = CoreAlloc(mBytes, mName);
        if (mExpansionSize != 0) {
            *(unsigned int *)mBlock = 0;
            ((unsigned int *)mBlock)[1] = mBytes;
            mUsed = 16;
        }
        mInited = true;
    }
}

void *FastMem::Alloc(std::size_t bytes, const char *kind) {
    void *mem;
    if (bytes > 0x400) {
        mem = CoreAlloc(bytes, kind);
        bMemSet(mem, 0, bytes);
    } else {
        std::size_t listIndex = (bytes - 1) >> 4;
        if (mFreeLists[listIndex] == 0) {
            if (CreateBlock(listIndex) == 0) {
                DumpRecord();
                bReleasePrintf("Out of Fastmem memory\n");
                bBreak();
                return 0;
            }
        }
        FreeBlock *block = mFreeLists[listIndex];
        mem = block;
        mFreeLists[listIndex] = block->mNext;
        bMemSet(mem, 0, bytes);
    }
    return mem;
}

void FastMem::Free(void *ptr, std::size_t bytes, const char *kind) {
    if (bytes > 0x400) {
        CoreFree(ptr);
    } else {
        size_t listIndex = (bytes - 1) / 16;
        FreeBlock *freeBlk = reinterpret_cast<FreeBlock *>(ptr);
        freeBlk->mNext = this->mFreeLists[listIndex]; // TODO bug? shouldn't this be FastMem::FreeBlock::mNext?
        this->mFreeLists[listIndex] = freeBlk;
    }
}

void *FastMem::CoreAlloc(std::size_t bytes, const char *kind) {
    return bMalloc(bytes, 0);
}

void FastMem::CoreFree(void *ptr) {
    bFree(ptr);
}

bool FastMem::SplitOrExpand(std::size_t bytes) {
    void *block;
    if (mExpansionSize == 0 || mLocks != 0) {
        return false;
    }
    block = CoreAlloc(mExpansionSize, mName);
    if (block == 0) {
        return false;
    }
    ((void **)block)[0] = mBlock;
    ((unsigned int *)block)[1] = mExpansionSize;
    mBlock = block;
    mBytes = mExpansionSize;
    mUsed = 16;
    return true;
}

bool FastMem::AssignToFree(std::size_t bytes) {
    if (mBytes < mUsed + bytes) {
        unsigned int remaining = mBytes - mUsed;
        if (remaining > 16 && (mUsed & 0x1F)) {
            AssignToFree(16);
        }
        while ((remaining = mBytes - mUsed) > 32) {
            AssignToFree(32);
        }
        if (SplitOrExpand(bytes) == 0) {
            return false;
        }
    }
    unsigned int offset = mUsed;
    mUsed = offset + bytes;
    Free((char *)mBlock + offset, bytes, 0);
    return true;
}

bool FastMem::CreateBlock(unsigned int listIndex) {
    unsigned int bytes;
    unsigned int mask;

    if (!mInited) {
        Init();
    }
    bytes = (listIndex + 1) * 16;
    mask = bytes - 1;
    if ((bytes & mask) == 0) {
        if (listIndex != 0) {
            if (mUsed & 0x1F) {
                if (AssignToFree(16) == 0) {
                    return false;
                }
            }
        }
        if (listIndex > 1) {
            if (mUsed & 0x3F) {
                unsigned int num2xBlocks = 2;
                do {
                    if (AssignToFree(32) == 0) {
                        return false;
                    }
                    if ((mUsed & 0x3F) == 0) {
                        break;
                    }
                } while (num2xBlocks-- > 0);
            }
        }
        if (listIndex > 3) {
            mask &= 0x7F;
            if (mUsed & mask) {
                do {
                    if (AssignToFree(64) == 0) {
                        return false;
                    }
                } while (mUsed & mask);
            }
        }
    }
    return AssignToFree(bytes);
}

// STRIPPED
void FastMem::DumpRecord() {}
