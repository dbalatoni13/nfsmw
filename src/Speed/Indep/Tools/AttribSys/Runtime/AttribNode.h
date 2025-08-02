#ifndef ATTRIBSYS_ATTRIB_NODE_H
#define ATTRIBSYS_ATTRIB_NODE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

// Credit: Brawltendo
namespace Attrib {

class Node {
  public:
    enum Flags {
        Flag_RequiresRelease = 1 << 0,
        Flag_IsArray = 1 << 1,
        Flag_IsInherited = 1 << 2,
        Flag_IsAccessor = 1 << 3,
        Flag_IsLaidOut = 1 << 4,
        Flag_IsByValue = 1 << 5,
        Flag_IsLocatable = 1 << 6,
    };

    bool GetFlag(unsigned int mask) {
        return mFlags & mask;
    }
    bool RequiresRelease() {
        return GetFlag(Flag_RequiresRelease);
    }
    bool IsArray() {
        return GetFlag(Flag_IsArray);
    }
    bool IsInherited() {
        return GetFlag(Flag_IsInherited);
    }
    bool IsAccessor() {
        return GetFlag(Flag_IsAccessor);
    }
    bool IsLaidOut() {
        return GetFlag(Flag_IsLaidOut);
    }
    bool IsByValue() {
        return GetFlag(Flag_IsByValue);
    }
    bool IsLocatable() {
        return GetFlag(Flag_IsLocatable);
    }
    bool IsValid() {
        return IsLaidOut() || mPtr != this;
    }
    void *GetPointer(void *layoutptr) {
        if (IsByValue())
            return &mValue;
        else if (IsLaidOut())
            return (void *)(uintptr_t(layoutptr) + uintptr_t(mPtr));
        else
            return mPtr;
    }
    class Array *GetArray(void *layoutptr) {
        if (IsLaidOut())
            return (Array *)(uintptr_t(layoutptr) + uintptr_t(mArray));
        else
            return mArray;
    }
    unsigned int GetValue() {
        return mValue;
    }
    unsigned int *GetRefValue() {
        return &mValue;
    }
    unsigned int GetKey() {
        return IsValid() ? mKey : 0;
    }
    unsigned int MaxSearch() {
        return mMax;
    }

  private:
    unsigned int mKey;
    union {
        void *mPtr;
        class Array *mArray;
        unsigned int mValue;
        unsigned int mOffset;
    };
    unsigned short mTypeIndex;
    unsigned char mMax;
    unsigned char mFlags;
};

} // namespace Attrib

#endif
