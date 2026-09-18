#include "ScratchBuffer.h"

namespace EAGL4Anim {

ScratchBuffer ScratchBufferHelper::mScratchBuffers[3];

void ScratchBuffer::FreeBuffer() {
    mRefCount--;
    if (mBuffer && mRefCount <= 0) {

        EAGL4Internal::EAGL4Free(mBuffer, mSize);
        mBuffer = nullptr;
    }
}

ScratchBuffer &ScratchBuffer::GetScratchBuffer(int i) {
    return ScratchBufferHelper::mScratchBuffers[i];
}

};
