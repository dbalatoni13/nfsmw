#ifndef _attrib_gen_aud_stitch_loop_h
#define _attrib_gen_aud_stitch_loop_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct aud_stitch_loop : Instance {
    typedef EA::Reflection::Int32 TypeOf_overlap;
    typedef STICH_COLLISION_TYPE TypeOf_stitch;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("aud_stitch_loop");
    aud_stitch_loop(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {}
    aud_stitch_loop(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {}
    aud_stitch_loop(const Instance &src) : Instance(src) {}
    aud_stitch_loop(const aud_stitch_loop &src) : Instance(src) {}
    aud_stitch_loop(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {}
    ~aud_stitch_loop() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0x3473edcd;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0x3473edcd, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const aud_stitch_loop &operator=(const aud_stitch_loop &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const aud_stitch_loop &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool overlap(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xad8c27f5);
    }
    bool overlap(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xad8c27f5, result);
    }
    const EA::Reflection::Int32 &overlap() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xad8c27f5);
    }
    bool SET_overlap(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xad8c27f5, input);
    }
    bool stitch(TAttrib<STICH_COLLISION_TYPE> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(STICH_COLLISION_TYPE, 0x1553cd23);
    }
    bool stitch(STICH_COLLISION_TYPE &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(STICH_COLLISION_TYPE, 0x1553cd23, result);
    }
    const STICH_COLLISION_TYPE &stitch() const {
        ATTRIB_CODEGEN_GETVALUE(STICH_COLLISION_TYPE, 0x1553cd23);
    }
    bool SET_stitch(const STICH_COLLISION_TYPE &input) {
        ATTRIB_CODEGEN_SETVALUE(STICH_COLLISION_TYPE, 0x1553cd23, input);
    }

  private:
    unsigned int GetLayoutSize() {
        return 0;
    }
    aud_stitch_loop &ConvertFromInstance(Instance &src) {}
    const aud_stitch_loop &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key aud_stitch_loop = 0x3473edcd;

}; // namespace ClassName

namespace Hash {
namespace aud_stitch_loop {

static const Key overlap = 0xad8c27f5;
static const Key stitch = 0x1553cd23;

}; // namespace aud_stitch_loop
}; // namespace Hash

inline Key Gen::aud_stitch_loop::ClassKey() {
    return ClassName::aud_stitch_loop;
}

}; // namespace Attrib

#endif
