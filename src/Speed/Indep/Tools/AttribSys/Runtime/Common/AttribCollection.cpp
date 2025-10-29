#include "AttribHashMap.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"

namespace Attrib {

Collection::Collection(Class *aclass, const Collection *parent, Key k, unsigned int reserveSize) : mTable(reserveSize, 0, false) {
    mParent = parent;
    mClass = aclass;
    mLayout = nullptr;
    mSource = nullptr;
    mKey = k;
    mRefCount = 1;
    mNamePtr = KeyToString(k);
    if (mParent && !mClass) {
        mClass = parent->mClass;
    }
    mClass->AddCollection(this);
    mClass->AddRef();
    if (mParent) {
        mParent->AddRef();
        mLayout = mClass->CloneLayout(mParent->mLayout);
    } else {
        mLayout = mClass->AllocLayout();
    }
    mNamePtr = nullptr;
}

Collection::~Collection() {
    mClass->RemoveCollection(this);
    mClass->Release();
    if (mParent) {
        mParent->Release();
    }
    Clear();
    if (mSource) {
        mSource->Release();
    } else {
        mClass->FreeLayout(mLayout);
    }
}

Attribute Collection::Get(const Instance &instance, Key attributeKey) const {
    const Collection *c;
    Node *node = GetNode(attributeKey, c);
    if (node) {
        return Attribute(instance, c, node);
    } else {
        return Attribute();
    }
}

void *Collection::GetData(Key attributeKey, std::size_t index) const {
    const Collection *c;
    Node *node = GetNode(attributeKey, c);
    if (node) {
        if (node->IsArray()) {
            return node->GetArray(c->mLayout)->GetData(index);
        } else {
            return node->GetPointer(c->mLayout);
        }
    } else {
        return nullptr;
    }
}

std::size_t Collection::Count() const {
    return mTable.Size() + mClass->mPrivates.mLayoutTable.Size();
}

// UNSOLVED regswaps
bool Collection::Contains(Key k) const {
    return mTable.Find(k) || mClass->mPrivates.mLayoutTable.Find(k);
}

Key Collection::FirstKey(bool &inLayout) const {
    Key k = ScanForValidKey(mTable, -1);
    if (k == 0) {
        k = ScanForValidKey(mClass->mPrivates.mLayoutTable, -1);
        inLayout = true;
    } else {
        inLayout = false;
    }
    return k;
}

Key Collection::NextKey(Key prev, bool &inLayout) const {
    Key k = 0;
    if (!inLayout) {
        unsigned int index = mTable.FindIndex(prev);
        if (mTable.ValidIndex(index)) {
            k = ScanForValidKey(mTable, index);
        }
        if (k == 0) {
            inLayout = true;
            k = ScanForValidKey(mClass->mPrivates.mLayoutTable, static_cast<Key>(-1));
        }
    } else {
        HashMap &layoutTable = mClass->mPrivates.mLayoutTable;
        unsigned int index = layoutTable.FindIndex(prev);
        if (layoutTable.ValidIndex(index)) {
            k = ScanForValidKey(layoutTable, index);
        }
    }
    return k;
}

void Collection::SetParent(Key parent) {
    if (mParent) {
        mParent->Release();
    }
    if (mClass) {
        mParent = mClass->GetCollection(parent);
    } else {
        mParent = nullptr;
    }
    if (mParent) {
        mParent->AddRef();
    }
}

void Collection::Delete() const {
    delete this;
}

AttributeIterator::AttributeIterator(const Collection *c) {
    mCurrentKey = 0;
    mInLayout = false;
    mCollection = c;
    mCurrentKey = c->FirstKey(mInLayout);
}

bool AttributeIterator::Advance() {
    if (mCurrentKey != 0) {
        mCurrentKey = mCollection->NextKey(mCurrentKey, mInLayout);
    }
    return mCurrentKey != 0;
}

}; // namespace Attrib
