#ifndef EAXSOUND_PF_IALLOCATORIMPL_H
#define EAXSOUND_PF_IALLOCATORIMPL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

inline void PF_Allocator::Free(void *pBlock, unsigned int size) {
    gAudioMemoryManager.FreeMemory(pBlock);
}

inline int PF_Allocator::AddRef() {
    return ++mRefcount;
}

inline int PF_Allocator::Release() {
    if (--mRefcount < 1) {
        if (this) {
            delete this;
        }
        return 0;
    }
    return mRefcount;
}

#endif
