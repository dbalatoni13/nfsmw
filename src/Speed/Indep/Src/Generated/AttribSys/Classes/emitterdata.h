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
    bool AccelDelta(TAttrib<UMath::Vector4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x81167023);
    }
    bool AccelDelta(UMath::Vector4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(AccelDelta, result);
    }
    const UMath::Vector4 &AccelDelta() const {
        ATTRIB_CODEGEN_GETLAYOUT(AccelDelta);
    }
    bool SET_AccelDelta(const UMath::Vector4 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(AccelDelta, input);
    }
    bool AccelStart(TAttrib<UMath::Vector4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x81222c15);
    }
    bool AccelStart(UMath::Vector4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(AccelStart, result);
    }
    const UMath::Vector4 &AccelStart() const {
        ATTRIB_CODEGEN_GETLAYOUT(AccelStart);
    }
    bool SET_AccelStart(const UMath::Vector4 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(AccelStart, input);
    }
    bool AlphaToKillAt(TAttrib<EA::Reflection::Int8> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int8, 0x9ec0561b);
    }
    bool AlphaToKillAt(EA::Reflection::Int8 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(AlphaToKillAt, result);
    }
    const EA::Reflection::Int8 &AlphaToKillAt() const {
        ATTRIB_CODEGEN_GETLAYOUT(AlphaToKillAt);
    }
    bool SET_AlphaToKillAt(const EA::Reflection::Int8 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(AlphaToKillAt, input);
    }
    bool AxisConstraint(TAttrib<EffectParticleConstraint> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EffectParticleConstraint, 0x78d7facd);
    }
    bool AxisConstraint(EffectParticleConstraint &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(AxisConstraint, result);
    }
    const EffectParticleConstraint &AxisConstraint() const {
        ATTRIB_CODEGEN_GETLAYOUT(AxisConstraint);
    }
    bool SET_AxisConstraint(const EffectParticleConstraint &input) {
        ATTRIB_CODEGEN_SETLAYOUT(AxisConstraint, input);
    }
    bool CollectionName(TAttrib<EA::Reflection::Text> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x9ca1c8f9);
    }
    bool CollectionName(EA::Reflection::Text &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(CollectionName, result);
    }
    const EA::Reflection::Text &CollectionName() const {
        ATTRIB_CODEGEN_GETLAYOUT(CollectionName);
    }
    bool SET_CollectionName(const EA::Reflection::Text &input) {
        ATTRIB_CODEGEN_SETLAYOUT(CollectionName, input);
    }
    bool Color1(TAttrib<EA::Reflection::UInt32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0xccf41b18);
    }
    bool Color1(EA::Reflection::UInt32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(Color1, result);
    }
    const EA::Reflection::UInt32 &Color1() const {
        ATTRIB_CODEGEN_GETLAYOUT(Color1);
    }
    bool SET_Color1(const EA::Reflection::UInt32 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(Color1, input);
    }
    bool Color2(TAttrib<EA::Reflection::UInt32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0x4a282af1);
    }
    bool Color2(EA::Reflection::UInt32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(Color2, result);
    }
    const EA::Reflection::UInt32 &Color2() const {
        ATTRIB_CODEGEN_GETLAYOUT(Color2);
    }
    bool SET_Color2(const EA::Reflection::UInt32 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(Color2, input);
    }
    bool Color3(TAttrib<EA::Reflection::UInt32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0xc103b771);
    }
    bool Color3(EA::Reflection::UInt32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(Color3, result);
    }
    const EA::Reflection::UInt32 &Color3() const {
        ATTRIB_CODEGEN_GETLAYOUT(Color3);
    }
    bool SET_Color3(const EA::Reflection::UInt32 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(Color3, input);
    }
    bool Color4(TAttrib<EA::Reflection::UInt32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0x78f32c40);
    }
    bool Color4(EA::Reflection::UInt32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(Color4, result);
    }
    const EA::Reflection::UInt32 &Color4() const {
        ATTRIB_CODEGEN_GETLAYOUT(Color4);
    }
    bool SET_Color4(const EA::Reflection::UInt32 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(Color4, input);
    }
    bool Drag(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x4e58fa4e);
    }
    bool Drag(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(Drag, result);
    }
    const EA::Reflection::Float &Drag() const {
        ATTRIB_CODEGEN_GETLAYOUT(Drag);
    }
    bool SET_Drag(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(Drag, input);
    }
    bool EliminateUnnecessaryRandomness(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x859043b2);
    }
    bool EliminateUnnecessaryRandomness(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(EliminateUnnecessaryRandomness, result);
    }
    const EA::Reflection::Bool &EliminateUnnecessaryRandomness() const {
        ATTRIB_CODEGEN_GETLAYOUT(EliminateUnnecessaryRandomness);
    }
    bool SET_EliminateUnnecessaryRandomness(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETLAYOUT(EliminateUnnecessaryRandomness, input);
    }
    bool FarClip(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x66052349);
    }
    bool FarClip(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(FarClip, result);
    }
    const EA::Reflection::Float &FarClip() const {
        ATTRIB_CODEGEN_GETLAYOUT(FarClip);
    }
    bool SET_FarClip(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(FarClip, input);
    }
    bool Gravity(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xac5b265e);
    }
    bool Gravity(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(Gravity, result);
    }
    const EA::Reflection::Float &Gravity() const {
        ATTRIB_CODEGEN_GETLAYOUT(Gravity);
    }
    bool SET_Gravity(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(Gravity, input);
    }
    bool InitialAngleRange(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x284a8c2c);
    }
    bool InitialAngleRange(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(InitialAngleRange, result);
    }
    const EA::Reflection::Float &InitialAngleRange() const {
        ATTRIB_CODEGEN_GETLAYOUT(InitialAngleRange);
    }
    bool SET_InitialAngleRange(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(InitialAngleRange, input);
    }
    bool IsOneShot(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xb2659b2a);
    }
    bool IsOneShot(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(IsOneShot, result);
    }
    const EA::Reflection::Bool &IsOneShot() const {
        ATTRIB_CODEGEN_GETLAYOUT(IsOneShot);
    }
    bool SET_IsOneShot(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETLAYOUT(IsOneShot, input);
    }
    bool KeyPositions(TAttrib<UMath::Vector4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x62fe3594);
    }
    bool KeyPositions(UMath::Vector4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(KeyPositions, result);
    }
    const UMath::Vector4 &KeyPositions() const {
        ATTRIB_CODEGEN_GETLAYOUT(KeyPositions);
    }
    bool SET_KeyPositions(const UMath::Vector4 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(KeyPositions, input);
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
    bool MotionInherit(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x6bcbfc06);
    }
    bool MotionInherit(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(MotionInherit, result);
    }
    const EA::Reflection::Float &MotionInherit() const {
        ATTRIB_CODEGEN_GETLAYOUT(MotionInherit);
    }
    bool SET_MotionInherit(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(MotionInherit, input);
    }
    bool MotionInheritVariance(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xd34a0b9d);
    }
    bool MotionInheritVariance(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(MotionInheritVariance, result);
    }
    const EA::Reflection::Float &MotionInheritVariance() const {
        ATTRIB_CODEGEN_GETLAYOUT(MotionInheritVariance);
    }
    bool SET_MotionInheritVariance(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(MotionInheritVariance, input);
    }
    bool MotionLive(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x4d69ef9e);
    }
    bool MotionLive(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(MotionLive, result);
    }
    const EA::Reflection::Int32 &MotionLive() const {
        ATTRIB_CODEGEN_GETLAYOUT(MotionLive);
    }
    bool SET_MotionLive(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(MotionLive, input);
    }
    bool NoKillAtAlpha(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x27a9a451);
    }
    bool NoKillAtAlpha(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(NoKillAtAlpha, result);
    }
    const EA::Reflection::Bool &NoKillAtAlpha() const {
        ATTRIB_CODEGEN_GETLAYOUT(NoKillAtAlpha);
    }
    bool SET_NoKillAtAlpha(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETLAYOUT(NoKillAtAlpha, input);
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
    bool OffCycle(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xb619ecb2);
    }
    bool OffCycle(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(OffCycle, result);
    }
    const EA::Reflection::Float &OffCycle() const {
        ATTRIB_CODEGEN_GETLAYOUT(OffCycle);
    }
    bool SET_OffCycle(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(OffCycle, input);
    }
    bool OffCycleVariance(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x8510a6ef);
    }
    bool OffCycleVariance(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(OffCycleVariance, result);
    }
    const EA::Reflection::Float &OffCycleVariance() const {
        ATTRIB_CODEGEN_GETLAYOUT(OffCycleVariance);
    }
    bool SET_OffCycleVariance(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(OffCycleVariance, input);
    }
    bool OnCycle(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xeb86a538);
    }
    bool OnCycle(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(OnCycle, result);
    }
    const EA::Reflection::Float &OnCycle() const {
        ATTRIB_CODEGEN_GETLAYOUT(OnCycle);
    }
    bool SET_OnCycle(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(OnCycle, input);
    }
    bool OnCycleVariance(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x3b53b68c);
    }
    bool OnCycleVariance(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(OnCycleVariance, result);
    }
    const EA::Reflection::Float &OnCycleVariance() const {
        ATTRIB_CODEGEN_GETLAYOUT(OnCycleVariance);
    }
    bool SET_OnCycleVariance(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(OnCycleVariance, input);
    }
    bool RandomRotationDirection(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x0be45b47);
    }
    bool RandomRotationDirection(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(RandomRotationDirection, result);
    }
    const EA::Reflection::Int32 &RandomRotationDirection() const {
        ATTRIB_CODEGEN_GETLAYOUT(RandomRotationDirection);
    }
    bool SET_RandomRotationDirection(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(RandomRotationDirection, input);
    }
    bool RelativeAngle(TAttrib<UMath::Vector4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0xcd0d867f);
    }
    bool RelativeAngle(UMath::Vector4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(RelativeAngle, result);
    }
    const UMath::Vector4 &RelativeAngle() const {
        ATTRIB_CODEGEN_GETLAYOUT(RelativeAngle);
    }
    bool SET_RelativeAngle(const UMath::Vector4 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(RelativeAngle, input);
    }
    bool RotationVariance(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x31af20d6);
    }
    bool RotationVariance(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(RotationVariance, result);
    }
    const EA::Reflection::Float &RotationVariance() const {
        ATTRIB_CODEGEN_GETLAYOUT(RotationVariance);
    }
    bool SET_RotationVariance(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(RotationVariance, input);
    }
    bool Size(TAttrib<UMath::Vector4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0xc10c3e15);
    }
    bool Size(UMath::Vector4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(Size, result);
    }
    const UMath::Vector4 &Size() const {
        ATTRIB_CODEGEN_GETLAYOUT(Size);
    }
    bool SET_Size(const UMath::Vector4 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(Size, input);
    }
    bool Speed(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x41862fe6);
    }
    bool Speed(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(Speed, result);
    }
    const EA::Reflection::Float &Speed() const {
        ATTRIB_CODEGEN_GETLAYOUT(Speed);
    }
    bool SET_Speed(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(Speed, input);
    }
    bool SpeedVariance(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x58fcb1c3);
    }
    bool SpeedVariance(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(SpeedVariance, result);
    }
    const EA::Reflection::Float &SpeedVariance() const {
        ATTRIB_CODEGEN_GETLAYOUT(SpeedVariance);
    }
    bool SET_SpeedVariance(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(SpeedVariance, input);
    }
    bool SpreadAngle(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xcac30ff2);
    }
    bool SpreadAngle(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(SpreadAngle, result);
    }
    const EA::Reflection::Float &SpreadAngle() const {
        ATTRIB_CODEGEN_GETLAYOUT(SpreadAngle);
    }
    bool SET_SpreadAngle(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(SpreadAngle, input);
    }
    bool SpreadAsDisc(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x0836f5cc);
    }
    bool SpreadAsDisc(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(SpreadAsDisc, result);
    }
    const EA::Reflection::Int32 &SpreadAsDisc() const {
        ATTRIB_CODEGEN_GETLAYOUT(SpreadAsDisc);
    }
    bool SET_SpreadAsDisc(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(SpreadAsDisc, input);
    }
    bool StartDelay(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xee67ad35);
    }
    bool StartDelay(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(StartDelay, result);
    }
    const EA::Reflection::Float &StartDelay() const {
        ATTRIB_CODEGEN_GETLAYOUT(StartDelay);
    }
    bool SET_StartDelay(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(StartDelay, input);
    }
    bool StartDelayRandomVariance(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x6e088259);
    }
    bool StartDelayRandomVariance(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(StartDelayRandomVariance, result);
    }
    const EA::Reflection::Bool &StartDelayRandomVariance() const {
        ATTRIB_CODEGEN_GETLAYOUT(StartDelayRandomVariance);
    }
    bool SET_StartDelayRandomVariance(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETLAYOUT(StartDelayRandomVariance, input);
    }
    bool Texture(TAttrib<ParticleTextureRecord> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(ParticleTextureRecord, 0x6f3afee5);
    }
    bool Texture(ParticleTextureRecord &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(Texture, result);
    }
    const ParticleTextureRecord &Texture() const {
        ATTRIB_CODEGEN_GETLAYOUT(Texture);
    }
    bool SET_Texture(const ParticleTextureRecord &input) {
        ATTRIB_CODEGEN_SETLAYOUT(Texture, input);
    }
    bool TextureAnimation(TAttrib<ParticleAnimationInfo> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(ParticleAnimationInfo, 0x92709587);
    }
    bool TextureAnimation(ParticleAnimationInfo &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(TextureAnimation, result);
    }
    const ParticleAnimationInfo &TextureAnimation() const {
        ATTRIB_CODEGEN_GETLAYOUT(TextureAnimation);
    }
    bool SET_TextureAnimation(const ParticleAnimationInfo &input) {
        ATTRIB_CODEGEN_SETLAYOUT(TextureAnimation, input);
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
    bool XenonEffect(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0xfe40e637);
    }
    bool XenonEffect(RefSpec &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(RefSpec, 0xfe40e637, result, index);
    }
    const RefSpec &XenonEffect(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(RefSpec, 0xfe40e637, index);
    }
    unsigned int Num_XenonEffect() const {
        ATTRIB_CODEGEN_GETLENGTH(0xfe40e637);
    }
    bool SET_XenonEffect(const RefSpec &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(RefSpec, 0xfe40e637, input, index);
    }

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

}; // namespace Attrib

#endif
