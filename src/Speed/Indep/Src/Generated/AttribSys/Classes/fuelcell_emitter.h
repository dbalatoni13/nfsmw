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
    bool Colour1(TAttrib<UMath::Vector4> &result) const;
    bool Colour1(UMath::Vector4 &result) const;
    const UMath::Vector4 &Colour1() const;
    bool SET_Colour1(const UMath::Vector4 &input);

    bool GravityDelta(TAttrib<EA::Reflection::Float> &result) const;
    bool GravityDelta(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &GravityDelta() const;
    bool SET_GravityDelta(const EA::Reflection::Float &input);

    bool GravityStart(TAttrib<EA::Reflection::Float> &result) const;
    bool GravityStart(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &GravityStart() const;
    bool SET_GravityStart(const EA::Reflection::Float &input);

    bool HeightStart(TAttrib<EA::Reflection::Float> &result) const;
    bool HeightStart(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &HeightStart() const;
    bool SET_HeightStart(const EA::Reflection::Float &input);

    bool LengthDelta(TAttrib<EA::Reflection::Float> &result) const;
    bool LengthDelta(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &LengthDelta() const;
    bool SET_LengthDelta(const EA::Reflection::Float &input);

    bool LengthStart(TAttrib<EA::Reflection::Float> &result) const;
    bool LengthStart(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &LengthStart() const;
    bool SET_LengthStart(const EA::Reflection::Float &input);

    bool Life(TAttrib<EA::Reflection::Float> &result) const;
    bool Life(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &Life() const;
    bool SET_Life(const EA::Reflection::Float &input);

    bool LifeVariance(TAttrib<EA::Reflection::Float> &result) const;
    bool LifeVariance(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &LifeVariance() const;
    bool SET_LifeVariance(const EA::Reflection::Float &input);

    bool NumParticles(TAttrib<EA::Reflection::Float> &result) const;
    bool NumParticles(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &NumParticles() const;
    bool SET_NumParticles(const EA::Reflection::Float &input);

    bool NumParticlesVariance(TAttrib<EA::Reflection::Float> &result) const;
    bool NumParticlesVariance(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &NumParticlesVariance() const;
    bool SET_NumParticlesVariance(const EA::Reflection::Float &input);

    bool Spin(TAttrib<EA::Reflection::Int16> &result) const;
    bool Spin(EA::Reflection::Int16 &result) const;
    const EA::Reflection::Int16 &Spin() const;
    bool SET_Spin(const EA::Reflection::Int16 &input);

    bool VelocityDelta(TAttrib<UMath::Vector4> &result) const;
    bool VelocityDelta(UMath::Vector4 &result) const;
    const UMath::Vector4 &VelocityDelta() const;
    bool SET_VelocityDelta(const UMath::Vector4 &input);

    bool VelocityInherit(TAttrib<UMath::Vector4> &result) const;
    bool VelocityInherit(UMath::Vector4 &result) const;
    const UMath::Vector4 &VelocityInherit() const;
    bool SET_VelocityInherit(const UMath::Vector4 &input);

    bool VelocityStart(TAttrib<UMath::Vector4> &result) const;
    bool VelocityStart(UMath::Vector4 &result) const;
    const UMath::Vector4 &VelocityStart() const;
    bool SET_VelocityStart(const UMath::Vector4 &input);

    bool VolumeCenter(TAttrib<UMath::Vector4> &result) const;
    bool VolumeCenter(UMath::Vector4 &result) const;
    const UMath::Vector4 &VolumeCenter() const;
    bool SET_VolumeCenter(const UMath::Vector4 &input);

    bool VolumeExtent(TAttrib<UMath::Vector4> &result) const;
    bool VolumeExtent(UMath::Vector4 &result) const;
    const UMath::Vector4 &VolumeExtent() const;
    bool SET_VolumeExtent(const UMath::Vector4 &input);

    bool emitteruv(TAttrib<RefSpec> &result) const;
    bool emitteruv(RefSpec &result) const;
    const RefSpec &emitteruv() const;
    bool SET_emitteruv(const RefSpec &input);

    bool zContrail(TAttrib<EA::Reflection::Int8> &result) const;
    bool zContrail(EA::Reflection::Int8 &result) const;
    const EA::Reflection::Int8 &zContrail() const;
    bool SET_zContrail(const EA::Reflection::Int8 &input);

    bool zSprite(TAttrib<EA::Reflection::Int8> &result) const;
    bool zSprite(EA::Reflection::Int8 &result) const;
    const EA::Reflection::Int8 &zSprite() const;
    bool SET_zSprite(const EA::Reflection::Int8 &input);

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

inline bool Gen::fuelcell_emitter::Colour1(TAttrib<UMath::Vector4> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x54c99f82);
}
inline const UMath::Vector4 &Gen::fuelcell_emitter::Colour1() const {
    ATTRIB_CODEGEN_GETLAYOUT(Colour1);
}
inline bool Gen::fuelcell_emitter::Colour1(UMath::Vector4 &result) const {
    result = Colour1();
    return true;
}
inline bool Gen::fuelcell_emitter::SET_Colour1(const UMath::Vector4 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(Colour1, input);
}

inline bool Gen::fuelcell_emitter::GravityDelta(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x0802dd99);
}
inline const EA::Reflection::Float &Gen::fuelcell_emitter::GravityDelta() const {
    ATTRIB_CODEGEN_GETLAYOUT(GravityDelta);
}
inline bool Gen::fuelcell_emitter::GravityDelta(EA::Reflection::Float &result) const {
    result = GravityDelta();
    return true;
}
inline bool Gen::fuelcell_emitter::SET_GravityDelta(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(GravityDelta, input);
}

inline bool Gen::fuelcell_emitter::GravityStart(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xe652e2b6);
}
inline const EA::Reflection::Float &Gen::fuelcell_emitter::GravityStart() const {
    ATTRIB_CODEGEN_GETLAYOUT(GravityStart);
}
inline bool Gen::fuelcell_emitter::GravityStart(EA::Reflection::Float &result) const {
    result = GravityStart();
    return true;
}
inline bool Gen::fuelcell_emitter::SET_GravityStart(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(GravityStart, input);
}

inline bool Gen::fuelcell_emitter::HeightStart(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x4c141ed7);
}
inline const EA::Reflection::Float &Gen::fuelcell_emitter::HeightStart() const {
    ATTRIB_CODEGEN_GETLAYOUT(HeightStart);
}
inline bool Gen::fuelcell_emitter::HeightStart(EA::Reflection::Float &result) const {
    result = HeightStart();
    return true;
}
inline bool Gen::fuelcell_emitter::SET_HeightStart(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(HeightStart, input);
}

inline bool Gen::fuelcell_emitter::LengthDelta(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xa6762035);
}
inline const EA::Reflection::Float &Gen::fuelcell_emitter::LengthDelta() const {
    ATTRIB_CODEGEN_GETLAYOUT(LengthDelta);
}
inline bool Gen::fuelcell_emitter::LengthDelta(EA::Reflection::Float &result) const {
    result = LengthDelta();
    return true;
}
inline bool Gen::fuelcell_emitter::SET_LengthDelta(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(LengthDelta, input);
}

inline bool Gen::fuelcell_emitter::LengthStart(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x6bbc13ee);
}
inline const EA::Reflection::Float &Gen::fuelcell_emitter::LengthStart() const {
    ATTRIB_CODEGEN_GETLAYOUT(LengthStart);
}
inline bool Gen::fuelcell_emitter::LengthStart(EA::Reflection::Float &result) const {
    result = LengthStart();
    return true;
}
inline bool Gen::fuelcell_emitter::SET_LengthStart(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(LengthStart, input);
}

inline bool Gen::fuelcell_emitter::Life(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x81625b35);
}
inline const EA::Reflection::Float &Gen::fuelcell_emitter::Life() const {
    ATTRIB_CODEGEN_GETLAYOUT(Life);
}
inline bool Gen::fuelcell_emitter::Life(EA::Reflection::Float &result) const {
    result = Life();
    return true;
}
inline bool Gen::fuelcell_emitter::SET_Life(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(Life, input);
}

inline bool Gen::fuelcell_emitter::LifeVariance(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xefb4bb64);
}
inline const EA::Reflection::Float &Gen::fuelcell_emitter::LifeVariance() const {
    ATTRIB_CODEGEN_GETLAYOUT(LifeVariance);
}
inline bool Gen::fuelcell_emitter::LifeVariance(EA::Reflection::Float &result) const {
    result = LifeVariance();
    return true;
}
inline bool Gen::fuelcell_emitter::SET_LifeVariance(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(LifeVariance, input);
}

inline bool Gen::fuelcell_emitter::NumParticles(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xdc943cc9);
}
inline const EA::Reflection::Float &Gen::fuelcell_emitter::NumParticles() const {
    ATTRIB_CODEGEN_GETLAYOUT(NumParticles);
}
inline bool Gen::fuelcell_emitter::NumParticles(EA::Reflection::Float &result) const {
    result = NumParticles();
    return true;
}
inline bool Gen::fuelcell_emitter::SET_NumParticles(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(NumParticles, input);
}

inline bool Gen::fuelcell_emitter::NumParticlesVariance(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xd8165518);
}
inline const EA::Reflection::Float &Gen::fuelcell_emitter::NumParticlesVariance() const {
    ATTRIB_CODEGEN_GETLAYOUT(NumParticlesVariance);
}
inline bool Gen::fuelcell_emitter::NumParticlesVariance(EA::Reflection::Float &result) const {
    result = NumParticlesVariance();
    return true;
}
inline bool Gen::fuelcell_emitter::SET_NumParticlesVariance(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(NumParticlesVariance, input);
}

inline bool Gen::fuelcell_emitter::Spin(TAttrib<EA::Reflection::Int16> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int16, 0xe643b999);
}
inline const EA::Reflection::Int16 &Gen::fuelcell_emitter::Spin() const {
    ATTRIB_CODEGEN_GETLAYOUT(Spin);
}
inline bool Gen::fuelcell_emitter::Spin(EA::Reflection::Int16 &result) const {
    result = Spin();
    return true;
}
inline bool Gen::fuelcell_emitter::SET_Spin(const EA::Reflection::Int16 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(Spin, input);
}

inline bool Gen::fuelcell_emitter::VelocityDelta(TAttrib<UMath::Vector4> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x4bfe9062);
}
inline const UMath::Vector4 &Gen::fuelcell_emitter::VelocityDelta() const {
    ATTRIB_CODEGEN_GETLAYOUT(VelocityDelta);
}
inline bool Gen::fuelcell_emitter::VelocityDelta(UMath::Vector4 &result) const {
    result = VelocityDelta();
    return true;
}
inline bool Gen::fuelcell_emitter::SET_VelocityDelta(const UMath::Vector4 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(VelocityDelta, input);
}

inline bool Gen::fuelcell_emitter::VelocityInherit(TAttrib<UMath::Vector4> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x028d34e5);
}
inline const UMath::Vector4 &Gen::fuelcell_emitter::VelocityInherit() const {
    ATTRIB_CODEGEN_GETLAYOUT(VelocityInherit);
}
inline bool Gen::fuelcell_emitter::VelocityInherit(UMath::Vector4 &result) const {
    result = VelocityInherit();
    return true;
}
inline bool Gen::fuelcell_emitter::SET_VelocityInherit(const UMath::Vector4 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(VelocityInherit, input);
}

inline bool Gen::fuelcell_emitter::VelocityStart(TAttrib<UMath::Vector4> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x1b6c6ce2);
}
inline const UMath::Vector4 &Gen::fuelcell_emitter::VelocityStart() const {
    ATTRIB_CODEGEN_GETLAYOUT(VelocityStart);
}
inline bool Gen::fuelcell_emitter::VelocityStart(UMath::Vector4 &result) const {
    result = VelocityStart();
    return true;
}
inline bool Gen::fuelcell_emitter::SET_VelocityStart(const UMath::Vector4 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(VelocityStart, input);
}

inline bool Gen::fuelcell_emitter::VolumeCenter(TAttrib<UMath::Vector4> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x1bd3f18c);
}
inline const UMath::Vector4 &Gen::fuelcell_emitter::VolumeCenter() const {
    ATTRIB_CODEGEN_GETLAYOUT(VolumeCenter);
}
inline bool Gen::fuelcell_emitter::VolumeCenter(UMath::Vector4 &result) const {
    result = VolumeCenter();
    return true;
}
inline bool Gen::fuelcell_emitter::SET_VolumeCenter(const UMath::Vector4 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(VolumeCenter, input);
}

inline bool Gen::fuelcell_emitter::VolumeExtent(TAttrib<UMath::Vector4> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x0aaf558c);
}
inline const UMath::Vector4 &Gen::fuelcell_emitter::VolumeExtent() const {
    ATTRIB_CODEGEN_GETLAYOUT(VolumeExtent);
}
inline bool Gen::fuelcell_emitter::VolumeExtent(UMath::Vector4 &result) const {
    result = VolumeExtent();
    return true;
}
inline bool Gen::fuelcell_emitter::SET_VolumeExtent(const UMath::Vector4 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(VolumeExtent, input);
}

inline bool Gen::fuelcell_emitter::emitteruv(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0xe4983a7d);
}
inline const RefSpec &Gen::fuelcell_emitter::emitteruv() const {
    ATTRIB_CODEGEN_GETLAYOUT(emitteruv);
}
inline bool Gen::fuelcell_emitter::emitteruv(RefSpec &result) const {
    result = emitteruv();
    return true;
}
inline bool Gen::fuelcell_emitter::SET_emitteruv(const RefSpec &input) {
    ATTRIB_CODEGEN_SETLAYOUT(emitteruv, input);
}

inline bool Gen::fuelcell_emitter::zContrail(TAttrib<EA::Reflection::Int8> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int8, 0x77f88e8c);
}
inline const EA::Reflection::Int8 &Gen::fuelcell_emitter::zContrail() const {
    ATTRIB_CODEGEN_GETLAYOUT(zContrail);
}
inline bool Gen::fuelcell_emitter::zContrail(EA::Reflection::Int8 &result) const {
    result = zContrail();
    return true;
}
inline bool Gen::fuelcell_emitter::SET_zContrail(const EA::Reflection::Int8 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(zContrail, input);
}

inline bool Gen::fuelcell_emitter::zSprite(TAttrib<EA::Reflection::Int8> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int8, 0xd8782943);
}
inline const EA::Reflection::Int8 &Gen::fuelcell_emitter::zSprite() const {
    ATTRIB_CODEGEN_GETLAYOUT(zSprite);
}
inline bool Gen::fuelcell_emitter::zSprite(EA::Reflection::Int8 &result) const {
    result = zSprite();
    return true;
}
inline bool Gen::fuelcell_emitter::SET_zSprite(const EA::Reflection::Int8 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(zSprite, input);
}

}; // namespace Attrib

#endif
