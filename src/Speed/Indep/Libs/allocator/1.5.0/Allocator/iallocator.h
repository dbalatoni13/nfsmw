//
//
#ifndef EA_IALLOCATOR_H
#define EA_IALLOCATOR_H 1

#include <cstddef>

#define ALLOCATOR_VERSION 150 // Decl: 8

namespace EA {

// total size: 0xC
// Decl: 25
struct TagValuePair {
    unsigned int mTag; // offset 0x0, size 0x4
    union {
        int mInt;              // offset 0x0, size 0x4
        size_t mSize;          // offset 0x0, size 0x4
        float mFloat;          // offset 0x0, size 0x4
        const void *mPointer;  // offset 0x0, size 0x4
    } mValue;                  // offset 0x4, size 0x4
    const TagValuePair *mNext; // offset 0x8, size 0x4
};

#define NULLALLOCTVP EA::TagValuePair(EA::Allocator::ATT_NULL, 0) // Decl: 98

namespace Allocator {

// Decl: 106
class IAllocator {
  public:
    virtual void *Alloc(size_t size, const TagValuePair &flags) = 0;
    virtual void Free(void *pBlock, size_t size) = 0;
    virtual int AddRef() = 0;
    virtual int Release() = 0;

  protected:
    virtual ~IAllocator() {}
};

// total size: 0x4
class ICoreAllocator {
  public:
    ICoreAllocator() {}

    virtual void *Alloc(size_t size, const char *name, unsigned int flags);
    virtual void *Alloc(size_t size, const char *name, unsigned int flags, unsigned int align, unsigned int alignOffset);
    virtual void Free(void *block, size_t size);

    ICoreAllocator *GetDefaultAllocator();
};

} // namespace Allocator
}; // namespace EA

#endif
