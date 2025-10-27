#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

namespace Attrib {

Attribute::Attribute() {
    mInstance = nullptr;
    mCollection = nullptr;
    mInternal = nullptr;
    mDataPointer = nullptr;
}

Attribute::Attribute(const Attribute &src) {
    mInstance = src.mInstance;
    mCollection = src.mCollection;
    mInternal = src.mInternal;
    mDataPointer = src.mDataPointer;
    if (mInstance) {
        mInstance->Lock();
    }
}

Attribute::Attribute(const Instance &instance, const Collection *collection, Node *node) {
    mCollection = collection;
    mDataPointer = nullptr;
    mInstance = &instance;
    mInternal = node;

    if (node && !node->IsArray()) {
        mDataPointer = node->GetPointer(instance.GetLayoutPointer());
    }
    if (mInstance) {
        mInstance->Lock();
    }
}

const Attribute &Attribute::operator=(const Attribute &rhs) {
    if (mInstance) {
        mInstance->Unlock();
    }
    mInstance = rhs.mInstance;
    mCollection = rhs.mCollection;
    mInternal = rhs.mInternal;
    mDataPointer = rhs.mDataPointer;
    if (mInstance) {
        mInstance->Lock();
    }
    return *this;
}

bool Attribute::IsValid() const {
    if (mInternal) {
        return true;
    }
    return false;
}

Key Attribute::GetKey() const {
    if (mInternal) {
        return mInternal->GetKey();
    }
    return 0;
}

unsigned int Attribute::GetType() const {
    if (mInternal) {
        return mInternal->GetTypeDesc().GetType();
    }
    return 0;
}

const Collection *Attribute::GetCollection() const {
    return mCollection;
}

unsigned int Attribute::GetLength() const {
    bool bVar1;
    Array *pAVar3;

    if (mInternal) {
        return mInternal->GetCount(mInstance->GetLayoutPointer());
    }
    return 0;
}

}; // namespace Attrib
