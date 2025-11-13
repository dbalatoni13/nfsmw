#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"

#include <ctype.h>

namespace Attrib {

static unsigned int gPeakMemory = 0;
static unsigned int gCurrMemory = 0;

void AllocationAccounting(std::size_t bytes, bool add) {
    if (add) {
        gCurrMemory += bytes;
    } else {
        gCurrMemory -= bytes;
    }
    if (gCurrMemory > gPeakMemory) {
        gPeakMemory = gCurrMemory;
    }
}

const Collection *FindCollection(Key classKey, Key collectionKey) {
    const Class *classPtr = Database::Get().GetClass(classKey);
    if (classPtr) {
        return classPtr->GetCollection(collectionKey);
    }
    return nullptr;
}

const Collection *FindCollectionWithDefault(Key classKey, Key collectionKey) {
    const Class *classPtr = Database::Get().GetClass(classKey);
    if (classPtr) {
        return classPtr->GetCollectionWithDefault(collectionKey);
    }
    return nullptr;
}

Key GetCollectionKey(const Collection *c) {
    if (c) {
        return c->GetKey();
    }
    return 0;
}

const Collection *GetCollectionParent(const Collection *c) {
    if (c) {
        return c->Parent();
    }
    return nullptr;
}

unsigned int StringToLowerCaseKey(const char *str) {
    char temp[256];
    char *ptr = temp;

    std::strlen(str);
    while (*str) {
        *ptr++ = tolower(*str++);
    }
    *ptr = '\0';
    return StringToKey(temp);
}

std::size_t AdjustHashTableSize(std::size_t requiredSize) {
    std::size_t result = requiredSize;
    return result;
}

unsigned int Private::GetLength() const {
    return ToArray()->GetCount();
}

RefSpec::RefSpec(const RefSpec &src) {
    mClassKey = src.mClassKey;
    mCollectionKey = src.mCollectionKey;
    mCollectionPtr = src.mCollectionPtr;
    if (mCollectionPtr) {
        mCollectionPtr->AddRef();
    }
}

RefSpec &RefSpec::operator=(const RefSpec &rhs) {
    Clean();
    mClassKey = rhs.mClassKey;
    mCollectionKey = rhs.mCollectionKey;
    mCollectionPtr = rhs.mCollectionPtr;
    if (mCollectionPtr) {
        mCollectionPtr->AddRef();
    }
    return *this;
}

void RefSpec::SetCollection(const Collection *collectionPtr) {
    Clean();
    if (collectionPtr) {
        mClassKey = collectionPtr->GetClass();
        mCollectionKey = collectionPtr->GetKey();
        mCollectionPtr = collectionPtr;
        collectionPtr->AddRef();
    } else {
        mCollectionKey = 0;
    }
}

const Class *RefSpec::GetClass() const {
    if (mCollectionPtr) {
        return mCollectionPtr->ContainingClass();
    }
    if (mClassKey) {
        return Database::Get().GetClass(mClassKey);
    }
    return nullptr;
}

const struct Collection *RefSpec::GetCollection() const {
    if (!mCollectionKey) {
        return nullptr;
    }
    if (!mCollectionPtr) {
        const Class *c = GetClass();
        if (!c) {
            return nullptr;
        }
        mCollectionPtr = c->GetCollection(mCollectionKey);
        if (mCollectionPtr) {
            mCollectionPtr->AddRef();
        }
    }
    return mCollectionPtr;
}

const Collection *RefSpec::GetCollectionWithDefault() const {
    if (!mCollectionPtr) {
        const Class *c = GetClass();
        if (!c) {
            return nullptr;
        }
        mCollectionPtr = c->GetCollectionWithDefault(mCollectionKey);
        if (mCollectionPtr) {
            mCollectionPtr->AddRef();
        }
    }
    return mCollectionPtr;
}

void RefSpec::Clean() const {
    if (mCollectionPtr) {
        mCollectionPtr->Release();
        *const_cast<Collection **>(&mCollectionPtr) = nullptr;
    }
}

Attribute::~Attribute() {
    if (mInstance) {
        mInstance->Unlock();
    }
}

}; // namespace Attrib
