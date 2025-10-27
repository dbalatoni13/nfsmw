#ifndef ATTRIBSYS_ATTRIB_ARRAY_H
#define ATTRIBSYS_ATTRIB_ARRAY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>
#include <types.h>

// Credit: Brawltendo
namespace Attrib {

// TODO should this really be in AttribHashMap.h?
class Array {
  public:
    void *operator new(std::size_t, void *ptr) {
        return ptr;
    }

    // Returns the base location of this array's data
    unsigned char *BasePointer() const {
        return const_cast<unsigned char *>(data);
    }

    // TODO
    void *Data(unsigned int byteindex, bool hack) const {
        unsigned char *base = BasePointer();
        // TODO use base?
        if (hack) {
            return (void *)(data + GetPad() + byteindex * 4);
        }
        return (void *)(data + GetPad() + byteindex);
    }

    bool IsReferences() const {
        return mSize == 0;
    }

    std::size_t GetCount() const {
        return mCount;
    }

    void *GetData(std::size_t index) const {
        if (index < mCount) {
            if (IsReferences()) {
                return *reinterpret_cast<void **>(Data(index, true));
            } else {
                return Data(index * mSize, false);
            }
        } else {
            return nullptr;
        }
    }

    unsigned int GetPad() const {
        if ((mEncodedTypePad & 0x8000) == 0) {
            return 0;
        }
        return 8;
    }

  private:
    unsigned short mAlloc;
    unsigned short mCount;
    unsigned short mSize;
    unsigned short mEncodedTypePad;
    unsigned char data[];
};

} // namespace Attrib

#endif
