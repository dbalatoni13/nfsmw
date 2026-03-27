#include "ScratchBuffer.h"

#include "Speed/Indep/Src/EAGL4Anim/eagl4supportdef.h"

namespace EAGL4Anim {

ScratchBuffer ScratchBufferHelper::mScratchBuffers[] = {};

void ScratchBuffer::FreeBuffer() {
    int refCount = mRefCount;

    mRefCount = refCount - 1;
    if (mBuffer && refCount - 1 < 1) {
        EAGL4Internal::EAGL4Free(mBuffer, mSize);
        mBuffer = nullptr;
    }
}

ScratchBuffer &ScratchBuffer::GetScratchBuffer(int i) {
    return ScratchBufferHelper::mScratchBuffers[i];
}

};
