#ifndef _attrib_gen_engine_h
#define _attrib_gen_engine_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct engine : Instance {
    struct _LayoutStruct {
        Private _Array_TORQUE;                   // offset 0x0, size 0x8
        EA::Reflection::Float TORQUE[9];         // offset 0x8, size 0x24
        Private _Array_SPEED_LIMITER;            // offset 0x2c, size 0x8
        EA::Reflection::Float SPEED_LIMITER[2];  // offset 0x34, size 0x8
        Private _Array_ENGINE_BRAKING;           // offset 0x3c, size 0x8
        EA::Reflection::Float ENGINE_BRAKING[3]; // offset 0x44, size 0xc
        EA::Reflection::Float FLYWHEEL_MASS;     // offset 0x50, size 0x4
        EA::Reflection::Float MAX_RPM;           // offset 0x54, size 0x4
        EA::Reflection::Float RED_LINE;          // offset 0x58, size 0x4
        EA::Reflection::Float IDLE;              // offset 0x5c, size 0x4
    };

    typedef EA::Reflection::Float TypeOf_ENGINE_BRAKING;
    typedef EA::Reflection::Float TypeOf_FLYWHEEL_MASS;
    typedef EA::Reflection::Float TypeOf_IDLE;
    typedef EA::Reflection::Float TypeOf_MAX_RPM;
    typedef EA::Reflection::Float TypeOf_RED_LINE;
    typedef EA::Reflection::Float TypeOf_SPEED_LIMITER;
    typedef EA::Reflection::Float TypeOf_TORQUE;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("engine");
    engine(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    engine(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    engine(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    engine(const engine &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    engine(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~engine() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0xf1f5fbc7;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0xf1f5fbc7, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const engine &operator=(const engine &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const engine &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool ENGINE_BRAKING(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x82c55940);
    }
    bool ENGINE_BRAKING(EA::Reflection::Float &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(ENGINE_BRAKING, result, index);
    }
    const EA::Reflection::Float &ENGINE_BRAKING(unsigned int index) const {
        ATTRIB_CODEGEN_GETLAYOUTINDEXED(EA::Reflection::Float, ENGINE_BRAKING, index);
    }
    unsigned int Num_ENGINE_BRAKING() const {
        ATTRIB_CODEGEN_GETLAYOUTLENGTH(ENGINE_BRAKING);
    }
    bool SET_ENGINE_BRAKING(const EA::Reflection::Float &input, unsigned int index) {
        ATTRIB_CODEGEN_SETLAYOUTINDEXED(ENGINE_BRAKING, input, index);
    }
    bool FLYWHEEL_MASS(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xccbb0245);
    }
    bool FLYWHEEL_MASS(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(FLYWHEEL_MASS, result);
    }
    const EA::Reflection::Float &FLYWHEEL_MASS() const {
        ATTRIB_CODEGEN_GETLAYOUT(FLYWHEEL_MASS);
    }
    bool SET_FLYWHEEL_MASS(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(FLYWHEEL_MASS, input);
    }
    bool IDLE(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x9319476a);
    }
    bool IDLE(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(IDLE, result);
    }
    const EA::Reflection::Float &IDLE() const {
        ATTRIB_CODEGEN_GETLAYOUT(IDLE);
    }
    bool SET_IDLE(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(IDLE, input);
    }
    bool MAX_RPM(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xc5561812);
    }
    bool MAX_RPM(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(MAX_RPM, result);
    }
    const EA::Reflection::Float &MAX_RPM() const {
        ATTRIB_CODEGEN_GETLAYOUT(MAX_RPM);
    }
    bool SET_MAX_RPM(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(MAX_RPM, input);
    }
    bool RED_LINE(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xe00b01bf);
    }
    bool RED_LINE(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(RED_LINE, result);
    }
    const EA::Reflection::Float &RED_LINE() const {
        ATTRIB_CODEGEN_GETLAYOUT(RED_LINE);
    }
    bool SET_RED_LINE(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(RED_LINE, input);
    }
    bool SPEED_LIMITER(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xa2d9ecb4);
    }
    bool SPEED_LIMITER(EA::Reflection::Float &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(SPEED_LIMITER, result, index);
    }
    const EA::Reflection::Float &SPEED_LIMITER(unsigned int index) const {
        ATTRIB_CODEGEN_GETLAYOUTINDEXED(EA::Reflection::Float, SPEED_LIMITER, index);
    }
    unsigned int Num_SPEED_LIMITER() const {
        ATTRIB_CODEGEN_GETLAYOUTLENGTH(SPEED_LIMITER);
    }
    bool SET_SPEED_LIMITER(const EA::Reflection::Float &input, unsigned int index) {
        ATTRIB_CODEGEN_SETLAYOUTINDEXED(SPEED_LIMITER, input, index);
    }
    bool TORQUE(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xafcd67bc);
    }
    bool TORQUE(EA::Reflection::Float &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(TORQUE, result, index);
    }
    const EA::Reflection::Float &TORQUE(unsigned int index) const {
        ATTRIB_CODEGEN_GETLAYOUTINDEXED(EA::Reflection::Float, TORQUE, index);
    }
    unsigned int Num_TORQUE() const {
        ATTRIB_CODEGEN_GETLAYOUTLENGTH(TORQUE);
    }
    bool SET_TORQUE(const EA::Reflection::Float &input, unsigned int index) {
        ATTRIB_CODEGEN_SETLAYOUTINDEXED(TORQUE, input, index);
    }

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    engine &ConvertFromInstance(Instance &src) {}
    const engine &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key engine = 0xf1f5fbc7;

}; // namespace ClassName

namespace Hash {
namespace engine {

static const Key ENGINE_BRAKING = 0x82c55940;
static const Key FLYWHEEL_MASS = 0xccbb0245;
static const Key IDLE = 0x9319476a;
static const Key MAX_RPM = 0xc5561812;
static const Key RED_LINE = 0xe00b01bf;
static const Key SPEED_LIMITER = 0xa2d9ecb4;
static const Key TORQUE = 0xafcd67bc;

}; // namespace engine
}; // namespace Hash

inline Key Gen::engine::ClassKey() {
    return ClassName::engine;
}

}; // namespace Attrib

#endif
