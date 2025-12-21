#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"
#include <cstdio>

namespace Attrib {

inline void AddInstance() {}

inline void RmvInstance() {}

Instance::Instance(const Collection *collection, uint32_t msgPort, UTL::COM::IUnknown *owner) {
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

Instance::Instance(const RefSpec &refspec, uint32_t msgPort, UTL::COM::IUnknown *owner) {
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

Key Instance::GenerateUniqueKey(const char *name, bool registerName) const {
    return GUKeyInternal(mCollection->GetClass(), name, registerName);
}

Key Instance::GUKeyInternal(Key classKey, const char *name, bool registerName) const {
    static unsigned int gGUKindexer = 0;
    char namebuffer[64];
    Class *c;
    Key k;

    std::strlen(name);
    c = Database::Get().GetClass(classKey);
    do {
        std::sprintf(namebuffer, "%s_%08x", name, gGUKindexer++);
        k = StringToKey(namebuffer);
        if (c->GetCollection(k)) {
            k = 0;
        }
    } while (k == 0);
    if (registerName) {
        return RegisterString(namebuffer);
    } else {
        return k;
    }
}

void Instance::Change(const Collection *collection) {
    if (collection != mCollection) {
        Unmodify();
        if (mCollection) {
            mCollection->Release();
        }
        mCollection = collection;

        bool isDynamic = false;
        if (collection) {
            collection->AddRef();
            mLayoutPtr = mCollection->GetLayout();
            isDynamic = mCollection->IsDynamic();
        }
        if (isDynamic) {
            this->mFlags |= 1;
        } else {
            this->mFlags &= ~1;
        }
    }
}

void Instance::Change(const RefSpec &refspec) {
    Change(refspec.GetCollection());
}

void Instance::ChangeWithDefault(const RefSpec &refspec) {
    Change(refspec.GetCollectionWithDefault());
}

const void *Instance::GetAttributePointer(Key attribkey, unsigned int index) const {
    if (mCollection) {
        return mCollection->GetData(attribkey, index);
    }
    return nullptr;
}

}; // namespace Attrib
