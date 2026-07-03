#ifndef _attrib_gen_simsurface_h
#define _attrib_gen_simsurface_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct simsurface : Instance {
    struct _LayoutStruct {
        Private _Array_TireDriveEffects;             // offset 0x0, size 0x8
        TireEffectRecord TireDriveEffects[3];        // offset 0x8, size 0x3c
        Private _Array_TireSlideEffects;             // offset 0x44, size 0x8
        TireEffectRecord TireSlideEffects[3];        // offset 0x4c, size 0x3c
        Private _Array_TireSlipEffects;              // offset 0x88, size 0x8
        TireEffectRecord TireSlipEffects[3];         // offset 0x90, size 0x3c
        RoadNoiseRecord RenderNoise;                 // offset 0xcc, size 0x10
        EA::Reflection::Text CollectionName;         // offset 0xdc, size 0x4
        EA::Reflection::Float GROUND_FRICTION;       // offset 0xe0, size 0x4
        EA::Reflection::Float ROLLING_RESISTANCE;    // offset 0xe4, size 0x4
        EA::Reflection::Float WORLD_FRICTION;        // offset 0xe8, size 0x4
        EA::Reflection::Float DRIVE_GRIP;            // offset 0xec, size 0x4
        EA::Reflection::Float LATERAL_GRIP;          // offset 0xf0, size 0x4
        EA::Reflection::Float STICK;                 // offset 0xf4, size 0x4
        EA::Reflection::UInt16 WheelEffectFrequency; // offset 0xf8, size 0x2
        EA::Reflection::UInt8 WheelEffectIntensity;  // offset 0xfa, size 0x1
    };

    typedef FXROADNOISE_TRANSITION TypeOf_Aud_RoadNoise_TransOFF;
    typedef FXROADNOISE_TRANSITION TypeOf_Aud_RoadNoise_TransON;
    typedef FXROADNOISE_LOOP TypeOf_Aud_Roadnoise_LOOP;
    typedef EA::Reflection::Float TypeOf_CAMERA_NOISE;
    typedef EA::Reflection::Text TypeOf_CollectionName;
    typedef UMath::Vector4 TypeOf_DEBUG_COLOUR;
    typedef EA::Reflection::Float TypeOf_DRIVE_GRIP;
    typedef FFBWaveRecord TypeOf_FFB_ROLL;
    typedef FFBWaveRecord TypeOf_FFB_SKID;
    typedef FFBWaveRecord TypeOf_FFB_SLIP;
    typedef EA::Reflection::Float TypeOf_GROUND_FRICTION;
    typedef EA::Reflection::Float TypeOf_LATERAL_GRIP;
    typedef EA::Reflection::Float TypeOf_MATERIAL_STRENGTH;
    typedef EA::Reflection::Float TypeOf_ROLLING_RESISTANCE;
    typedef EA::Reflection::UInt32 TypeOf_RSNMCHUNK_FERESNAMES;
    typedef RoadNoiseRecord TypeOf_RenderNoise;
    typedef EA::Reflection::Float TypeOf_STICK;
    typedef TireEffectRecord TypeOf_TireDriveEffects;
    typedef TireEffectRecord TypeOf_TireSlideEffects;
    typedef TireEffectRecord TypeOf_TireSlipEffects;
    typedef EA::Reflection::Float TypeOf_WORLD_FRICTION;
    typedef EA::Reflection::UInt16 TypeOf_WheelEffectFrequency;
    typedef EA::Reflection::UInt8 TypeOf_WheelEffectIntensity;
    typedef SurfaceEffectType TypeOf_WheelSurfaceEffect;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("simsurface");
    simsurface(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    simsurface(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    simsurface(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    simsurface(const simsurface &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    simsurface(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~simsurface() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0xfb111fef;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0xfb111fef, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const simsurface &operator=(const simsurface &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const simsurface &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool Aud_RoadNoise_TransOFF(TAttrib<FXROADNOISE_TRANSITION> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(FXROADNOISE_TRANSITION, 0xcdf83544);
    }
    bool Aud_RoadNoise_TransOFF(FXROADNOISE_TRANSITION &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(FXROADNOISE_TRANSITION, 0xcdf83544, result);
    }
    const FXROADNOISE_TRANSITION &Aud_RoadNoise_TransOFF() const {
        ATTRIB_CODEGEN_GETVALUE(FXROADNOISE_TRANSITION, 0xcdf83544);
    }
    bool SET_Aud_RoadNoise_TransOFF(const FXROADNOISE_TRANSITION &input) {
        ATTRIB_CODEGEN_SETVALUE(FXROADNOISE_TRANSITION, 0xcdf83544, input);
    }
    bool Aud_RoadNoise_TransON(TAttrib<FXROADNOISE_TRANSITION> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(FXROADNOISE_TRANSITION, 0xb4c1b2cf);
    }
    bool Aud_RoadNoise_TransON(FXROADNOISE_TRANSITION &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(FXROADNOISE_TRANSITION, 0xb4c1b2cf, result);
    }
    const FXROADNOISE_TRANSITION &Aud_RoadNoise_TransON() const {
        ATTRIB_CODEGEN_GETVALUE(FXROADNOISE_TRANSITION, 0xb4c1b2cf);
    }
    bool SET_Aud_RoadNoise_TransON(const FXROADNOISE_TRANSITION &input) {
        ATTRIB_CODEGEN_SETVALUE(FXROADNOISE_TRANSITION, 0xb4c1b2cf, input);
    }
    bool Aud_Roadnoise_LOOP(TAttrib<FXROADNOISE_LOOP> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(FXROADNOISE_LOOP, 0x2907c135);
    }
    bool Aud_Roadnoise_LOOP(FXROADNOISE_LOOP &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(FXROADNOISE_LOOP, 0x2907c135, result);
    }
    const FXROADNOISE_LOOP &Aud_Roadnoise_LOOP() const {
        ATTRIB_CODEGEN_GETVALUE(FXROADNOISE_LOOP, 0x2907c135);
    }
    bool SET_Aud_Roadnoise_LOOP(const FXROADNOISE_LOOP &input) {
        ATTRIB_CODEGEN_SETVALUE(FXROADNOISE_LOOP, 0x2907c135, input);
    }
    bool CAMERA_NOISE(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xf0c9e498);
    }
    bool CAMERA_NOISE(EA::Reflection::Float &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(EA::Reflection::Float, 0xf0c9e498, result, index);
    }
    const EA::Reflection::Float &CAMERA_NOISE(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(EA::Reflection::Float, 0xf0c9e498, index);
    }
    unsigned int Num_CAMERA_NOISE() const {
        ATTRIB_CODEGEN_GETLENGTH(0xf0c9e498);
    }
    bool SET_CAMERA_NOISE(const EA::Reflection::Float &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(EA::Reflection::Float, 0xf0c9e498, input, index);
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
    bool DEBUG_COLOUR(TAttrib<UMath::Vector4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x740d3125);
    }
    bool DEBUG_COLOUR(UMath::Vector4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(UMath::Vector4, 0x740d3125, result);
    }
    const UMath::Vector4 &DEBUG_COLOUR() const {
        ATTRIB_CODEGEN_GETVALUE(UMath::Vector4, 0x740d3125);
    }
    bool SET_DEBUG_COLOUR(const UMath::Vector4 &input) {
        ATTRIB_CODEGEN_SETVALUE(UMath::Vector4, 0x740d3125, input);
    }
    bool DRIVE_GRIP(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xc68f17c2);
    }
    bool DRIVE_GRIP(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(DRIVE_GRIP, result);
    }
    const EA::Reflection::Float &DRIVE_GRIP() const {
        ATTRIB_CODEGEN_GETLAYOUT(DRIVE_GRIP);
    }
    bool SET_DRIVE_GRIP(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(DRIVE_GRIP, input);
    }
    bool FFB_ROLL(TAttrib<FFBWaveRecord> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(FFBWaveRecord, 0xba1297da);
    }
    bool FFB_ROLL(FFBWaveRecord &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(FFBWaveRecord, 0xba1297da, result, index);
    }
    const FFBWaveRecord &FFB_ROLL(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(FFBWaveRecord, 0xba1297da, index);
    }
    unsigned int Num_FFB_ROLL() const {
        ATTRIB_CODEGEN_GETLENGTH(0xba1297da);
    }
    bool SET_FFB_ROLL(const FFBWaveRecord &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(FFBWaveRecord, 0xba1297da, input, index);
    }
    bool FFB_SKID(TAttrib<FFBWaveRecord> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(FFBWaveRecord, 0x0c149044);
    }
    bool FFB_SKID(FFBWaveRecord &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(FFBWaveRecord, 0x0c149044, result, index);
    }
    const FFBWaveRecord &FFB_SKID(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(FFBWaveRecord, 0x0c149044, index);
    }
    unsigned int Num_FFB_SKID() const {
        ATTRIB_CODEGEN_GETLENGTH(0x0c149044);
    }
    bool SET_FFB_SKID(const FFBWaveRecord &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(FFBWaveRecord, 0x0c149044, input, index);
    }
    bool FFB_SLIP(TAttrib<FFBWaveRecord> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(FFBWaveRecord, 0x8fd11d27);
    }
    bool FFB_SLIP(FFBWaveRecord &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(FFBWaveRecord, 0x8fd11d27, result, index);
    }
    const FFBWaveRecord &FFB_SLIP(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(FFBWaveRecord, 0x8fd11d27, index);
    }
    unsigned int Num_FFB_SLIP() const {
        ATTRIB_CODEGEN_GETLENGTH(0x8fd11d27);
    }
    bool SET_FFB_SLIP(const FFBWaveRecord &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(FFBWaveRecord, 0x8fd11d27, input, index);
    }
    bool GROUND_FRICTION(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x3fb1b342);
    }
    bool GROUND_FRICTION(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(GROUND_FRICTION, result);
    }
    const EA::Reflection::Float &GROUND_FRICTION() const {
        ATTRIB_CODEGEN_GETLAYOUT(GROUND_FRICTION);
    }
    bool SET_GROUND_FRICTION(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(GROUND_FRICTION, input);
    }
    bool LATERAL_GRIP(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x5ce7dba8);
    }
    bool LATERAL_GRIP(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(LATERAL_GRIP, result);
    }
    const EA::Reflection::Float &LATERAL_GRIP() const {
        ATTRIB_CODEGEN_GETLAYOUT(LATERAL_GRIP);
    }
    bool SET_LATERAL_GRIP(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(LATERAL_GRIP, input);
    }
    bool MATERIAL_STRENGTH(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x9fca0b40);
    }
    bool MATERIAL_STRENGTH(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x9fca0b40, result);
    }
    const EA::Reflection::Float &MATERIAL_STRENGTH() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x9fca0b40);
    }
    bool SET_MATERIAL_STRENGTH(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x9fca0b40, input);
    }
    bool ROLLING_RESISTANCE(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x9f2b0192);
    }
    bool ROLLING_RESISTANCE(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(ROLLING_RESISTANCE, result);
    }
    const EA::Reflection::Float &ROLLING_RESISTANCE() const {
        ATTRIB_CODEGEN_GETLAYOUT(ROLLING_RESISTANCE);
    }
    bool SET_ROLLING_RESISTANCE(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(ROLLING_RESISTANCE, input);
    }
    bool RSNMCHUNK_FERESNAMES(TAttrib<EA::Reflection::UInt32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0xd377b339);
    }
    bool RSNMCHUNK_FERESNAMES(EA::Reflection::UInt32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::UInt32, 0xd377b339, result);
    }
    const EA::Reflection::UInt32 &RSNMCHUNK_FERESNAMES() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::UInt32, 0xd377b339);
    }
    bool SET_RSNMCHUNK_FERESNAMES(const EA::Reflection::UInt32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::UInt32, 0xd377b339, input);
    }
    bool RenderNoise(TAttrib<RoadNoiseRecord> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RoadNoiseRecord, 0x940b79c4);
    }
    bool RenderNoise(RoadNoiseRecord &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(RenderNoise, result);
    }
    const RoadNoiseRecord &RenderNoise() const {
        ATTRIB_CODEGEN_GETLAYOUT(RenderNoise);
    }
    bool SET_RenderNoise(const RoadNoiseRecord &input) {
        ATTRIB_CODEGEN_SETLAYOUT(RenderNoise, input);
    }
    bool STICK(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xd72119b4);
    }
    bool STICK(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(STICK, result);
    }
    const EA::Reflection::Float &STICK() const {
        ATTRIB_CODEGEN_GETLAYOUT(STICK);
    }
    bool SET_STICK(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(STICK, input);
    }
    bool TireDriveEffects(TAttrib<TireEffectRecord> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(TireEffectRecord, 0xfcc3efa6);
    }
    bool TireDriveEffects(TireEffectRecord &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(TireDriveEffects, result, index);
    }
    const TireEffectRecord &TireDriveEffects(unsigned int index) const {
        ATTRIB_CODEGEN_GETLAYOUTINDEXED(TireEffectRecord, TireDriveEffects, index);
    }
    unsigned int Num_TireDriveEffects() const {
        ATTRIB_CODEGEN_GETLAYOUTLENGTH(TireDriveEffects);
    }
    bool SET_TireDriveEffects(const TireEffectRecord &input, unsigned int index) {
        ATTRIB_CODEGEN_SETLAYOUTINDEXED(TireDriveEffects, input, index);
    }
    bool TireSlideEffects(TAttrib<TireEffectRecord> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(TireEffectRecord, 0x27b6871c);
    }
    bool TireSlideEffects(TireEffectRecord &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(TireSlideEffects, result, index);
    }
    const TireEffectRecord &TireSlideEffects(unsigned int index) const {
        ATTRIB_CODEGEN_GETLAYOUTINDEXED(TireEffectRecord, TireSlideEffects, index);
    }
    unsigned int Num_TireSlideEffects() const {
        ATTRIB_CODEGEN_GETLAYOUTLENGTH(TireSlideEffects);
    }
    bool SET_TireSlideEffects(const TireEffectRecord &input, unsigned int index) {
        ATTRIB_CODEGEN_SETLAYOUTINDEXED(TireSlideEffects, input, index);
    }
    bool TireSlipEffects(TAttrib<TireEffectRecord> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(TireEffectRecord, 0x68411a69);
    }
    bool TireSlipEffects(TireEffectRecord &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(TireSlipEffects, result, index);
    }
    const TireEffectRecord &TireSlipEffects(unsigned int index) const {
        ATTRIB_CODEGEN_GETLAYOUTINDEXED(TireEffectRecord, TireSlipEffects, index);
    }
    unsigned int Num_TireSlipEffects() const {
        ATTRIB_CODEGEN_GETLAYOUTLENGTH(TireSlipEffects);
    }
    bool SET_TireSlipEffects(const TireEffectRecord &input, unsigned int index) {
        ATTRIB_CODEGEN_SETLAYOUTINDEXED(TireSlipEffects, input, index);
    }
    bool WORLD_FRICTION(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x875165fe);
    }
    bool WORLD_FRICTION(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(WORLD_FRICTION, result);
    }
    const EA::Reflection::Float &WORLD_FRICTION() const {
        ATTRIB_CODEGEN_GETLAYOUT(WORLD_FRICTION);
    }
    bool SET_WORLD_FRICTION(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(WORLD_FRICTION, input);
    }
    bool WheelEffectFrequency(TAttrib<EA::Reflection::UInt16> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt16, 0x968c3422);
    }
    bool WheelEffectFrequency(EA::Reflection::UInt16 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(WheelEffectFrequency, result);
    }
    const EA::Reflection::UInt16 &WheelEffectFrequency() const {
        ATTRIB_CODEGEN_GETLAYOUT(WheelEffectFrequency);
    }
    bool SET_WheelEffectFrequency(const EA::Reflection::UInt16 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(WheelEffectFrequency, input);
    }
    bool WheelEffectIntensity(TAttrib<EA::Reflection::UInt8> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt8, 0xb7e0af1c);
    }
    bool WheelEffectIntensity(EA::Reflection::UInt8 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(WheelEffectIntensity, result);
    }
    const EA::Reflection::UInt8 &WheelEffectIntensity() const {
        ATTRIB_CODEGEN_GETLAYOUT(WheelEffectIntensity);
    }
    bool SET_WheelEffectIntensity(const EA::Reflection::UInt8 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(WheelEffectIntensity, input);
    }
    bool WheelSurfaceEffect(TAttrib<SurfaceEffectType> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(SurfaceEffectType, 0x46226745);
    }
    bool WheelSurfaceEffect(SurfaceEffectType &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(SurfaceEffectType, 0x46226745, result);
    }
    const SurfaceEffectType &WheelSurfaceEffect() const {
        ATTRIB_CODEGEN_GETVALUE(SurfaceEffectType, 0x46226745);
    }
    bool SET_WheelSurfaceEffect(const SurfaceEffectType &input) {
        ATTRIB_CODEGEN_SETVALUE(SurfaceEffectType, 0x46226745, input);
    }

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    simsurface &ConvertFromInstance(Instance &src) {}
    const simsurface &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key simsurface = 0xfb111fef;

}; // namespace ClassName

namespace Hash {
namespace simsurface {

static const Key Aud_RoadNoise_TransOFF = 0xcdf83544;
static const Key Aud_RoadNoise_TransON = 0xb4c1b2cf;
static const Key Aud_Roadnoise_LOOP = 0x2907c135;
static const Key CAMERA_NOISE = 0xf0c9e498;
static const Key CollectionName = 0x9ca1c8f9;
static const Key DEBUG_COLOUR = 0x740d3125;
static const Key DRIVE_GRIP = 0xc68f17c2;
static const Key FFB_ROLL = 0xba1297da;
static const Key FFB_SKID = 0x0c149044;
static const Key FFB_SLIP = 0x8fd11d27;
static const Key GROUND_FRICTION = 0x3fb1b342;
static const Key LATERAL_GRIP = 0x5ce7dba8;
static const Key MATERIAL_STRENGTH = 0x9fca0b40;
static const Key ROLLING_RESISTANCE = 0x9f2b0192;
static const Key RSNMCHUNK_FERESNAMES = 0xd377b339;
static const Key RenderNoise = 0x940b79c4;
static const Key STICK = 0xd72119b4;
static const Key TireDriveEffects = 0xfcc3efa6;
static const Key TireSlideEffects = 0x27b6871c;
static const Key TireSlipEffects = 0x68411a69;
static const Key WORLD_FRICTION = 0x875165fe;
static const Key WheelEffectFrequency = 0x968c3422;
static const Key WheelEffectIntensity = 0xb7e0af1c;
static const Key WheelSurfaceEffect = 0x46226745;

}; // namespace simsurface
}; // namespace Hash

inline Key Gen::simsurface::ClassKey() {
    return ClassName::simsurface;
}

}; // namespace Attrib

#endif
