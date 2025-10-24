#include "AttribSys.h"
#include "Common/AttribPrivate.h"

namespace Attrib {

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
