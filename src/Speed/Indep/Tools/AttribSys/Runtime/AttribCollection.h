#ifndef ATTRIBSYS_ATTRIB_COLLECTION_H
#define ATTRIBSYS_ATTRIB_COLLECTION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Common/AttribHashMap.h"

// Credit: Brawltendo
namespace Attrib {

class Collection {
  public:
    class Node *GetNode(unsigned int attributeKey, const Collection *&container);
    void *GetData(unsigned int attributeKey, unsigned int index);

  private:
    HashMap mTable;
    const Collection *mParent;
    class Class *mClass;
    void *mLayout;
    unsigned int mRefCount;
    unsigned int mKey;
    class Vault *mSource;
    const char *mNamePtr;
};

} // namespace Attrib

#endif
