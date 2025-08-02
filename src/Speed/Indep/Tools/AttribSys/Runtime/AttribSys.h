#ifndef ATTRIBSYS_ATTRIBSYS_H
#define ATTRIBSYS_ATTRIBSYS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AttribCollection.h"
#include "Speed/Indep/Src/Misc/AttribAlloc.h"

namespace UTL {
namespace COM {
class IUnknown;
}
} // namespace UTL

// Credit: Brawltendo
namespace Attrib {

const int kTypeHandlerCount = 7;
unsigned int kTypeHandlerIds[kTypeHandlerCount] = {0x2B936EB7u, 0xAA229CD7u, 0x341F03A0u, 0x600994C4u, 0x681D219Cu, 0x5FDE6463u, 0x57D382C9u};

const void *DefaultDataArea(unsigned int bytes);

inline void Free(void *ptr, unsigned int bytes, const char *name) {
    AttribAlloc::Free(ptr, bytes, name);
}

class RefSpec {
  public:
    unsigned int GetClassKey() {
        return mClassKey;
    }
    unsigned int GetCollectionKey() {
        return mCollectionKey;
    }

  private:
    unsigned int mClassKey;
    unsigned int mCollectionKey;
    const Collection *mCollectionPtr;
};

struct Instance {
    UTL::COM::IUnknown *mOwner;
    const Collection *mCollection;
    void *mLayoutPtr;
    unsigned int mMsgPort;
    unsigned short mFlags;
    unsigned short mLocks;

    enum Flags { kDynamic = 1 };

    Instance(const struct Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner);
    ~Instance();
    void *GetAttributePointer(unsigned int attribkey, unsigned int index);

    void *GetLayoutPointer() const {
        return mLayoutPtr;
    }
    void *GetLayoutPointer() {
        return mLayoutPtr;
    }
    void SetDefaultLayout(unsigned int bytes) {
        if (this->mLayoutPtr == nullptr) {
            this->mLayoutPtr = const_cast<void *>(Attrib::DefaultDataArea(bytes));
        }
    }

    bool IsValid() const {
        return this->mCollection != nullptr;
    }
};

} // namespace Attrib

#endif
