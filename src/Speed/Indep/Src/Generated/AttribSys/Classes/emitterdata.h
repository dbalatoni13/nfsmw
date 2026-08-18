#ifndef _attrib_gen_emitterdata_h
#define _attrib_gen_emitterdata_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct emitterdata : Instance {
    struct _LayoutStruct {
        UMath::Vector4 VelocityStart;                        // offset 0x0, size 0x10
        UMath::Vector4 AccelDelta;                           // offset 0x10, size 0x10
        UMath::Vector4 RelativeAngle;                        // offset 0x20, size 0x10
        UMath::Vector4 VelocityDelta;                        // offset 0x30, size 0x10
        UMath::Vector4 Size;                                 // offset 0x40, size 0x10
        UMath::Vector4 KeyPositions;                         // offset 0x50, size 0x10
        UMath::Vector4 VolumeExtent;                         // offset 0x60, size 0x10
        UMath::Vector4 AccelStart;                           // offset 0x70, size 0x10
        UMath::Vector4 VolumeCenter;                         // offset 0x80, size 0x10
        ParticleAnimationInfo TextureAnimation;              // offset 0x90, size 0x8
        ParticleTextureRecord Texture;                       // offset 0x98, size 0x8
        EA::Reflection::Text CollectionName;                 // offset 0xa0, size 0x4
        EA::Reflection::Float InitialAngleRange;             // offset 0xa4, size 0x4
        EA::Reflection::Float Drag;                          // offset 0xa8, size 0x4
        EA::Reflection::UInt32 Color1;                       // offset 0xac, size 0x4
        EA::Reflection::UInt32 Color3;                       // offset 0xb0, size 0x4
        EA::Reflection::UInt32 Color4;                       // offset 0xb4, size 0x4
        EA::Reflection::Float OnCycle;                       // offset 0xb8, size 0x4
        EA::Reflection::Float Life;                          // offset 0xbc, size 0x4
        EA::Reflection::Float NumParticlesVariance;          // offset 0xc0, size 0x4
        EA::Reflection::Float NumParticles;                  // offset 0xc4, size 0x4
        EffectParticleConstraint AxisConstraint;             // offset 0xc8, size 0x4
        EA::Reflection::Float OnCycleVariance;               // offset 0xcc, size 0x4
        EA::Reflection::Float OffCycleVariance;              // offset 0xd0, size 0x4
        EA::Reflection::Float FarClip;                       // offset 0xd4, size 0x4
        EA::Reflection::Float OffCycle;                      // offset 0xd8, size 0x4
        EA::Reflection::Float RotationVariance;              // offset 0xdc, size 0x4
        EA::Reflection::Int32 MotionLive;                    // offset 0xe0, size 0x4
        EA::Reflection::UInt32 Color2;                       // offset 0xe4, size 0x4
        EA::Reflection::Float LifeVariance;                  // offset 0xe8, size 0x4
        EA::Reflection::Float MotionInherit;                 // offset 0xec, size 0x4
        EA::Reflection::Float Speed;                         // offset 0xf0, size 0x4
        EA::Reflection::Float StartDelay;                    // offset 0xf4, size 0x4
        EA::Reflection::Float SpreadAngle;                   // offset 0xf8, size 0x4
        EA::Reflection::Int32 RandomRotationDirection;       // offset 0xfc, size 0x4
        EA::Reflection::Float SpeedVariance;                 // offset 0x100, size 0x4
        EA::Reflection::Float MotionInheritVariance;         // offset 0x104, size 0x4
        EA::Reflection::Float Gravity;                       // offset 0x108, size 0x4
        EA::Reflection::Int32 SpreadAsDisc;                  // offset 0x10c, size 0x4
        EA::Reflection::Int8 AlphaToKillAt;                  // offset 0x110, size 0x1
        EA::Reflection::Bool NoKillAtAlpha;                  // offset 0x111, size 0x1
        EA::Reflection::Bool IsOneShot;                      // offset 0x112, size 0x1
        EA::Reflection::Bool StartDelayRandomVariance;       // offset 0x113, size 0x1
        EA::Reflection::Bool EliminateUnnecessaryRandomness; // offset 0x114, size 0x1
    };

    typedef UMath::Vector4 TypeOf_AccelDelta;
    typedef UMath::Vector4 TypeOf_AccelStart;
    typedef EA::Reflection::Int8 TypeOf_AlphaToKillAt;
    typedef EffectParticleConstraint TypeOf_AxisConstraint;
    typedef EA::Reflection::Text TypeOf_CollectionName;
    typedef EA::Reflection::UInt32 TypeOf_Color1;
    typedef EA::Reflection::UInt32 TypeOf_Color2;
    typedef EA::Reflection::UInt32 TypeOf_Color3;
    typedef EA::Reflection::UInt32 TypeOf_Color4;
    typedef EA::Reflection::Float TypeOf_Drag;
    typedef EA::Reflection::Bool TypeOf_EliminateUnnecessaryRandomness;
    typedef EA::Reflection::Float TypeOf_FarClip;
    typedef EA::Reflection::Float TypeOf_Gravity;
    typedef EA::Reflection::Float TypeOf_InitialAngleRange;
    typedef EA::Reflection::Bool TypeOf_IsOneShot;
    typedef UMath::Vector4 TypeOf_KeyPositions;
    typedef EA::Reflection::Float TypeOf_Life;
    typedef EA::Reflection::Float TypeOf_LifeVariance;
    typedef EA::Reflection::Float TypeOf_MotionInherit;
    typedef EA::Reflection::Float TypeOf_MotionInheritVariance;
    typedef EA::Reflection::Int32 TypeOf_MotionLive;
    typedef EA::Reflection::Bool TypeOf_NoKillAtAlpha;
    typedef EA::Reflection::Float TypeOf_NumParticles;
    typedef EA::Reflection::Float TypeOf_NumParticlesVariance;
    typedef EA::Reflection::Float TypeOf_OffCycle;
    typedef EA::Reflection::Float TypeOf_OffCycleVariance;
    typedef EA::Reflection::Float TypeOf_OnCycle;
    typedef EA::Reflection::Float TypeOf_OnCycleVariance;
    typedef EA::Reflection::Int32 TypeOf_RandomRotationDirection;
    typedef UMath::Vector4 TypeOf_RelativeAngle;
    typedef EA::Reflection::Float TypeOf_RotationVariance;
    typedef UMath::Vector4 TypeOf_Size;
    typedef EA::Reflection::Float TypeOf_Speed;
    typedef EA::Reflection::Float TypeOf_SpeedVariance;
    typedef EA::Reflection::Float TypeOf_SpreadAngle;
    typedef EA::Reflection::Int32 TypeOf_SpreadAsDisc;
    typedef EA::Reflection::Float TypeOf_StartDelay;
    typedef EA::Reflection::Bool TypeOf_StartDelayRandomVariance;
    typedef ParticleTextureRecord TypeOf_Texture;
    typedef ParticleAnimationInfo TypeOf_TextureAnimation;
    typedef UMath::Vector4 TypeOf_VelocityDelta;
    typedef UMath::Vector4 TypeOf_VelocityStart;
    typedef UMath::Vector4 TypeOf_VolumeCenter;
    typedef UMath::Vector4 TypeOf_VolumeExtent;
    typedef RefSpec TypeOf_XenonEffect;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("emitterdata");
    emitterdata(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    emitterdata(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    emitterdata(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    emitterdata(const emitterdata &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    emitterdata(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~emitterdata() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0xb30b18af;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0xb30b18af, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const emitterdata &operator=(const emitterdata &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const emitterdata &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool AccelDelta(UMath::Vector4 &result) const;
    const UMath::Vector4 &AccelDelta() const;
    bool SET_AccelDelta(const UMath::Vector4 &input);

    bool AccelStart(UMath::Vector4 &result) const;
    const UMath::Vector4 &AccelStart() const;
    bool SET_AccelStart(const UMath::Vector4 &input);

    bool AlphaToKillAt(EA::Reflection::Int8 &result) const;
    const EA::Reflection::Int8 &AlphaToKillAt() const;
    bool SET_AlphaToKillAt(const EA::Reflection::Int8 &input);

    bool AxisConstraint(EffectParticleConstraint &result) const;
    const EffectParticleConstraint &AxisConstraint() const;
    bool SET_AxisConstraint(const EffectParticleConstraint &input);

    bool CollectionName(TAttrib<EA::Reflection::Text> &result) const;
    bool CollectionName(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &CollectionName() const;
    bool SET_CollectionName(const EA::Reflection::Text &input);

    bool Color1(EA::Reflection::UInt32 &result) const;
    const EA::Reflection::UInt32 &Color1() const;
    bool SET_Color1(const EA::Reflection::UInt32 &input);

    bool Color2(EA::Reflection::UInt32 &result) const;
    const EA::Reflection::UInt32 &Color2() const;
    bool SET_Color2(const EA::Reflection::UInt32 &input);

    bool Color3(EA::Reflection::UInt32 &result) const;
    const EA::Reflection::UInt32 &Color3() const;
    bool SET_Color3(const EA::Reflection::UInt32 &input);

    bool Color4(EA::Reflection::UInt32 &result) const;
    const EA::Reflection::UInt32 &Color4() const;
    bool SET_Color4(const EA::Reflection::UInt32 &input);

    bool Drag(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &Drag() const;
    bool SET_Drag(const EA::Reflection::Float &input);

    bool EliminateUnnecessaryRandomness(TAttrib<EA::Reflection::Bool> &result) const;
    bool EliminateUnnecessaryRandomness(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &EliminateUnnecessaryRandomness() const;
    bool SET_EliminateUnnecessaryRandomness(const EA::Reflection::Bool &input);

    bool FarClip(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &FarClip() const;
    bool SET_FarClip(const EA::Reflection::Float &input);

    bool Gravity(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &Gravity() const;
    bool SET_Gravity(const EA::Reflection::Float &input);

    bool InitialAngleRange(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &InitialAngleRange() const;
    bool SET_InitialAngleRange(const EA::Reflection::Float &input);

    bool IsOneShot(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &IsOneShot() const;
    bool SET_IsOneShot(const EA::Reflection::Bool &input);

    bool KeyPositions(UMath::Vector4 &result) const;
    const UMath::Vector4 &KeyPositions() const;
    bool SET_KeyPositions(const UMath::Vector4 &input);

    bool Life(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &Life() const;
    bool SET_Life(const EA::Reflection::Float &input);

    bool LifeVariance(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &LifeVariance() const;
    bool SET_LifeVariance(const EA::Reflection::Float &input);

    bool MotionInherit(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &MotionInherit() const;
    bool SET_MotionInherit(const EA::Reflection::Float &input);

    bool MotionInheritVariance(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &MotionInheritVariance() const;
    bool SET_MotionInheritVariance(const EA::Reflection::Float &input);

    bool MotionLive(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &MotionLive() const;
    bool SET_MotionLive(const EA::Reflection::Int32 &input);

    bool NoKillAtAlpha(TAttrib<EA::Reflection::Bool> &result) const;
    bool NoKillAtAlpha(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &NoKillAtAlpha() const;
    bool SET_NoKillAtAlpha(const EA::Reflection::Bool &input);

    bool NumParticles(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &NumParticles() const;
    bool SET_NumParticles(const EA::Reflection::Float &input);

    bool NumParticlesVariance(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &NumParticlesVariance() const;
    bool SET_NumParticlesVariance(const EA::Reflection::Float &input);

    bool OffCycle(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &OffCycle() const;
    bool SET_OffCycle(const EA::Reflection::Float &input);

    bool OffCycleVariance(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &OffCycleVariance() const;
    bool SET_OffCycleVariance(const EA::Reflection::Float &input);

    bool OnCycle(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &OnCycle() const;
    bool SET_OnCycle(const EA::Reflection::Float &input);

    bool OnCycleVariance(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &OnCycleVariance() const;
    bool SET_OnCycleVariance(const EA::Reflection::Float &input);

    bool RandomRotationDirection(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &RandomRotationDirection() const;
    bool SET_RandomRotationDirection(const EA::Reflection::Int32 &input);

    bool RelativeAngle(UMath::Vector4 &result) const;
    const UMath::Vector4 &RelativeAngle() const;
    bool SET_RelativeAngle(const UMath::Vector4 &input);

    bool RotationVariance(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &RotationVariance() const;
    bool SET_RotationVariance(const EA::Reflection::Float &input);

    bool Size(UMath::Vector4 &result) const;
    const UMath::Vector4 &Size() const;
    bool SET_Size(const UMath::Vector4 &input);

    bool Speed(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &Speed() const;
    bool SET_Speed(const EA::Reflection::Float &input);

    bool SpeedVariance(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &SpeedVariance() const;
    bool SET_SpeedVariance(const EA::Reflection::Float &input);

    bool SpreadAngle(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &SpreadAngle() const;
    bool SET_SpreadAngle(const EA::Reflection::Float &input);

    bool SpreadAsDisc(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &SpreadAsDisc() const;
    bool SET_SpreadAsDisc(const EA::Reflection::Int32 &input);

    bool StartDelay(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &StartDelay() const;
    bool SET_StartDelay(const EA::Reflection::Float &input);

    bool StartDelayRandomVariance(TAttrib<EA::Reflection::Bool> &result) const;
    bool StartDelayRandomVariance(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &StartDelayRandomVariance() const;
    bool SET_StartDelayRandomVariance(const EA::Reflection::Bool &input);

    bool Texture(TAttrib<ParticleTextureRecord> &result) const;
    bool Texture(ParticleTextureRecord &result) const;
    const ParticleTextureRecord &Texture() const;
    bool SET_Texture(const ParticleTextureRecord &input);

    bool TextureAnimation(ParticleAnimationInfo &result) const;
    const ParticleAnimationInfo &TextureAnimation() const;
    bool SET_TextureAnimation(const ParticleAnimationInfo &input);

    bool VelocityDelta(UMath::Vector4 &result) const;
    const UMath::Vector4 &VelocityDelta() const;
    bool SET_VelocityDelta(const UMath::Vector4 &input);

    bool VelocityStart(UMath::Vector4 &result) const;
    const UMath::Vector4 &VelocityStart() const;
    bool SET_VelocityStart(const UMath::Vector4 &input);

    bool VolumeCenter(UMath::Vector4 &result) const;
    const UMath::Vector4 &VolumeCenter() const;
    bool SET_VolumeCenter(const UMath::Vector4 &input);

    bool VolumeExtent(UMath::Vector4 &result) const;
    const UMath::Vector4 &VolumeExtent() const;
    bool SET_VolumeExtent(const UMath::Vector4 &input);

    bool XenonEffect(TAttrib<RefSpec> &result) const;
    bool XenonEffect(RefSpec &result, unsigned int index) const;
    const RefSpec &XenonEffect(unsigned int index) const;
    unsigned int Num_XenonEffect() const;
    bool SET_XenonEffect(const RefSpec &input, unsigned int index);

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    emitterdata &ConvertFromInstance(Instance &src) {}
    const emitterdata &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key emitterdata = 0xb30b18af;

}; // namespace ClassName

namespace Hash {
namespace emitterdata {

static const Key AccelDelta = 0x81167023;
static const Key AccelStart = 0x81222c15;
static const Key AlphaToKillAt = 0x9ec0561b;
static const Key AxisConstraint = 0x78d7facd;
static const Key CollectionName = 0x9ca1c8f9;
static const Key Color1 = 0xccf41b18;
static const Key Color2 = 0x4a282af1;
static const Key Color3 = 0xc103b771;
static const Key Color4 = 0x78f32c40;
static const Key Drag = 0x4e58fa4e;
static const Key EliminateUnnecessaryRandomness = 0x859043b2;
static const Key FarClip = 0x66052349;
static const Key Gravity = 0xac5b265e;
static const Key InitialAngleRange = 0x284a8c2c;
static const Key IsOneShot = 0xb2659b2a;
static const Key KeyPositions = 0x62fe3594;
static const Key Life = 0x81625b35;
static const Key LifeVariance = 0xefb4bb64;
static const Key MotionInherit = 0x6bcbfc06;
static const Key MotionInheritVariance = 0xd34a0b9d;
static const Key MotionLive = 0x4d69ef9e;
static const Key NoKillAtAlpha = 0x27a9a451;
static const Key NumParticles = 0xdc943cc9;
static const Key NumParticlesVariance = 0xd8165518;
static const Key OffCycle = 0xb619ecb2;
static const Key OffCycleVariance = 0x8510a6ef;
static const Key OnCycle = 0xeb86a538;
static const Key OnCycleVariance = 0x3b53b68c;
static const Key RandomRotationDirection = 0x0be45b47;
static const Key RelativeAngle = 0xcd0d867f;
static const Key RotationVariance = 0x31af20d6;
static const Key Size = 0xc10c3e15;
static const Key Speed = 0x41862fe6;
static const Key SpeedVariance = 0x58fcb1c3;
static const Key SpreadAngle = 0xcac30ff2;
static const Key SpreadAsDisc = 0x0836f5cc;
static const Key StartDelay = 0xee67ad35;
static const Key StartDelayRandomVariance = 0x6e088259;
static const Key Texture = 0x6f3afee5;
static const Key TextureAnimation = 0x92709587;
static const Key VelocityDelta = 0x4bfe9062;
static const Key VelocityStart = 0x1b6c6ce2;
static const Key VolumeCenter = 0x1bd3f18c;
static const Key VolumeExtent = 0x0aaf558c;
static const Key XenonEffect = 0xfe40e637;

}; // namespace emitterdata
}; // namespace Hash

inline Key Gen::emitterdata::ClassKey() {
    return ClassName::emitterdata;
}

inline const UMath::Vector4 &Gen::emitterdata::AccelDelta() const {
    ATTRIB_CODEGEN_GETLAYOUT(AccelDelta);
}
inline bool Gen::emitterdata::AccelDelta(UMath::Vector4 &result) const {
    result = AccelDelta();
    return true;
}
inline bool Gen::emitterdata::SET_AccelDelta(const UMath::Vector4 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(AccelDelta, input);
}

inline const UMath::Vector4 &Gen::emitterdata::AccelStart() const {
    ATTRIB_CODEGEN_GETLAYOUT(AccelStart);
}
inline bool Gen::emitterdata::AccelStart(UMath::Vector4 &result) const {
    result = AccelStart();
    return true;
}
inline bool Gen::emitterdata::SET_AccelStart(const UMath::Vector4 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(AccelStart, input);
}

inline const EA::Reflection::Int8 &Gen::emitterdata::AlphaToKillAt() const {
    ATTRIB_CODEGEN_GETLAYOUT(AlphaToKillAt);
}
inline bool Gen::emitterdata::AlphaToKillAt(EA::Reflection::Int8 &result) const {
    result = AlphaToKillAt();
    return true;
}
inline bool Gen::emitterdata::SET_AlphaToKillAt(const EA::Reflection::Int8 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(AlphaToKillAt, input);
}

inline const EffectParticleConstraint &Gen::emitterdata::AxisConstraint() const {
    ATTRIB_CODEGEN_GETLAYOUT(AxisConstraint);
}
inline bool Gen::emitterdata::AxisConstraint(EffectParticleConstraint &result) const {
    result = AxisConstraint();
    return true;
}
inline bool Gen::emitterdata::SET_AxisConstraint(const EffectParticleConstraint &input) {
    ATTRIB_CODEGEN_SETLAYOUT(AxisConstraint, input);
}

inline bool Gen::emitterdata::CollectionName(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x9ca1c8f9);
}
inline const EA::Reflection::Text &Gen::emitterdata::CollectionName() const {
    ATTRIB_CODEGEN_GETLAYOUT(CollectionName);
}
inline bool Gen::emitterdata::CollectionName(EA::Reflection::Text &result) const {
    result = CollectionName();
    return true;
}
inline bool Gen::emitterdata::SET_CollectionName(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETLAYOUT(CollectionName, input);
}

inline const EA::Reflection::UInt32 &Gen::emitterdata::Color1() const {
    ATTRIB_CODEGEN_GETLAYOUT(Color1);
}
inline bool Gen::emitterdata::Color1(EA::Reflection::UInt32 &result) const {
    result = Color1();
    return true;
}
inline bool Gen::emitterdata::SET_Color1(const EA::Reflection::UInt32 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(Color1, input);
}

inline const EA::Reflection::UInt32 &Gen::emitterdata::Color2() const {
    ATTRIB_CODEGEN_GETLAYOUT(Color2);
}
inline bool Gen::emitterdata::Color2(EA::Reflection::UInt32 &result) const {
    result = Color2();
    return true;
}
inline bool Gen::emitterdata::SET_Color2(const EA::Reflection::UInt32 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(Color2, input);
}

inline const EA::Reflection::UInt32 &Gen::emitterdata::Color3() const {
    ATTRIB_CODEGEN_GETLAYOUT(Color3);
}
inline bool Gen::emitterdata::Color3(EA::Reflection::UInt32 &result) const {
    result = Color3();
    return true;
}
inline bool Gen::emitterdata::SET_Color3(const EA::Reflection::UInt32 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(Color3, input);
}

inline const EA::Reflection::UInt32 &Gen::emitterdata::Color4() const {
    ATTRIB_CODEGEN_GETLAYOUT(Color4);
}
inline bool Gen::emitterdata::Color4(EA::Reflection::UInt32 &result) const {
    result = Color4();
    return true;
}
inline bool Gen::emitterdata::SET_Color4(const EA::Reflection::UInt32 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(Color4, input);
}

inline const EA::Reflection::Float &Gen::emitterdata::Drag() const {
    ATTRIB_CODEGEN_GETLAYOUT(Drag);
}
inline bool Gen::emitterdata::Drag(EA::Reflection::Float &result) const {
    result = Drag();
    return true;
}
inline bool Gen::emitterdata::SET_Drag(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(Drag, input);
}

inline bool Gen::emitterdata::EliminateUnnecessaryRandomness(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x859043b2);
}
inline const EA::Reflection::Bool &Gen::emitterdata::EliminateUnnecessaryRandomness() const {
    ATTRIB_CODEGEN_GETLAYOUT(EliminateUnnecessaryRandomness);
}
inline bool Gen::emitterdata::EliminateUnnecessaryRandomness(EA::Reflection::Bool &result) const {
    result = EliminateUnnecessaryRandomness();
    return true;
}
inline bool Gen::emitterdata::SET_EliminateUnnecessaryRandomness(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETLAYOUT(EliminateUnnecessaryRandomness, input);
}

inline const EA::Reflection::Float &Gen::emitterdata::FarClip() const {
    ATTRIB_CODEGEN_GETLAYOUT(FarClip);
}
inline bool Gen::emitterdata::FarClip(EA::Reflection::Float &result) const {
    result = FarClip();
    return true;
}
inline bool Gen::emitterdata::SET_FarClip(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(FarClip, input);
}

inline const EA::Reflection::Float &Gen::emitterdata::Gravity() const {
    ATTRIB_CODEGEN_GETLAYOUT(Gravity);
}
inline bool Gen::emitterdata::Gravity(EA::Reflection::Float &result) const {
    result = Gravity();
    return true;
}
inline bool Gen::emitterdata::SET_Gravity(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(Gravity, input);
}

inline const EA::Reflection::Float &Gen::emitterdata::InitialAngleRange() const {
    ATTRIB_CODEGEN_GETLAYOUT(InitialAngleRange);
}
inline bool Gen::emitterdata::InitialAngleRange(EA::Reflection::Float &result) const {
    result = InitialAngleRange();
    return true;
}
inline bool Gen::emitterdata::SET_InitialAngleRange(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(InitialAngleRange, input);
}

inline const EA::Reflection::Bool &Gen::emitterdata::IsOneShot() const {
    ATTRIB_CODEGEN_GETLAYOUT(IsOneShot);
}
inline bool Gen::emitterdata::IsOneShot(EA::Reflection::Bool &result) const {
    result = IsOneShot();
    return true;
}
inline bool Gen::emitterdata::SET_IsOneShot(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETLAYOUT(IsOneShot, input);
}

inline const UMath::Vector4 &Gen::emitterdata::KeyPositions() const {
    ATTRIB_CODEGEN_GETLAYOUT(KeyPositions);
}
inline bool Gen::emitterdata::KeyPositions(UMath::Vector4 &result) const {
    result = KeyPositions();
    return true;
}
inline bool Gen::emitterdata::SET_KeyPositions(const UMath::Vector4 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(KeyPositions, input);
}

inline const EA::Reflection::Float &Gen::emitterdata::Life() const {
    ATTRIB_CODEGEN_GETLAYOUT(Life);
}
inline bool Gen::emitterdata::Life(EA::Reflection::Float &result) const {
    result = Life();
    return true;
}
inline bool Gen::emitterdata::SET_Life(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(Life, input);
}

inline const EA::Reflection::Float &Gen::emitterdata::LifeVariance() const {
    ATTRIB_CODEGEN_GETLAYOUT(LifeVariance);
}
inline bool Gen::emitterdata::LifeVariance(EA::Reflection::Float &result) const {
    result = LifeVariance();
    return true;
}
inline bool Gen::emitterdata::SET_LifeVariance(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(LifeVariance, input);
}

inline const EA::Reflection::Float &Gen::emitterdata::MotionInherit() const {
    ATTRIB_CODEGEN_GETLAYOUT(MotionInherit);
}
inline bool Gen::emitterdata::MotionInherit(EA::Reflection::Float &result) const {
    result = MotionInherit();
    return true;
}
inline bool Gen::emitterdata::SET_MotionInherit(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(MotionInherit, input);
}

inline const EA::Reflection::Float &Gen::emitterdata::MotionInheritVariance() const {
    ATTRIB_CODEGEN_GETLAYOUT(MotionInheritVariance);
}
inline bool Gen::emitterdata::MotionInheritVariance(EA::Reflection::Float &result) const {
    result = MotionInheritVariance();
    return true;
}
inline bool Gen::emitterdata::SET_MotionInheritVariance(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(MotionInheritVariance, input);
}

inline const EA::Reflection::Int32 &Gen::emitterdata::MotionLive() const {
    ATTRIB_CODEGEN_GETLAYOUT(MotionLive);
}
inline bool Gen::emitterdata::MotionLive(EA::Reflection::Int32 &result) const {
    result = MotionLive();
    return true;
}
inline bool Gen::emitterdata::SET_MotionLive(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(MotionLive, input);
}

inline bool Gen::emitterdata::NoKillAtAlpha(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x27a9a451);
}
inline const EA::Reflection::Bool &Gen::emitterdata::NoKillAtAlpha() const {
    ATTRIB_CODEGEN_GETLAYOUT(NoKillAtAlpha);
}
inline bool Gen::emitterdata::NoKillAtAlpha(EA::Reflection::Bool &result) const {
    result = NoKillAtAlpha();
    return true;
}
inline bool Gen::emitterdata::SET_NoKillAtAlpha(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETLAYOUT(NoKillAtAlpha, input);
}

inline const EA::Reflection::Float &Gen::emitterdata::NumParticles() const {
    ATTRIB_CODEGEN_GETLAYOUT(NumParticles);
}
inline bool Gen::emitterdata::NumParticles(EA::Reflection::Float &result) const {
    result = NumParticles();
    return true;
}
inline bool Gen::emitterdata::SET_NumParticles(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(NumParticles, input);
}

inline const EA::Reflection::Float &Gen::emitterdata::NumParticlesVariance() const {
    ATTRIB_CODEGEN_GETLAYOUT(NumParticlesVariance);
}
inline bool Gen::emitterdata::NumParticlesVariance(EA::Reflection::Float &result) const {
    result = NumParticlesVariance();
    return true;
}
inline bool Gen::emitterdata::SET_NumParticlesVariance(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(NumParticlesVariance, input);
}

inline const EA::Reflection::Float &Gen::emitterdata::OffCycle() const {
    ATTRIB_CODEGEN_GETLAYOUT(OffCycle);
}
inline bool Gen::emitterdata::OffCycle(EA::Reflection::Float &result) const {
    result = OffCycle();
    return true;
}
inline bool Gen::emitterdata::SET_OffCycle(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(OffCycle, input);
}

inline const EA::Reflection::Float &Gen::emitterdata::OffCycleVariance() const {
    ATTRIB_CODEGEN_GETLAYOUT(OffCycleVariance);
}
inline bool Gen::emitterdata::OffCycleVariance(EA::Reflection::Float &result) const {
    result = OffCycleVariance();
    return true;
}
inline bool Gen::emitterdata::SET_OffCycleVariance(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(OffCycleVariance, input);
}

inline const EA::Reflection::Float &Gen::emitterdata::OnCycle() const {
    ATTRIB_CODEGEN_GETLAYOUT(OnCycle);
}
inline bool Gen::emitterdata::OnCycle(EA::Reflection::Float &result) const {
    result = OnCycle();
    return true;
}
inline bool Gen::emitterdata::SET_OnCycle(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(OnCycle, input);
}

inline const EA::Reflection::Float &Gen::emitterdata::OnCycleVariance() const {
    ATTRIB_CODEGEN_GETLAYOUT(OnCycleVariance);
}
inline bool Gen::emitterdata::OnCycleVariance(EA::Reflection::Float &result) const {
    result = OnCycleVariance();
    return true;
}
inline bool Gen::emitterdata::SET_OnCycleVariance(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(OnCycleVariance, input);
}

inline const EA::Reflection::Int32 &Gen::emitterdata::RandomRotationDirection() const {
    ATTRIB_CODEGEN_GETLAYOUT(RandomRotationDirection);
}
inline bool Gen::emitterdata::RandomRotationDirection(EA::Reflection::Int32 &result) const {
    result = RandomRotationDirection();
    return true;
}
inline bool Gen::emitterdata::SET_RandomRotationDirection(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(RandomRotationDirection, input);
}

inline const UMath::Vector4 &Gen::emitterdata::RelativeAngle() const {
    ATTRIB_CODEGEN_GETLAYOUT(RelativeAngle);
}
inline bool Gen::emitterdata::RelativeAngle(UMath::Vector4 &result) const {
    result = RelativeAngle();
    return true;
}
inline bool Gen::emitterdata::SET_RelativeAngle(const UMath::Vector4 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(RelativeAngle, input);
}

inline const EA::Reflection::Float &Gen::emitterdata::RotationVariance() const {
    ATTRIB_CODEGEN_GETLAYOUT(RotationVariance);
}
inline bool Gen::emitterdata::RotationVariance(EA::Reflection::Float &result) const {
    result = RotationVariance();
    return true;
}
inline bool Gen::emitterdata::SET_RotationVariance(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(RotationVariance, input);
}

inline const UMath::Vector4 &Gen::emitterdata::Size() const {
    ATTRIB_CODEGEN_GETLAYOUT(Size);
}
inline bool Gen::emitterdata::Size(UMath::Vector4 &result) const {
    result = Size();
    return true;
}
inline bool Gen::emitterdata::SET_Size(const UMath::Vector4 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(Size, input);
}

inline const EA::Reflection::Float &Gen::emitterdata::Speed() const {
    ATTRIB_CODEGEN_GETLAYOUT(Speed);
}
inline bool Gen::emitterdata::Speed(EA::Reflection::Float &result) const {
    result = Speed();
    return true;
}
inline bool Gen::emitterdata::SET_Speed(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(Speed, input);
}

inline const EA::Reflection::Float &Gen::emitterdata::SpeedVariance() const {
    ATTRIB_CODEGEN_GETLAYOUT(SpeedVariance);
}
inline bool Gen::emitterdata::SpeedVariance(EA::Reflection::Float &result) const {
    result = SpeedVariance();
    return true;
}
inline bool Gen::emitterdata::SET_SpeedVariance(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(SpeedVariance, input);
}

inline const EA::Reflection::Float &Gen::emitterdata::SpreadAngle() const {
    ATTRIB_CODEGEN_GETLAYOUT(SpreadAngle);
}
inline bool Gen::emitterdata::SpreadAngle(EA::Reflection::Float &result) const {
    result = SpreadAngle();
    return true;
}
inline bool Gen::emitterdata::SET_SpreadAngle(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(SpreadAngle, input);
}

inline const EA::Reflection::Int32 &Gen::emitterdata::SpreadAsDisc() const {
    ATTRIB_CODEGEN_GETLAYOUT(SpreadAsDisc);
}
inline bool Gen::emitterdata::SpreadAsDisc(EA::Reflection::Int32 &result) const {
    result = SpreadAsDisc();
    return true;
}
inline bool Gen::emitterdata::SET_SpreadAsDisc(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(SpreadAsDisc, input);
}

inline const EA::Reflection::Float &Gen::emitterdata::StartDelay() const {
    ATTRIB_CODEGEN_GETLAYOUT(StartDelay);
}
inline bool Gen::emitterdata::StartDelay(EA::Reflection::Float &result) const {
    result = StartDelay();
    return true;
}
inline bool Gen::emitterdata::SET_StartDelay(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(StartDelay, input);
}

inline bool Gen::emitterdata::StartDelayRandomVariance(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x6e088259);
}
inline const EA::Reflection::Bool &Gen::emitterdata::StartDelayRandomVariance() const {
    ATTRIB_CODEGEN_GETLAYOUT(StartDelayRandomVariance);
}
inline bool Gen::emitterdata::StartDelayRandomVariance(EA::Reflection::Bool &result) const {
    result = StartDelayRandomVariance();
    return true;
}
inline bool Gen::emitterdata::SET_StartDelayRandomVariance(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETLAYOUT(StartDelayRandomVariance, input);
}

inline bool Gen::emitterdata::Texture(TAttrib<ParticleTextureRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ParticleTextureRecord, 0x6f3afee5);
}
inline const ParticleTextureRecord &Gen::emitterdata::Texture() const {
    ATTRIB_CODEGEN_GETLAYOUT(Texture);
}
inline bool Gen::emitterdata::Texture(ParticleTextureRecord &result) const {
    result = Texture();
    return true;
}
inline bool Gen::emitterdata::SET_Texture(const ParticleTextureRecord &input) {
    ATTRIB_CODEGEN_SETLAYOUT(Texture, input);
}

inline const ParticleAnimationInfo &Gen::emitterdata::TextureAnimation() const {
    ATTRIB_CODEGEN_GETLAYOUT(TextureAnimation);
}
inline bool Gen::emitterdata::TextureAnimation(ParticleAnimationInfo &result) const {
    result = TextureAnimation();
    return true;
}
inline bool Gen::emitterdata::SET_TextureAnimation(const ParticleAnimationInfo &input) {
    ATTRIB_CODEGEN_SETLAYOUT(TextureAnimation, input);
}

inline const UMath::Vector4 &Gen::emitterdata::VelocityDelta() const {
    ATTRIB_CODEGEN_GETLAYOUT(VelocityDelta);
}
inline bool Gen::emitterdata::VelocityDelta(UMath::Vector4 &result) const {
    result = VelocityDelta();
    return true;
}
inline bool Gen::emitterdata::SET_VelocityDelta(const UMath::Vector4 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(VelocityDelta, input);
}

inline const UMath::Vector4 &Gen::emitterdata::VelocityStart() const {
    ATTRIB_CODEGEN_GETLAYOUT(VelocityStart);
}
inline bool Gen::emitterdata::VelocityStart(UMath::Vector4 &result) const {
    result = VelocityStart();
    return true;
}
inline bool Gen::emitterdata::SET_VelocityStart(const UMath::Vector4 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(VelocityStart, input);
}

inline const UMath::Vector4 &Gen::emitterdata::VolumeCenter() const {
    ATTRIB_CODEGEN_GETLAYOUT(VolumeCenter);
}
inline bool Gen::emitterdata::VolumeCenter(UMath::Vector4 &result) const {
    result = VolumeCenter();
    return true;
}
inline bool Gen::emitterdata::SET_VolumeCenter(const UMath::Vector4 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(VolumeCenter, input);
}

inline const UMath::Vector4 &Gen::emitterdata::VolumeExtent() const {
    ATTRIB_CODEGEN_GETLAYOUT(VolumeExtent);
}
inline bool Gen::emitterdata::VolumeExtent(UMath::Vector4 &result) const {
    result = VolumeExtent();
    return true;
}
inline bool Gen::emitterdata::SET_VolumeExtent(const UMath::Vector4 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(VolumeExtent, input);
}

inline bool Gen::emitterdata::XenonEffect(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0xfe40e637);
}
inline const RefSpec &Gen::emitterdata::XenonEffect(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(RefSpec, 0xfe40e637, index);
}
inline bool Gen::emitterdata::XenonEffect(RefSpec &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(RefSpec, 0xfe40e637, result, index);
}
inline unsigned int Gen::emitterdata::Num_XenonEffect() const {
    ATTRIB_CODEGEN_GETLENGTH(0xfe40e637);
}
inline bool Gen::emitterdata::SET_XenonEffect(const RefSpec &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(RefSpec, 0xfe40e637, input, index);
}

}; // namespace Attrib

#endif
