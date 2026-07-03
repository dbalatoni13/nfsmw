#ifndef _attrib_gen_audioscrape_h
#define _attrib_gen_audioscrape_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct audioscrape : Instance {
    typedef Attrib::StringKey TypeOf_CSIS_EFFECT;
    typedef Attrib::StringKey TypeOf_DESCRIPTION;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("audioscrape");
    audioscrape(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {}
    audioscrape(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {}
    audioscrape(const Instance &src) : Instance(src) {}
    audioscrape(const audioscrape &src) : Instance(src) {}
    audioscrape(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {}
    ~audioscrape() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0x11b47832;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0x11b47832, dynamicCollectionKey, spaceForAdditionalAttributes);
    }
    Key GenerateUniqueKey(const char *name, bool registerName) const {
        return GenerateUniqueKey(name, registerName);
    }
    void Change(const Collection *c) {
        Instance::Change(c);
    }
    void Change(const RefSpec &refspec) {
        Instance::Change(refspec);
    }
    void Change(Key collectionkey) {
        Change(FindCollection(ClassKey(), collectionkey));
    }
    void ChangeWithDefault(const RefSpec &refspec) {
        Instance::ChangeWithDefault(refspec);
    }
    void ChangeWithDefault(Key collectionkey) {
        Change(FindCollectionWithDefault(ClassKey(), collectionkey));
    }
    const audioscrape &operator=(const audioscrape &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const audioscrape &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool CSIS_EFFECT(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0xbec30f42);
    }
    bool CSIS_EFFECT(Attrib::StringKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0xbec30f42, result);
    }
    const Attrib::StringKey &CSIS_EFFECT() const {
        ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0xbec30f42);
    }
    bool SET_CSIS_EFFECT(const Attrib::StringKey &input) {
        ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0xbec30f42, input);
    }
    bool DESCRIPTION(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x09925106);
    }
    bool DESCRIPTION(Attrib::StringKey &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(Attrib::StringKey, 0x09925106, result, index);
    }
    const Attrib::StringKey &DESCRIPTION(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(Attrib::StringKey, 0x09925106, index);
    }
    unsigned int Num_DESCRIPTION() const {
        ATTRIB_CODEGEN_GETLENGTH(0x09925106);
    }
    bool SET_DESCRIPTION(const Attrib::StringKey &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(Attrib::StringKey, 0x09925106, input, index);
    }

  private:
    unsigned int GetLayoutSize() {
        return 0;
    }
    audioscrape &ConvertFromInstance(Instance &src) {}
    const audioscrape &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key audioscrape = 0x11b47832;

}; // namespace ClassName

namespace Hash {
namespace audioscrape {

static const Key CSIS_EFFECT = 0xbec30f42;
static const Key DESCRIPTION = 0x09925106;

}; // namespace audioscrape
}; // namespace Hash

inline Key Gen::audioscrape::ClassKey() {
    return ClassName::audioscrape;
}

}; // namespace Attrib

#endif
