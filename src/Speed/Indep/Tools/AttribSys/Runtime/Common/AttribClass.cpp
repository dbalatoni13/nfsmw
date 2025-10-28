#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"

#include <algorithm>

namespace Attrib {

Class::Class(Key k, ClassPrivate &privates) : mKey(k), mRefCount(1), mPrivates(privates) {
    Database::Get().AddClass(this);
}

Class::~Class() {
    Database::Get().RemoveClass(this);
}

const Definition *Class::GetDefinition(Key key) const {
    Definition target(key);
    const Definition *b = mPrivates.mDefinitions;
    const Definition *e = &b[mPrivates.mNumDefinitions];

    const Definition *result = std::lower_bound(b, e, target);
    if (result < e) {
        return result;
    }
    return nullptr;
}

std::size_t Class::GetNumDefinitions() const {
    return mPrivates.mNumDefinitions;
}

Key Class::GetFirstDefinition() const {
    if (mPrivates.mNumDefinitions) {
        return mPrivates.mDefinitions[0].GetKey();
    }
    return 0;
}

Key Class::GetNextDefinition(Key prev) const {
    Definition prevTarget(prev);
    const Definition *b = mPrivates.mDefinitions;
    const Definition *e = &b[mPrivates.mNumDefinitions];

    const Definition *result = std::lower_bound(b, e, prevTarget);
    Key index = result - b + 1;
    if (index < mPrivates.mNumDefinitions) {
        return mPrivates.mDefinitions[index].GetKey();
    }
    return 0;
}

const Collection *Class::GetCollection(Key key) const {
    return mPrivates.mCollections.Find(key);
}

std::size_t Class::GetNumCollections() const {
    return mPrivates.mCollections.Size();
}

Key Class::GetFirstCollection() const {
    return ScanForValidKey(mPrivates.mCollections, -1);
}

std::size_t Class::GetTableNodeSize() const {
    return 12;
}

void Class::CopyLayout(void *srcLayout, void *dstLayout) const {
    if (mPrivates.mLayoutSize != 0) {
        std::memcpy(dstLayout, srcLayout, mPrivates.mLayoutSize);
    }
}

void Class::FreeLayout(void *layout) const {
    if (layout) {
        Free(layout, mPrivates.mLayoutSize, nullptr);
    }
}

}; // namespace Attrib
