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

static unsigned int gSearchCacheLines = 0;
static unsigned int gTotalAttribNodes = 0;
static unsigned int gByValueBytes = 0;

Collection::Collection(const CollectionLoadData &loadData, Vault *v) : mTable(loadData.mTableReserve, loadData.mTableKeyShift, true) {
    mParent = nullptr;
    mClass = Database::Get().GetClass(loadData.mClass);
    mLayout = loadData.mLayout;
    mRefCount = 1;
    mKey = loadData.mKey;
    mSource = v;
    mNamePtr = KeyToString(loadData.mKey);
    mSource->AddRef();
    mClass->AddCollection(this);
    mClass->AddRef();

    if (loadData.mParent != 0) {
        mParent = mClass->GetCollection(loadData.mParent);
        KeyToString(loadData.mParent);
        KeyToString(loadData.mKey);
        KeyToString(loadData.mClass);
        KeyToString(mParent->mClass->GetKey());
        KeyToString(mClass->GetKey());
        mParent->AddRef();
    }

    bool success;
    const unsigned int *typeList = loadData.GetTypes();
    const CollectionLoadData::AttribEntry *entries = loadData.GetEntries();

    for (std::size_t i = 0; i < loadData.mNumEntries; i++) {
        const CollectionLoadData::AttribEntry &entry = entries[i];
        if (entry.mNodeFlags & Node::Flag_IsByValue) {
            unsigned int bytes = Database::Get().GetTypeDesc(typeList[entry.mType]).GetSize();
            if (bytes <= 4) {
                gByValueBytes += bytes;
            }
        }
        mTable.Add(entry.mKey, typeList[entry.mType], entry.mData, false, entry.mNodeFlags, true, mLayout);
        gSearchCacheLines += mTable.CountSearchCacheLines(entry.mKey, 7);
        gTotalAttribNodes++;
    }

    bool inLayout;
    unsigned int k;
    for (k = FirstKey(inLayout); k != 0; k = NextKey(k, inLayout)) {
        const Collection *container = nullptr;
        Node *node = GetNode(k, container);
        if (this == container) {
            if (node && node->IsArray()) {
                node->GetArray(mLayout)->SetTypeIndex(node->GetTypeDesc().GetIndex());
            }
        }
    }
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

bool Collection::AddAttribute(Key attributeKey, std::size_t count) {
    if (Contains(attributeKey)) {
        return false;
    }
    bool result = false;
    unsigned char flags = 0;
    const Attrib::Definition *d = mClass->GetDefinition(attributeKey);
    if (d) {
        d->InLayout();
        if (d->IsArray()) {
            flags = Node::Flag_RequiresRelease | Node::Flag_IsArray;
            Attrib::Array *array = Array::Create(d->GetType(), count);
            result = mTable.Add(attributeKey, d->GetType(), array, true, flags, false, mLayout);
            if (!result && (flags & Node::Flag_RequiresRelease)) {
                Array::Destroy(array);
            }
        } else {
            void *dst;
            if (d->GetSize() < 5) {
                dst = nullptr;
                flags = Node::Flag_IsByValue;
            } else {
                dst = Alloc(d->GetSize(), "Attrib::attribute_data");
                flags |= Node::Flag_RequiresRelease;
            }
            if (dst) {
                memset(dst, 0, d->GetSize());
            }
            result = mTable.Add(attributeKey, d->GetType(), dst, true, flags, false, mLayout);
            if (!result && (flags & Node::Flag_RequiresRelease)) {
                Free(dst, d->GetSize(), "Attrib::attribute_data");
            }
        }
    }
    return result;
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
                    memcpy(data, parentnode->GetPointer(container->GetLayout()), typeDesc.GetSize());
                }
            } else {
                if (isArray) {
                    reinterpret_cast<Array *>(data)->SetCount(0);
                } else {
                    memset(data, 0, typeDesc.GetSize());
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

void Collection::Clean() const {
    AttributeIterator iterator(this);
    if (iterator.Valid()) {
        do {
            const Collection *container = this;
            Node *node = GetNode(iterator.GetKey(), container);
            if (container == this) {
                node->IsValid();
                ITypeHandler *handler = node->GetTypeDesc().GetHandler();
                if (handler) {
                    if (node->IsArray()) {
                        Array *array = node->GetArray(mLayout);
                        for (unsigned int i = 0; i < array->GetCount(); i++) {
                            handler->Clean(array->GetData(i));
                        }
                    } else {
                        handler->Clean(node->GetPointer(mLayout));
                    }
                }
            }
        } while (iterator.Advance());
    }
    if (mLayout) {
        Definition *defs = mClass->mPrivates.mDefinitions;
        unsigned int count = mClass->mPrivates.mNumDefinitions;
        for (unsigned int d = 0; d < count; d++) {
            if (defs[d].IsNotSearchable()) {
                ITypeHandler *handler = Database::Get().GetTypeDesc(defs[d].GetType()).GetHandler();
                if (handler) {
                    void *dataptr = (char *)mLayout + defs[d].GetOffset();
                    if (defs[d].IsArray()) {
                        Array *array = reinterpret_cast<Array *>(dataptr);
                        for (unsigned int i = 0; i < array->GetCount(); i++) {
                            handler->Clean(array->GetData(i));
                        }
                    } else {
                        handler->Clean(dataptr);
                    }
                }
            }
        }
    }
}

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
