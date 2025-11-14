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

Node *Collection::GetNode(Key attributeKey, const Collection *&container) const {
    const Collection *c = this;
    while (c) {
        Node *node = c->mTable.Find(attributeKey);
        if (node) {
            container = c;
            return node;
        }
        c = c->mParent;
    }

    if (mLayout) {
        Node *node = mClass->mPrivates.mLayoutTable.Find(attributeKey);
        if (node) {
            container = this;
            return node;
        }
    }
    container = nullptr;
    return nullptr;
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

bool Collection::RemoveAttribute(Key attributeKey) {
    Node *node = mTable.Find(attributeKey);
    if (node) {
        bool isArray = node->IsArray();
        bool releaseRequired = node->RequiresRelease();
        const TypeDesc &typeDesc = node->GetTypeDesc();
        void *data = mTable.Remove(node, mLayout, true);

        if (node->IsLaidOut()) {
            const Collection *container = nullptr;
            Node *parentnode = GetNode(attributeKey, container);
            if (parentnode) {
                if (isArray) {
                    *reinterpret_cast<Array *>(data) = *parentnode->GetArray(container->GetLayout());
                } else {
                    std::memcpy(data, parentnode->GetPointer(container->GetLayout()), typeDesc.GetSize());
                }
            } else {
                if (isArray) {
                    reinterpret_cast<Array *>(data)->SetCount(0);
                } else {
                    std::memset(data, 0, typeDesc.GetSize());
                }
            }
        } else if (data) {
            FreeNodeData(isArray, data, releaseRequired, typeDesc);
        }
        return true;
    }
    return false;
}

void Collection::Clear() {
    std::size_t index = mTable.GetNextValidIndex(-1);
    while (mTable.ValidIndex(index)) {
        Node *node = mTable.GetNodeAtIndex(index);
        node->IsValid(); // useless but needed

        bool isArray = node->IsArray();
        bool releaseRequired = node->RequiresRelease();
        const TypeDesc &typeDesc = node->GetTypeDesc();

        void *data = mTable.Remove(node, mLayout, false);
        if (data) {
            FreeNodeData(isArray, data, releaseRequired, typeDesc);
        }
        index = mTable.GetNextValidIndex(index);
    }
}

void Collection::Clean() const {}

void Collection::FreeNodeData(bool isArray, void *data, bool requiresRelease, const TypeDesc &typeDesc) {
    ITypeHandler *handler = typeDesc.GetHandler();
    if (isArray) {
        Array *array = reinterpret_cast<Array *>(data);
        if (handler) {
            for (std::size_t i = 0; i < array->GetCount(); i++) {
                handler->Release(array->GetData(i));
            }
        }
        if (requiresRelease) {
            Array::Destroy(array);
        }
    } else {
        if (handler) {
            handler->Release(data);
        }
        if (requiresRelease) {
            Free(data, typeDesc.GetSize(), "Attrib::attribute_data");
        }
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
