#include <stdlib.h>

namespace EA {

struct TagValuePair {
    TagValuePair() {}
    TagValuePair(unsigned int tag, int value);

    unsigned int mTag;
    union {
        int mInt;
        unsigned int mSize;
        float mFloat;
        const void *mPointer;
    } mValue;
    const TagValuePair *mNext;
};

inline TagValuePair::TagValuePair(unsigned int tag, int value) {
    this->mTag = tag;
    this->mValue.mInt = value;
    this->mNext = 0;
}

namespace Allocator {

class IAllocator {
  public:
    virtual void *Alloc(unsigned int size, const TagValuePair &flags) = 0;
    virtual void Free(void *pBlock, unsigned int size) = 0;
};

} // namespace Allocator

} // namespace EA

namespace Vp6 {

EA::Allocator::IAllocator *gAllocator = 0;

void SetAllocator(EA::Allocator::IAllocator *allocator) {
    gAllocator = allocator;
}

void *Alloc(int size) {
    if (gAllocator != 0) {
        return gAllocator->Alloc(size, EA::TagValuePair(0, 0));
    }
    return malloc(size);
}

void Free(void *mem) {
    if (gAllocator != 0) {
        gAllocator->Free(mem, 0);
    } else {
        free(mem);
    }
}

} // namespace Vp6
