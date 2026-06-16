#ifndef _attrib_gen_fuelcell_emitter_h
#define _attrib_gen_fuelcell_emitter_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct fuelcell_emitter : Instance {
    struct _LayoutStruct {
        UMath::Vector4 VolumeCenter;                // offset 0x0, size 0x10
        UMath::Vector4 VelocityDelta;               // offset 0x10, size 0x10
        UMath::Vector4 VolumeExtent;                // offset 0x20, size 0x10
        UMath::Vector4 VelocityInherit;             // offset 0x30, size 0x10
        UMath::Vector4 VelocityStart;               // offset 0x40, size 0x10
        UMath::Vector4 Colour1;                     // offset 0x50, size 0x10
        RefSpec emitteruv;                          // offset 0x60, size 0xc
        EA::Reflection::Float Life;                 // offset 0x6c, size 0x4
        EA::Reflection::Float NumParticlesVariance; // offset 0x70, size 0x4
        EA::Reflection::Float GravityStart;         // offset 0x74, size 0x4
        EA::Reflection::Float HeightStart;          // offset 0x78, size 0x4
        EA::Reflection::Float GravityDelta;         // offset 0x7c, size 0x4
        EA::Reflection::Float LengthStart;          // offset 0x80, size 0x4
        EA::Reflection::Float LengthDelta;          // offset 0x84, size 0x4
        EA::Reflection::Float LifeVariance;         // offset 0x88, size 0x4
        EA::Reflection::Float NumParticles;         // offset 0x8c, size 0x4
        EA::Reflection::Int16 Spin;                 // offset 0x90, size 0x2
        EA::Reflection::Int8 zSprite;               // offset 0x92, size 0x1
        EA::Reflection::Int8 zContrail;             // offset 0x93, size 0x1
    };

    typedef UMath::Vector4 TypeOf_Colour1;
    typedef EA::Reflection::Float TypeOf_GravityDelta;
    typedef EA::Reflection::Float TypeOf_GravityStart;
    typedef EA::Reflection::Float TypeOf_HeightStart;
    typedef EA::Reflection::Float TypeOf_LengthDelta;
    typedef EA::Reflection::Float TypeOf_LengthStart;
    typedef EA::Reflection::Float TypeOf_Life;
    typedef EA::Reflection::Float TypeOf_LifeVariance;
    typedef EA::Reflection::Float TypeOf_NumParticles;
    typedef EA::Reflection::Float TypeOf_NumParticlesVariance;
    typedef EA::Reflection::Int16 TypeOf_Spin;
    typedef UMath::Vector4 TypeOf_VelocityDelta;
    typedef UMath::Vector4 TypeOf_VelocityInherit;
    typedef UMath::Vector4 TypeOf_VelocityStart;
    typedef UMath::Vector4 TypeOf_VolumeCenter;
    typedef UMath::Vector4 TypeOf_VolumeExtent;
    typedef RefSpec TypeOf_emitteruv;
    typedef EA::Reflection::Int8 TypeOf_zContrail;
    typedef EA::Reflection::Int8 TypeOf_zSprite;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("fuelcell_emitter");
    fuelcell_emitter(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    fuelcell_emitter(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    fuelcell_emitter(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    fuelcell_emitter(const fuelcell_emitter &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    fuelcell_emitter(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~fuelcell_emitter() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0xb267a856;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0xb267a856, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const fuelcell_emitter &operator=(const fuelcell_emitter &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const fuelcell_emitter &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool Colour1(TAttrib<UMath::Vector4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x54c99f82);
    }
    bool Colour1(UMath::Vector4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(Colour1, result);
    }
    const UMath::Vector4 &Colour1() const {
        ATTRIB_CODEGEN_GETLAYOUT(Colour1);
    }
    bool SET_Colour1(const UMath::Vector4 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(Colour1, input);
    }
    bool GravityDelta(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x0802dd99);
    }
    bool GravityDelta(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(GravityDelta, result);
    }
    const EA::Reflection::Float &GravityDelta() const {
        ATTRIB_CODEGEN_GETLAYOUT(GravityDelta);
    }
    bool SET_GravityDelta(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(GravityDelta, input);
    }
    bool GravityStart(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xe652e2b6);
    }
    bool GravityStart(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(GravityStart, result);
    }
    const EA::Reflection::Float &GravityStart() const {
        ATTRIB_CODEGEN_GETLAYOUT(GravityStart);
    }
    bool SET_GravityStart(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(GravityStart, input);
    }
    bool HeightStart(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x4c141ed7);
    }
    bool HeightStart(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(HeightStart, result);
    }
    const EA::Reflection::Float &HeightStart() const {
        ATTRIB_CODEGEN_GETLAYOUT(HeightStart);
    }
    bool SET_HeightStart(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(HeightStart, input);
    }
    bool LengthDelta(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xa6762035);
    }
    bool LengthDelta(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(LengthDelta, result);
    }
    const EA::Reflection::Float &LengthDelta() const {
        ATTRIB_CODEGEN_GETLAYOUT(LengthDelta);
    }
    bool SET_LengthDelta(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(LengthDelta, input);
    }
    bool LengthStart(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x6bbc13ee);
    }
    bool LengthStart(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(LengthStart, result);
    }
    const EA::Reflection::Float &LengthStart() const {
        ATTRIB_CODEGEN_GETLAYOUT(LengthStart);
    }
    bool SET_LengthStart(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(LengthStart, input);
    }
    bool Life(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x81625b35);
    }
    bool Life(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(Life, result);
    }
    const EA::Reflection::Float &Life() const {
        ATTRIB_CODEGEN_GETLAYOUT(Life);
    }
    bool SET_Life(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(Life, input);
    }
    bool LifeVariance(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xefb4bb64);
    }
    bool LifeVariance(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(LifeVariance, result);
    }
    const EA::Reflection::Float &LifeVariance() const {
        ATTRIB_CODEGEN_GETLAYOUT(LifeVariance);
    }
    bool SET_LifeVariance(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(LifeVariance, input);
    }
    bool NumParticles(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xdc943cc9);
    }
    bool NumParticles(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(NumParticles, result);
    }
    const EA::Reflection::Float &NumParticles() const {
        ATTRIB_CODEGEN_GETLAYOUT(NumParticles);
    }
    bool SET_NumParticles(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(NumParticles, input);
    }
    bool NumParticlesVariance(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xd8165518);
    }
    bool NumParticlesVariance(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(NumParticlesVariance, result);
    }
    const EA::Reflection::Float &NumParticlesVariance() const {
        ATTRIB_CODEGEN_GETLAYOUT(NumParticlesVariance);
    }
    bool SET_NumParticlesVariance(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(NumParticlesVariance, input);
    }
    bool Spin(TAttrib<EA::Reflection::Int16> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int16, 0xe643b999);
    }
    bool Spin(EA::Reflection::Int16 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(Spin, result);
    }
    const EA::Reflection::Int16 &Spin() const {
        ATTRIB_CODEGEN_GETLAYOUT(Spin);
    }
    bool SET_Spin(const EA::Reflection::Int16 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(Spin, input);
    }
    bool VelocityDelta(TAttrib<UMath::Vector4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x4bfe9062);
    }
    bool VelocityDelta(UMath::Vector4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(VelocityDelta, result);
    }
    const UMath::Vector4 &VelocityDelta() const {
        ATTRIB_CODEGEN_GETLAYOUT(VelocityDelta);
    }
    bool SET_VelocityDelta(const UMath::Vector4 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(VelocityDelta, input);
    }
    bool VelocityInherit(TAttrib<UMath::Vector4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x028d34e5);
    }
    bool VelocityInherit(UMath::Vector4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(VelocityInherit, result);
    }
    const UMath::Vector4 &VelocityInherit() const {
        ATTRIB_CODEGEN_GETLAYOUT(VelocityInherit);
    }
    bool SET_VelocityInherit(const UMath::Vector4 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(VelocityInherit, input);
    }
    bool VelocityStart(TAttrib<UMath::Vector4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x1b6c6ce2);
    }
    bool VelocityStart(UMath::Vector4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(VelocityStart, result);
    }
    const UMath::Vector4 &VelocityStart() const {
        ATTRIB_CODEGEN_GETLAYOUT(VelocityStart);
    }
    bool SET_VelocityStart(const UMath::Vector4 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(VelocityStart, input);
    }
    bool VolumeCenter(TAttrib<UMath::Vector4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x1bd3f18c);
    }
    bool VolumeCenter(UMath::Vector4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(VolumeCenter, result);
    }
    const UMath::Vector4 &VolumeCenter() const {
        ATTRIB_CODEGEN_GETLAYOUT(VolumeCenter);
    }
    bool SET_VolumeCenter(const UMath::Vector4 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(VolumeCenter, input);
    }
    bool VolumeExtent(TAttrib<UMath::Vector4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x0aaf558c);
    }
    bool VolumeExtent(UMath::Vector4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(VolumeExtent, result);
    }
    const UMath::Vector4 &VolumeExtent() const {
        ATTRIB_CODEGEN_GETLAYOUT(VolumeExtent);
    }
    bool SET_VolumeExtent(const UMath::Vector4 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(VolumeExtent, input);
    }
    bool emitteruv(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0xe4983a7d);
    }
    bool emitteruv(RefSpec &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(emitteruv, result);
    }
    const RefSpec &emitteruv() const {
        ATTRIB_CODEGEN_GETLAYOUT(emitteruv);
    }
    bool SET_emitteruv(const RefSpec &input) {
        ATTRIB_CODEGEN_SETLAYOUT(emitteruv, input);
    }
    bool zContrail(TAttrib<EA::Reflection::Int8> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int8, 0x77f88e8c);
    }
    bool zContrail(EA::Reflection::Int8 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(zContrail, result);
    }
    const EA::Reflection::Int8 &zContrail() const {
        ATTRIB_CODEGEN_GETLAYOUT(zContrail);
    }
    bool SET_zContrail(const EA::Reflection::Int8 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(zContrail, input);
    }
    bool zSprite(TAttrib<EA::Reflection::Int8> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int8, 0xd8782943);
    }
    bool zSprite(EA::Reflection::Int8 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(zSprite, result);
    }
    const EA::Reflection::Int8 &zSprite() const {
        ATTRIB_CODEGEN_GETLAYOUT(zSprite);
    }
    bool SET_zSprite(const EA::Reflection::Int8 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(zSprite, input);
    }

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    fuelcell_emitter &ConvertFromInstance(Instance &src) {}
    const fuelcell_emitter &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key fuelcell_emitter = 0xb267a856;

}; // namespace ClassName

namespace Hash {
namespace fuelcell_emitter {

static const Key Colour1 = 0x54c99f82;
static const Key GravityDelta = 0x0802dd99;
static const Key GravityStart = 0xe652e2b6;
static const Key HeightStart = 0x4c141ed7;
static const Key LengthDelta = 0xa6762035;
static const Key LengthStart = 0x6bbc13ee;
static const Key Life = 0x81625b35;
static const Key LifeVariance = 0xefb4bb64;
static const Key NumParticles = 0xdc943cc9;
static const Key NumParticlesVariance = 0xd8165518;
static const Key Spin = 0xe643b999;
static const Key VelocityDelta = 0x4bfe9062;
static const Key VelocityInherit = 0x028d34e5;
static const Key VelocityStart = 0x1b6c6ce2;
static const Key VolumeCenter = 0x1bd3f18c;
static const Key VolumeExtent = 0x0aaf558c;
static const Key emitteruv = 0xe4983a7d;
static const Key zContrail = 0x77f88e8c;
static const Key zSprite = 0xd8782943;

}; // namespace fuelcell_emitter
}; // namespace Hash

inline Key Gen::fuelcell_emitter::ClassKey() {
    return ClassName::fuelcell_emitter;
}

}; // namespace Attrib

#endif
