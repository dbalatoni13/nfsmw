#ifndef ATTRIBSUPPORT_H__
#define ATTRIBSUPPORT_H__

#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

namespace Attrib {

const Collection *FindCollection(unsigned int classKey, unsigned int collectionKey);
const Collection *FindCollectionWithDefault(unsigned int classKey, unsigned int collectionKey);
unsigned int GetCollectionKey(const Collection *c);
const Collection *GetCollectionParent(const Collection *c);
unsigned int StringToLowerCaseKey(const char *str);

} // namespace Attrib

template <typename T> class AttributeStructPtr : public T {
  public:
    static Attrib::Key GetClassKey() {
        return mAttributeClass;
    }

    AttributeStructPtr(Attrib::Key namekey) : T(namekey, 0, nullptr) {
        if (!T::IsValid()) {
            T::Change(Attrib::key_default);
        }
        T::IsValid();
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
    static Attrib::Key mAttributeClass;
};

#define BIND_ATTRIBUTE_STRUCT(_ATTRIBUTE_CLASS_)                                                                                                     \
    template <> Attrib::Key AttributeStructPtr<Attrib::Gen::_ATTRIBUTE_CLASS_>::mAttributeClass = Attrib::StringToKey(#_ATTRIBUTE_CLASS_);

#endif
