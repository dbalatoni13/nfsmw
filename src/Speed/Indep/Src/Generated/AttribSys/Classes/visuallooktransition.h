#ifndef _attrib_gen_visuallooktransition_h
#define _attrib_gen_visuallooktransition_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct visuallooktransition : Instance {
    typedef EA::Reflection::Float TypeOf__testvalue;
    typedef UMath::Matrix4 TypeOf_normal;
    typedef UMath::Matrix4 TypeOf_uves;
    typedef UMath::Matrix4 TypeOf_uves_extreme;
    typedef UMath::Matrix4 TypeOf_uves_pulse;
    typedef EA::Reflection::Float TypeOf_uves_pulse_length;
    typedef EA::Reflection::Float TypeOf_uves_pulse_trigger;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("visuallooktransition");
    visuallooktransition(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {}
    visuallooktransition(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {}
    visuallooktransition(const Instance &src) : Instance(src) {}
    visuallooktransition(const visuallooktransition &src) : Instance(src) {}
    visuallooktransition(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {}
    ~visuallooktransition() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0x0f409aa6;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0x0f409aa6, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const visuallooktransition &operator=(const visuallooktransition &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const visuallooktransition &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool _testvalue(TAttrib<EA::Reflection::Float> &result) const;
    bool _testvalue(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &_testvalue() const;
    bool SET__testvalue(const EA::Reflection::Float &input);

    bool normal(TAttrib<UMath::Matrix4> &result) const;
    bool normal(UMath::Matrix4 &result) const;
    const UMath::Matrix4 &normal() const;
    bool SET_normal(const UMath::Matrix4 &input);

    bool uves(TAttrib<UMath::Matrix4> &result) const;
    bool uves(UMath::Matrix4 &result) const;
    const UMath::Matrix4 &uves() const;
    bool SET_uves(const UMath::Matrix4 &input);

    bool uves_extreme(TAttrib<UMath::Matrix4> &result) const;
    bool uves_extreme(UMath::Matrix4 &result) const;
    const UMath::Matrix4 &uves_extreme() const;
    bool SET_uves_extreme(const UMath::Matrix4 &input);

    bool uves_pulse(TAttrib<UMath::Matrix4> &result) const;
    bool uves_pulse(UMath::Matrix4 &result) const;
    const UMath::Matrix4 &uves_pulse() const;
    bool SET_uves_pulse(const UMath::Matrix4 &input);

    bool uves_pulse_length(TAttrib<EA::Reflection::Float> &result) const;
    bool uves_pulse_length(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &uves_pulse_length() const;
    bool SET_uves_pulse_length(const EA::Reflection::Float &input);

    bool uves_pulse_trigger(TAttrib<EA::Reflection::Float> &result) const;
    bool uves_pulse_trigger(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &uves_pulse_trigger() const;
    bool SET_uves_pulse_trigger(const EA::Reflection::Float &input);

  private:
    unsigned int GetLayoutSize() {
        return 0;
    }
    visuallooktransition &ConvertFromInstance(Instance &src) {}
    const visuallooktransition &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key visuallooktransition = 0x0f409aa6;

}; // namespace ClassName

namespace Hash {
namespace visuallooktransition {

static const Key _testvalue = 0x4ef07d5d;
static const Key normal = 0x0eb3d357;
static const Key uves = 0x681bef75;
static const Key uves_extreme = 0x0e629621;
static const Key uves_pulse = 0x334f1e4d;
static const Key uves_pulse_length = 0x0b82d479;
static const Key uves_pulse_trigger = 0x53769b6c;

}; // namespace visuallooktransition
}; // namespace Hash

inline Key Gen::visuallooktransition::ClassKey() {
    return ClassName::visuallooktransition;
}

inline bool Gen::visuallooktransition::_testvalue(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x4ef07d5d);
}
inline const EA::Reflection::Float &Gen::visuallooktransition::_testvalue() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x4ef07d5d);
}
inline bool Gen::visuallooktransition::_testvalue(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x4ef07d5d, result);
}
inline bool Gen::visuallooktransition::SET__testvalue(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x4ef07d5d, input);
}

inline bool Gen::visuallooktransition::normal(TAttrib<UMath::Matrix4> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Matrix4, 0x0eb3d357);
}
inline const UMath::Matrix4 &Gen::visuallooktransition::normal() const {
    ATTRIB_CODEGEN_GETVALUE(UMath::Matrix4, 0x0eb3d357);
}
inline bool Gen::visuallooktransition::normal(UMath::Matrix4 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(UMath::Matrix4, 0x0eb3d357, result);
}
inline bool Gen::visuallooktransition::SET_normal(const UMath::Matrix4 &input) {
    ATTRIB_CODEGEN_SETVALUE(UMath::Matrix4, 0x0eb3d357, input);
}

inline bool Gen::visuallooktransition::uves(TAttrib<UMath::Matrix4> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Matrix4, 0x681bef75);
}
inline const UMath::Matrix4 &Gen::visuallooktransition::uves() const {
    ATTRIB_CODEGEN_GETVALUE(UMath::Matrix4, 0x681bef75);
}
inline bool Gen::visuallooktransition::uves(UMath::Matrix4 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(UMath::Matrix4, 0x681bef75, result);
}
inline bool Gen::visuallooktransition::SET_uves(const UMath::Matrix4 &input) {
    ATTRIB_CODEGEN_SETVALUE(UMath::Matrix4, 0x681bef75, input);
}

inline bool Gen::visuallooktransition::uves_extreme(TAttrib<UMath::Matrix4> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Matrix4, 0x0e629621);
}
inline const UMath::Matrix4 &Gen::visuallooktransition::uves_extreme() const {
    ATTRIB_CODEGEN_GETVALUE(UMath::Matrix4, 0x0e629621);
}
inline bool Gen::visuallooktransition::uves_extreme(UMath::Matrix4 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(UMath::Matrix4, 0x0e629621, result);
}
inline bool Gen::visuallooktransition::SET_uves_extreme(const UMath::Matrix4 &input) {
    ATTRIB_CODEGEN_SETVALUE(UMath::Matrix4, 0x0e629621, input);
}

inline bool Gen::visuallooktransition::uves_pulse(TAttrib<UMath::Matrix4> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Matrix4, 0x334f1e4d);
}
inline const UMath::Matrix4 &Gen::visuallooktransition::uves_pulse() const {
    ATTRIB_CODEGEN_GETVALUE(UMath::Matrix4, 0x334f1e4d);
}
inline bool Gen::visuallooktransition::uves_pulse(UMath::Matrix4 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(UMath::Matrix4, 0x334f1e4d, result);
}
inline bool Gen::visuallooktransition::SET_uves_pulse(const UMath::Matrix4 &input) {
    ATTRIB_CODEGEN_SETVALUE(UMath::Matrix4, 0x334f1e4d, input);
}

inline bool Gen::visuallooktransition::uves_pulse_length(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x0b82d479);
}
inline const EA::Reflection::Float &Gen::visuallooktransition::uves_pulse_length() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x0b82d479);
}
inline bool Gen::visuallooktransition::uves_pulse_length(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x0b82d479, result);
}
inline bool Gen::visuallooktransition::SET_uves_pulse_length(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x0b82d479, input);
}

inline bool Gen::visuallooktransition::uves_pulse_trigger(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x53769b6c);
}
inline const EA::Reflection::Float &Gen::visuallooktransition::uves_pulse_trigger() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x53769b6c);
}
inline bool Gen::visuallooktransition::uves_pulse_trigger(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x53769b6c, result);
}
inline bool Gen::visuallooktransition::SET_uves_pulse_trigger(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x53769b6c, input);
}

}; // namespace Attrib

#endif
