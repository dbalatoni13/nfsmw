#include "Speed/Indep/Tools/AttribSys//Runtime//AttribSys.h"

namespace Attrib {

inline void AddInstance() {}

inline void RmvInstance() {}

Instance::Instance(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) {
    mOwner = owner;
    mCollection = collection;
    mLayoutPtr = nullptr;
    mMsgPort = msgPort;
    mFlags = 0;
    mLocks = 0;

    if (mCollection) {
        if (mCollection->IsDynamic()) {
            mFlags |= 1;
        }
        mLayoutPtr = mCollection->GetLayout();
        mCollection->AddRef();
        AddInstance();
    }
}

Instance::Instance(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) {
    mOwner = owner;
    mCollection = refspec.GetCollection();
    mLayoutPtr = nullptr;
    mMsgPort = msgPort;
    mFlags = 0;
    mLocks = 0;

    if (mCollection) {
        if (mCollection->IsDynamic()) {
            mFlags |= 1;
        }
        mLayoutPtr = mCollection->GetLayout();
        mCollection->AddRef();
        AddInstance();
    }
}

Instance::Instance(const Instance &src) {
    mOwner = src.mOwner;
    mCollection = src.mCollection;
    mLayoutPtr = src.mLayoutPtr;
    mMsgPort = src.mMsgPort;
    mFlags = src.mFlags;
    mLocks = 0;

    if (mCollection) {
        if (mCollection->IsDynamic()) {
            mFlags |= 1;
        }
        mLayoutPtr = mCollection->GetLayout();
        mCollection->AddRef();
        AddInstance();
    }
}

const Instance &Instance::operator=(const Instance &rhs) {
    Change(rhs.mCollection);
    mOwner = rhs.mOwner;
    mMsgPort = rhs.mMsgPort;
    mFlags = rhs.mFlags;
    return *this;
}

Instance::~Instance() {
    RmvInstance();
    if (mCollection) {
        mCollection->Release();
    }
}

Key Instance::GetClass() const {
    if (mCollection) {
        return mCollection->GetClass();
    }
    return 0;
}

Key Instance::GetCollection() const {
    if (mCollection) {
        return mCollection->GetKey();
    }
    return 0;
}

Key Instance::GetParent() const {
    if (mCollection) {
        return mCollection->GetParent();
    }
    return 0;
}

void Instance::SetParent(Key parent) {
    if (IsDynamic() && mCollection) {
        GetDynamicCollection()->SetParent(parent);
    }
}

Attribute Instance::Get(Key attributeKey) const {
    if (mCollection) {
        return mCollection->Get(*this, attributeKey);
    }
    return Attribute();
}

bool Instance::Lookup(Key attributeKey, Attribute &attrib) const {
    if (mCollection) {
        attrib = mCollection->Get(*this, attributeKey);
        return attrib.IsValid();
    }
    return false;
}

bool Instance::Contains(Key attributeKey) const {
    if (mCollection) {
        return mCollection->Contains(attributeKey);
    }
    return false;
}

AttributeIterator Instance::Iterator() const {
    return AttributeIterator(mCollection);
}

unsigned int Instance::LocalAttribCount() const {
    if (mCollection) {
        return mCollection->Count();
    }
    return 0;
}

bool Instance::Add(Key attributeKey, unsigned int count) {
    if (IsDynamic() && GetDynamicCollection()->AddAttribute(attributeKey, count)) {
        return true;
    }
    return false;
}

bool Instance::Remove(Key attributeKey) {
    bool result = false;
    Collection *dynamicCollection = GetDynamicCollection();
    if (dynamicCollection) {
        result = dynamicCollection->RemoveAttribute(attributeKey);
    }
    return result;
}

bool Instance::Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
    return ModifyInternal(mCollection->GetClass(), dynamicCollectionKey, LocalAttribCount() + spaceForAdditionalAttributes);
}

bool Instance::ModifyInternal(Key classKey, Key dynamicCollectionKey, unsigned int reserve) {
    if (!IsDynamic()) {
        Class *c = Database::Get().GetClass(classKey);
        if (c->GetCollection(dynamicCollectionKey)) {
            return false;
        }
        const Collection *parent = mCollection;
        mCollection = new Collection(c, parent, dynamicCollectionKey, reserve);
        if (parent) {
            parent->Release();
        }
        mLayoutPtr = mCollection->GetLayout();
        mFlags |= 1;
    }
    return true;
}

void Instance::Unmodify() {
    if (IsDynamic()) {
        Collection *collection = const_cast<Collection *>(mCollection);
        mCollection = collection->Parent();
        if (mCollection) {
            mLayoutPtr = mCollection->GetLayout();
            mCollection->AddRef();
        } else {
            mLayoutPtr = nullptr;
        }
        collection->Release();
        mFlags &= ~1;
    }
}

}; // namespace Attrib
