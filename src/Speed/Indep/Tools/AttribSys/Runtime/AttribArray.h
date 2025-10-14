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
    unsigned int GetPad();

    // Returns the base location of this array's data
    unsigned char *BasePointer() {
        return reinterpret_cast<unsigned char *>(&this[1]);
    }

    void *Data(unsigned int byteindex) {
        unsigned char *base = BasePointer();
        return &base[byteindex];
    }

    bool IsReferences() const {
        // TODO or != 0?
        return mSize == 0;
    }

    void *GetData(std::size_t index) {
        if (index < mCount) {
            if (IsReferences()) {
                return Data(index * 4 + GetPad());
            } else {
                return Data(index * mSize + GetPad());
            }
        } else {
            return nullptr;
        }
    }

  private:
    unsigned short mAlloc;
    unsigned short mCount;
    unsigned short mSize;
    unsigned short mEncodedTypePad;
};

} // namespace Attrib

#endif
