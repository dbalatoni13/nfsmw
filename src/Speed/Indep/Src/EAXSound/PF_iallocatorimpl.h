#ifndef EAXSOUND_PF_IALLOCATORIMPL_H
#define EAXSOUND_PF_IALLOCATORIMPL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

inline void PF_Allocator::Free(void *pBlock, unsigned int size) {
    gAudioMemoryManager.FreeMemory(pBlock);
}

#endif
