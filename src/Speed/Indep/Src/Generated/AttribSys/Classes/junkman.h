#ifndef _attrib_gen_junkman_h
#define _attrib_gen_junkman_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct junkman : Instance {
    typedef JunkmanMod TypeOf_brakes_package;
    typedef JunkmanMod TypeOf_chassis_package;
    typedef JunkmanMod TypeOf_engine_package;
    typedef JunkmanMod TypeOf_induction_package;
    typedef JunkmanMod TypeOf_nos_package;
    typedef JunkmanMod TypeOf_tires_package;
    typedef JunkmanMod TypeOf_transmission_package;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("junkman");
    junkman(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {}
    junkman(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {}
    junkman(const Instance &src) : Instance(src) {}
    junkman(const junkman &src) : Instance(src) {}
    junkman(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {}
    ~junkman() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0x171737e9;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0x171737e9, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const junkman &operator=(const junkman &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const junkman &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool brakes_package(TAttrib<JunkmanMod> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(JunkmanMod, 0x56c63b6f);
    }
    bool brakes_package(JunkmanMod &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(JunkmanMod, 0x56c63b6f, result, index);
    }
    const JunkmanMod &brakes_package(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(JunkmanMod, 0x56c63b6f, index);
    }
    unsigned int Num_brakes_package() const {
        ATTRIB_CODEGEN_GETLENGTH(0x56c63b6f);
    }
    bool SET_brakes_package(const JunkmanMod &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(JunkmanMod, 0x56c63b6f, input, index);
    }
    bool chassis_package(TAttrib<JunkmanMod> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(JunkmanMod, 0xb6495c9e);
    }
    bool chassis_package(JunkmanMod &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(JunkmanMod, 0xb6495c9e, result, index);
    }
    const JunkmanMod &chassis_package(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(JunkmanMod, 0xb6495c9e, index);
    }
    unsigned int Num_chassis_package() const {
        ATTRIB_CODEGEN_GETLENGTH(0xb6495c9e);
    }
    bool SET_chassis_package(const JunkmanMod &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(JunkmanMod, 0xb6495c9e, input, index);
    }
    bool engine_package(TAttrib<JunkmanMod> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(JunkmanMod, 0x9206efd2);
    }
    bool engine_package(JunkmanMod &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(JunkmanMod, 0x9206efd2, result, index);
    }
    const JunkmanMod &engine_package(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(JunkmanMod, 0x9206efd2, index);
    }
    unsigned int Num_engine_package() const {
        ATTRIB_CODEGEN_GETLENGTH(0x9206efd2);
    }
    bool SET_engine_package(const JunkmanMod &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(JunkmanMod, 0x9206efd2, input, index);
    }
    bool induction_package(TAttrib<JunkmanMod> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(JunkmanMod, 0x7546359e);
    }
    bool induction_package(JunkmanMod &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(JunkmanMod, 0x7546359e, result, index);
    }
    const JunkmanMod &induction_package(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(JunkmanMod, 0x7546359e, index);
    }
    unsigned int Num_induction_package() const {
        ATTRIB_CODEGEN_GETLENGTH(0x7546359e);
    }
    bool SET_induction_package(const JunkmanMod &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(JunkmanMod, 0x7546359e, input, index);
    }
    bool nos_package(TAttrib<JunkmanMod> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(JunkmanMod, 0x452d2634);
    }
    bool nos_package(JunkmanMod &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(JunkmanMod, 0x452d2634, result, index);
    }
    const JunkmanMod &nos_package(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(JunkmanMod, 0x452d2634, index);
    }
    unsigned int Num_nos_package() const {
        ATTRIB_CODEGEN_GETLENGTH(0x452d2634);
    }
    bool SET_nos_package(const JunkmanMod &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(JunkmanMod, 0x452d2634, input, index);
    }
    bool tires_package(TAttrib<JunkmanMod> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(JunkmanMod, 0xc5860f58);
    }
    bool tires_package(JunkmanMod &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(JunkmanMod, 0xc5860f58, result, index);
    }
    const JunkmanMod &tires_package(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(JunkmanMod, 0xc5860f58, index);
    }
    unsigned int Num_tires_package() const {
        ATTRIB_CODEGEN_GETLENGTH(0xc5860f58);
    }
    bool SET_tires_package(const JunkmanMod &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(JunkmanMod, 0xc5860f58, input, index);
    }
    bool transmission_package(TAttrib<JunkmanMod> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(JunkmanMod, 0x25ae629a);
    }
    bool transmission_package(JunkmanMod &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(JunkmanMod, 0x25ae629a, result, index);
    }
    const JunkmanMod &transmission_package(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(JunkmanMod, 0x25ae629a, index);
    }
    unsigned int Num_transmission_package() const {
        ATTRIB_CODEGEN_GETLENGTH(0x25ae629a);
    }
    bool SET_transmission_package(const JunkmanMod &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(JunkmanMod, 0x25ae629a, input, index);
    }

  private:
    unsigned int GetLayoutSize() {
        return 0;
    }
    junkman &ConvertFromInstance(Instance &src) {}
    const junkman &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key junkman = 0x171737e9;

}; // namespace ClassName

namespace Hash {
namespace junkman {

static const Key brakes_package = 0x56c63b6f;
static const Key chassis_package = 0xb6495c9e;
static const Key engine_package = 0x9206efd2;
static const Key induction_package = 0x7546359e;
static const Key nos_package = 0x452d2634;
static const Key tires_package = 0xc5860f58;
static const Key transmission_package = 0x25ae629a;

}; // namespace junkman
}; // namespace Hash

inline Key Gen::junkman::ClassKey() {
    return ClassName::junkman;
}

}; // namespace Attrib

#endif
