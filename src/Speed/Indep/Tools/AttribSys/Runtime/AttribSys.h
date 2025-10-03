#ifndef ATTRIBSYS_ATTRIBSYS_H
#define ATTRIBSYS_ATTRIBSYS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AttribCollection.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Misc/AttribAlloc.h"

// Credit: Brawltendo
namespace Attrib {

typedef unsigned int Key;

// const int kTypeHandlerCount = 7;
// unsigned int kTypeHandlerIds[kTypeHandlerCount] = {0x2B936EB7u, 0xAA229CD7u, 0x341F03A0u, 0x600994C4u, 0x681D219Cu, 0x5FDE6463u, 0x57D382C9u};

const void *DefaultDataArea(unsigned int bytes);

inline void Free(void *ptr, unsigned int bytes, const char *name) {
    AttribAlloc::Free(ptr, bytes, name);
}

class RefSpec {
    // total size: 0xC
    Key mClassKey;                    // offset 0x0, size 0x4
    Key mCollectionKey;               // offset 0x4, size 0x4
    const Collection *mCollectionPtr; // offset 0x8, size 0x4

  public:
    void SetCollection(const Collection *collectionPtr);
    const Class *GetClass() const;
    const Collection *GetCollection() const;
    const Collection *GetCollectionWithDefault() const;
    RefSpec &operator=(const RefSpec &rhs);
    void Clean() const;

    void operator delete(void *ptr, std::size_t bytes) {
        Free(ptr, bytes, "RefSpec");
    }

    RefSpec() {
        mClassKey = 0;
        mCollectionKey = 0;
        mCollectionPtr = nullptr;
    }

    ~RefSpec() {
        if (mCollectionPtr) {
            Clean();
        }
    }

    Key GetClassKey() const {
        return mClassKey;
    }

    Key GetCollectionKey() const {
        return mCollectionKey;
    }
};

struct Instance;

class Attribute {
  protected:
    void *GetElementPointer(unsigned int index) const {
        return &reinterpret_cast<int *>(mDataPointer)[index];
    }

  public:
    Attribute(const Attrib::Attribute &src);
    Attribute();
    ~Attribute();
    const Attrib::Attribute &operator=(const Attrib::Attribute &rhs);
    bool operator==(const Attrib::Attribute &rhs) const;
    bool operator!=(const Attrib::Attribute &rhs) const;
    bool IsValid() const;
    bool IsInherited() const;
    bool IsMutable() const;
    bool IsLocatable();
    Key GetKey() const;
    unsigned int GetType() const;
    const Attrib::Instance *GetInstance() const;
    const Attrib::Collection *GetCollection() const;
    unsigned int GetSize() const;
    unsigned int GetLength() const;
    bool SetLength(unsigned int);
    void SendChangeMsg() const;
    template <typename T> const T &Get(unsigned int index, T &result) const;

    Key GetCollectionKey() const {
        // TODO
    }

    const void *GetDataAddress() const {
        return this->mDataPointer;
    }

    Attrib::Node *GetInternal() const {
        return this->mInternal;
    }

    bool Get(unsigned int index, RefSpec &result) {
        const RefSpec *resultptr = reinterpret_cast<const RefSpec *>(mDataPointer ? GetElementPointer(index) : GetInternalPointer(index));
        if (resultptr) {
            result = *resultptr;
            return true;
        } else {
            return false;
        }
    }

  private:
    void *GetInternalPointer(unsigned int index) const;

    Attribute(const Instance &instance, const Attrib::Collection *collection, Attrib::Node *node);

    // total size: 0x10
    const Instance *mInstance;     // offset 0x0, size 0x4
    const Collection *mCollection; // offset 0x4, size 0x4
    struct Node *mInternal;        // offset 0x8, size 0x4
    void *mDataPointer;            // offset 0xC, size 0x4
};

struct Instance {
    UTL::COM::IUnknown *mOwner;
    const Collection *mCollection;
    void *mLayoutPtr;
    unsigned int mMsgPort;
    unsigned short mFlags;
    unsigned short mLocks;

    enum Flags { kDynamic = 1 };

    Instance(const Instance &src);
    Instance(const struct Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner);
    Instance(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner);
    ~Instance();
    Key GetClass() const;
    Key GetCollection() const;
    Key GetParent() const;
    void SetParent(unsigned int parent);
    const void *GetAttributePointer(Key attribkey, unsigned int index) const;
    const Instance &operator=(const Instance &rhs);
    Attribute Get(Key attributeKey) const;
    void Change(const Collection *collection);

    void *GetLayoutPointer() const {
        return mLayoutPtr;
    }

    void *GetLayoutPointer() {
        return mLayoutPtr;
    }

    const Collection *GetConstCollection() const {
        return mCollection;
    }

    void SetDefaultLayout(unsigned int bytes) {
        if (this->mLayoutPtr == nullptr) {
            this->mLayoutPtr = const_cast<void *>(Attrib::DefaultDataArea(bytes));
        }
    }

    bool IsValid() const {
        return this->mCollection != nullptr;
    }
};

} // namespace Attrib

template <typename T> class AttributeStructPtr : public T {
  public:
    static Attrib::Key GetClassKey() {
        return mAttributeClass;
    }

    AttributeStructPtr(Attrib::Key namekey) : T(namekey, 0, nullptr) {
        if (!T::IsValid()) {
            T::Change(0xeec2271a);
        }
    }

    ~AttributeStructPtr() {}

    const T &operator*() const {
        return *this;
    }

    const T *operator->() const {
        return this;
    }

    operator const T *() const {
        return this;
    }

  private:
    static unsigned int mAttributeClass;
};

#endif
