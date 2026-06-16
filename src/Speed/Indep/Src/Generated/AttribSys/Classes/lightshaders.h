#ifndef _attrib_gen_lightshaders_h
#define _attrib_gen_lightshaders_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct lightshaders : Instance {
    typedef Attrib::StringKey TypeOf_name;
    typedef Attrib::StringKey TypeOf_shadername;
    typedef EA::Reflection::Bool TypeOf_useVertexColour;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("lightshaders");
    lightshaders(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {}
    lightshaders(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {}
    lightshaders(const Instance &src) : Instance(src) {}
    lightshaders(const lightshaders &src) : Instance(src) {}
    lightshaders(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {}
    ~lightshaders() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0xbc8c1aa3;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0xbc8c1aa3, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const lightshaders &operator=(const lightshaders &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const lightshaders &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool name(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0xba41a46a);
    }
    bool name(Attrib::StringKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0xba41a46a, result);
    }
    const Attrib::StringKey &name() const {
        ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0xba41a46a);
    }
    bool SET_name(const Attrib::StringKey &input) {
        ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0xba41a46a, input);
    }
    bool shadername(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x39004977);
    }
    bool shadername(Attrib::StringKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::StringKey, 0x39004977, result);
    }
    const Attrib::StringKey &shadername() const {
        ATTRIB_CODEGEN_GETVALUE(Attrib::StringKey, 0x39004977);
    }
    bool SET_shadername(const Attrib::StringKey &input) {
        ATTRIB_CODEGEN_SETVALUE(Attrib::StringKey, 0x39004977, input);
    }
    bool useVertexColour(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x2de05384);
    }
    bool useVertexColour(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x2de05384, result);
    }
    const EA::Reflection::Bool &useVertexColour() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x2de05384);
    }
    bool SET_useVertexColour(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x2de05384, input);
    }

  private:
    unsigned int GetLayoutSize() {
        return 0;
    }
    lightshaders &ConvertFromInstance(Instance &src) {}
    const lightshaders &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key lightshaders = 0xbc8c1aa3;

}; // namespace ClassName

namespace Hash {
namespace lightshaders {

static const Key name = 0xba41a46a;
static const Key shadername = 0x39004977;
static const Key useVertexColour = 0x2de05384;

}; // namespace lightshaders
}; // namespace Hash

inline Key Gen::lightshaders::ClassKey() {
    return ClassName::lightshaders;
}

}; // namespace Attrib

#endif
