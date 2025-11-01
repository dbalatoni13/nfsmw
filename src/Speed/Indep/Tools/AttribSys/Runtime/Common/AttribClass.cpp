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

const Collection *Class::GetCollectionWithDefault(Key key) const {
    Collection *result = mPrivates.mCollections.Find(key);
    if (!result) {
        result = mPrivates.mCollections.Find(0xeec2271a);
    }
    return result;
}

std::size_t Class::GetNumCollections() const {
    return mPrivates.mCollections.Size();
}

Key Class::GetFirstCollection() const {
    return ScanForValidKey(mPrivates.mCollections, -1);
}

Key Class::GetNextCollection(Key prev) const {
    unsigned int index = mPrivates.mCollections.FindIndex(prev);
    if (mPrivates.mCollections.ValidIndex(index)) {
        return ScanForValidKey(mPrivates.mCollections, index);
    }
    return 0;
}

std::size_t Class::GetTableNodeSize() const {
    return 12;
}

void Class::Delete() const {
    delete &mPrivates;
}

void *Class::AllocLayout() const {
    if (mPrivates.mLayoutSize == 0) {
        return nullptr;
    } else {
        void *data = Attrib::Alloc(mPrivates.mLayoutSize, "Attrib::Class");
        memset(data, 0, mPrivates.mLayoutSize);

        Definition *defs = mPrivates.mDefinitions;
        for (std::size_t d = 0; d < mPrivates.mNumDefinitions; d++) {
            if (defs[d].IsArray() && defs[d].InLayout()) {
                char *arrayptr = (char *)data + defs[d].GetOffset(); // or maybe the offset is added later
                Array::CreateInPlace(arrayptr, defs[d].GetType(), 0, defs[d].GetMaxCount());
            }
        }
        return data;
    }
}

void *Class::CloneLayout(void *srcLayout) const {

    if (mPrivates.mLayoutSize == 0) {
        return nullptr;
    } else if (!srcLayout) {
        return AllocLayout();
    } else {
        void *data = Alloc(mPrivates.mLayoutSize, "Attrib::layout");
        CopyLayout(srcLayout, data);
        return data;
    }
}

void Class::CopyLayout(void *srcLayout, void *dstLayout) const {
    if (mPrivates.mLayoutSize != 0) {
        std::memcpy(dstLayout, srcLayout, mPrivates.mLayoutSize);
    }
}

void Class::FreeLayout(void *layout) const {
    if (layout) {
        Free(layout, mPrivates.mLayoutSize, "Attrib::layout");
    }
}

}; // namespace Attrib
