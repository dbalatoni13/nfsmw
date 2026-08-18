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
    bool AirSupportOptions(TAttrib<AirSupport> &result) const;
    bool AirSupportOptions(AirSupport &result, unsigned int index) const;
    const AirSupport &AirSupportOptions(unsigned int index) const;
    unsigned int Num_AirSupportOptions() const;
    bool SET_AirSupportOptions(const AirSupport &input, unsigned int index);

    bool HeavySupportOptions(TAttrib<HeavySupport> &result) const;
    bool HeavySupportOptions(HeavySupport &result, unsigned int index) const;
    const HeavySupport &HeavySupportOptions(unsigned int index) const;
    unsigned int Num_HeavySupportOptions() const;
    bool SET_HeavySupportOptions(const HeavySupport &input, unsigned int index);

    bool LeaderSupportOptions(TAttrib<LeaderSupport> &result) const;
    bool LeaderSupportOptions(LeaderSupport &result, unsigned int index) const;
    const LeaderSupport &LeaderSupportOptions(unsigned int index) const;
    unsigned int Num_LeaderSupportOptions() const;
    bool SET_LeaderSupportOptions(const LeaderSupport &input, unsigned int index);

    bool MinimumSupportDelay(TAttrib<EA::Reflection::Float> &result) const;
    bool MinimumSupportDelay(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &MinimumSupportDelay() const;
    bool SET_MinimumSupportDelay(const EA::Reflection::Float &input);

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

inline bool Gen::pursuitsupport::AirSupportOptions(TAttrib<AirSupport> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(AirSupport, 0x3c6dbcb3);
}
inline const AirSupport &Gen::pursuitsupport::AirSupportOptions(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(AirSupport, 0x3c6dbcb3, index);
}
inline bool Gen::pursuitsupport::AirSupportOptions(AirSupport &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(AirSupport, 0x3c6dbcb3, result, index);
}
inline unsigned int Gen::pursuitsupport::Num_AirSupportOptions() const {
    ATTRIB_CODEGEN_GETLENGTH(0x3c6dbcb3);
}
inline bool Gen::pursuitsupport::SET_AirSupportOptions(const AirSupport &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(AirSupport, 0x3c6dbcb3, input, index);
}

inline bool Gen::pursuitsupport::HeavySupportOptions(TAttrib<HeavySupport> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(HeavySupport, 0xa73c3512);
}
inline const HeavySupport &Gen::pursuitsupport::HeavySupportOptions(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(HeavySupport, 0xa73c3512, index);
}
inline bool Gen::pursuitsupport::HeavySupportOptions(HeavySupport &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(HeavySupport, 0xa73c3512, result, index);
}
inline unsigned int Gen::pursuitsupport::Num_HeavySupportOptions() const {
    ATTRIB_CODEGEN_GETLENGTH(0xa73c3512);
}
inline bool Gen::pursuitsupport::SET_HeavySupportOptions(const HeavySupport &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(HeavySupport, 0xa73c3512, input, index);
}

inline bool Gen::pursuitsupport::LeaderSupportOptions(TAttrib<LeaderSupport> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(LeaderSupport, 0xebd53935);
}
inline const LeaderSupport &Gen::pursuitsupport::LeaderSupportOptions(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(LeaderSupport, 0xebd53935, index);
}
inline bool Gen::pursuitsupport::LeaderSupportOptions(LeaderSupport &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(LeaderSupport, 0xebd53935, result, index);
}
inline unsigned int Gen::pursuitsupport::Num_LeaderSupportOptions() const {
    ATTRIB_CODEGEN_GETLENGTH(0xebd53935);
}
inline bool Gen::pursuitsupport::SET_LeaderSupportOptions(const LeaderSupport &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(LeaderSupport, 0xebd53935, input, index);
}

inline bool Gen::pursuitsupport::MinimumSupportDelay(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xe4e4bc48);
}
inline const EA::Reflection::Float &Gen::pursuitsupport::MinimumSupportDelay() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xe4e4bc48);
}
inline bool Gen::pursuitsupport::MinimumSupportDelay(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xe4e4bc48, result);
}
inline bool Gen::pursuitsupport::SET_MinimumSupportDelay(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xe4e4bc48, input);
}

}; // namespace Attrib

#endif
