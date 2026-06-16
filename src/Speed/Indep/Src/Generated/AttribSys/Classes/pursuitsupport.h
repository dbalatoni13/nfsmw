#ifndef _attrib_gen_pursuitsupport_h
#define _attrib_gen_pursuitsupport_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct pursuitsupport : Instance {
    typedef AirSupport TypeOf_AirSupportOptions;
    typedef HeavySupport TypeOf_HeavySupportOptions;
    typedef LeaderSupport TypeOf_LeaderSupportOptions;
    typedef EA::Reflection::Float TypeOf_MinimumSupportDelay;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("pursuitsupport");
    pursuitsupport(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {}
    pursuitsupport(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {}
    pursuitsupport(const Instance &src) : Instance(src) {}
    pursuitsupport(const pursuitsupport &src) : Instance(src) {}
    pursuitsupport(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {}
    ~pursuitsupport() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0x77b93104;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0x77b93104, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const pursuitsupport &operator=(const pursuitsupport &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const pursuitsupport &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool AirSupportOptions(TAttrib<AirSupport> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(AirSupport, 0x3c6dbcb3);
    }
    bool AirSupportOptions(AirSupport &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(AirSupport, 0x3c6dbcb3, result, index);
    }
    const AirSupport &AirSupportOptions(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(AirSupport, 0x3c6dbcb3, index);
    }
    unsigned int Num_AirSupportOptions() const {
        ATTRIB_CODEGEN_GETLENGTH(0x3c6dbcb3);
    }
    bool SET_AirSupportOptions(const AirSupport &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(AirSupport, 0x3c6dbcb3, input, index);
    }
    bool HeavySupportOptions(TAttrib<HeavySupport> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(HeavySupport, 0xa73c3512);
    }
    bool HeavySupportOptions(HeavySupport &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(HeavySupport, 0xa73c3512, result, index);
    }
    const HeavySupport &HeavySupportOptions(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(HeavySupport, 0xa73c3512, index);
    }
    unsigned int Num_HeavySupportOptions() const {
        ATTRIB_CODEGEN_GETLENGTH(0xa73c3512);
    }
    bool SET_HeavySupportOptions(const HeavySupport &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(HeavySupport, 0xa73c3512, input, index);
    }
    bool LeaderSupportOptions(TAttrib<LeaderSupport> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(LeaderSupport, 0xebd53935);
    }
    bool LeaderSupportOptions(LeaderSupport &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(LeaderSupport, 0xebd53935, result, index);
    }
    const LeaderSupport &LeaderSupportOptions(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(LeaderSupport, 0xebd53935, index);
    }
    unsigned int Num_LeaderSupportOptions() const {
        ATTRIB_CODEGEN_GETLENGTH(0xebd53935);
    }
    bool SET_LeaderSupportOptions(const LeaderSupport &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(LeaderSupport, 0xebd53935, input, index);
    }
    bool MinimumSupportDelay(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xe4e4bc48);
    }
    bool MinimumSupportDelay(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xe4e4bc48, result);
    }
    const EA::Reflection::Float &MinimumSupportDelay() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xe4e4bc48);
    }
    bool SET_MinimumSupportDelay(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xe4e4bc48, input);
    }

  private:
    unsigned int GetLayoutSize() {
        return 0;
    }
    pursuitsupport &ConvertFromInstance(Instance &src) {}
    const pursuitsupport &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key pursuitsupport = 0x77b93104;

}; // namespace ClassName

namespace Hash {
namespace pursuitsupport {

static const Key AirSupportOptions = 0x3c6dbcb3;
static const Key HeavySupportOptions = 0xa73c3512;
static const Key LeaderSupportOptions = 0xebd53935;
static const Key MinimumSupportDelay = 0xe4e4bc48;

}; // namespace pursuitsupport
}; // namespace Hash

inline Key Gen::pursuitsupport::ClassKey() {
    return ClassName::pursuitsupport;
}

}; // namespace Attrib

#endif
