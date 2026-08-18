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
    typedef EA::Reflection::UInt32 TypeOf_Aud_Skid_Type;
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
    bool Aud_RoadNoise_TransOFF(TAttrib<FXROADNOISE_TRANSITION> &result) const;
    bool Aud_RoadNoise_TransOFF(FXROADNOISE_TRANSITION &result) const;
    const FXROADNOISE_TRANSITION &Aud_RoadNoise_TransOFF() const;
    bool SET_Aud_RoadNoise_TransOFF(const FXROADNOISE_TRANSITION &input);

    bool Aud_RoadNoise_TransON(TAttrib<FXROADNOISE_TRANSITION> &result) const;
    bool Aud_RoadNoise_TransON(FXROADNOISE_TRANSITION &result) const;
    const FXROADNOISE_TRANSITION &Aud_RoadNoise_TransON() const;
    bool SET_Aud_RoadNoise_TransON(const FXROADNOISE_TRANSITION &input);

    bool Aud_Roadnoise_LOOP(TAttrib<FXROADNOISE_LOOP> &result) const;
    bool Aud_Roadnoise_LOOP(FXROADNOISE_LOOP &result) const;
    const FXROADNOISE_LOOP &Aud_Roadnoise_LOOP() const;
    bool SET_Aud_Roadnoise_LOOP(const FXROADNOISE_LOOP &input);

    bool Aud_Skid_Type(TAttrib<EA::Reflection::UInt32> &result) const;
    bool Aud_Skid_Type(EA::Reflection::UInt32 &result) const;
    const EA::Reflection::UInt32 &Aud_Skid_Type() const;
    bool SET_Aud_Skid_Type(const EA::Reflection::UInt32 &input);

    bool CAMERA_NOISE(TAttrib<EA::Reflection::Float> &result) const;
    bool CAMERA_NOISE(EA::Reflection::Float &result, unsigned int index) const;
    const EA::Reflection::Float &CAMERA_NOISE(unsigned int index) const;
    unsigned int Num_CAMERA_NOISE() const;
    bool SET_CAMERA_NOISE(const EA::Reflection::Float &input, unsigned int index);

    bool CollectionName(TAttrib<EA::Reflection::Text> &result) const;
    bool CollectionName(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &CollectionName() const;
    bool SET_CollectionName(const EA::Reflection::Text &input);

    bool DEBUG_COLOUR(TAttrib<UMath::Vector4> &result) const;
    bool DEBUG_COLOUR(UMath::Vector4 &result) const;
    const UMath::Vector4 &DEBUG_COLOUR() const;
    bool SET_DEBUG_COLOUR(const UMath::Vector4 &input);

    bool DRIVE_GRIP(TAttrib<EA::Reflection::Float> &result) const;
    bool DRIVE_GRIP(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &DRIVE_GRIP() const;
    bool SET_DRIVE_GRIP(const EA::Reflection::Float &input);

    bool FFB_ROLL(TAttrib<FFBWaveRecord> &result) const;
    bool FFB_ROLL(FFBWaveRecord &result, unsigned int index) const;
    const FFBWaveRecord &FFB_ROLL(unsigned int index) const;
    unsigned int Num_FFB_ROLL() const;
    bool SET_FFB_ROLL(const FFBWaveRecord &input, unsigned int index);

    bool FFB_SKID(TAttrib<FFBWaveRecord> &result) const;
    bool FFB_SKID(FFBWaveRecord &result, unsigned int index) const;
    const FFBWaveRecord &FFB_SKID(unsigned int index) const;
    unsigned int Num_FFB_SKID() const;
    bool SET_FFB_SKID(const FFBWaveRecord &input, unsigned int index);

    bool FFB_SLIP(TAttrib<FFBWaveRecord> &result) const;
    bool FFB_SLIP(FFBWaveRecord &result, unsigned int index) const;
    const FFBWaveRecord &FFB_SLIP(unsigned int index) const;
    unsigned int Num_FFB_SLIP() const;
    bool SET_FFB_SLIP(const FFBWaveRecord &input, unsigned int index);

    bool GROUND_FRICTION(TAttrib<EA::Reflection::Float> &result) const;
    bool GROUND_FRICTION(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &GROUND_FRICTION() const;
    bool SET_GROUND_FRICTION(const EA::Reflection::Float &input);

    bool LATERAL_GRIP(TAttrib<EA::Reflection::Float> &result) const;
    bool LATERAL_GRIP(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &LATERAL_GRIP() const;
    bool SET_LATERAL_GRIP(const EA::Reflection::Float &input);

    bool MATERIAL_STRENGTH(TAttrib<EA::Reflection::Float> &result) const;
    bool MATERIAL_STRENGTH(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &MATERIAL_STRENGTH() const;
    bool SET_MATERIAL_STRENGTH(const EA::Reflection::Float &input);

    bool ROLLING_RESISTANCE(TAttrib<EA::Reflection::Float> &result) const;
    bool ROLLING_RESISTANCE(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &ROLLING_RESISTANCE() const;
    bool SET_ROLLING_RESISTANCE(const EA::Reflection::Float &input);

    bool RenderNoise(TAttrib<RoadNoiseRecord> &result) const;
    bool RenderNoise(RoadNoiseRecord &result) const;
    const RoadNoiseRecord &RenderNoise() const;
    bool SET_RenderNoise(const RoadNoiseRecord &input);

    bool STICK(TAttrib<EA::Reflection::Float> &result) const;
    bool STICK(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &STICK() const;
    bool SET_STICK(const EA::Reflection::Float &input);

    bool TireDriveEffects(TAttrib<TireEffectRecord> &result) const;
    bool TireDriveEffects(TireEffectRecord &result, unsigned int index) const;
    const TireEffectRecord &TireDriveEffects(unsigned int index) const;
    unsigned int Num_TireDriveEffects() const;
    bool SET_TireDriveEffects(const TireEffectRecord &input, unsigned int index);

    bool TireSlideEffects(TAttrib<TireEffectRecord> &result) const;
    bool TireSlideEffects(TireEffectRecord &result, unsigned int index) const;
    const TireEffectRecord &TireSlideEffects(unsigned int index) const;
    unsigned int Num_TireSlideEffects() const;
    bool SET_TireSlideEffects(const TireEffectRecord &input, unsigned int index);

    bool TireSlipEffects(TAttrib<TireEffectRecord> &result) const;
    bool TireSlipEffects(TireEffectRecord &result, unsigned int index) const;
    const TireEffectRecord &TireSlipEffects(unsigned int index) const;
    unsigned int Num_TireSlipEffects() const;
    bool SET_TireSlipEffects(const TireEffectRecord &input, unsigned int index);

    bool WORLD_FRICTION(TAttrib<EA::Reflection::Float> &result) const;
    bool WORLD_FRICTION(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &WORLD_FRICTION() const;
    bool SET_WORLD_FRICTION(const EA::Reflection::Float &input);

    bool WheelEffectFrequency(TAttrib<EA::Reflection::UInt16> &result) const;
    bool WheelEffectFrequency(EA::Reflection::UInt16 &result) const;
    const EA::Reflection::UInt16 &WheelEffectFrequency() const;
    bool SET_WheelEffectFrequency(const EA::Reflection::UInt16 &input);

    bool WheelEffectIntensity(TAttrib<EA::Reflection::UInt8> &result) const;
    bool WheelEffectIntensity(EA::Reflection::UInt8 &result) const;
    const EA::Reflection::UInt8 &WheelEffectIntensity() const;
    bool SET_WheelEffectIntensity(const EA::Reflection::UInt8 &input);

    bool WheelSurfaceEffect(TAttrib<SurfaceEffectType> &result) const;
    bool WheelSurfaceEffect(SurfaceEffectType &result) const;
    const SurfaceEffectType &WheelSurfaceEffect() const;
    bool SET_WheelSurfaceEffect(const SurfaceEffectType &input);

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
static const Key Aud_Skid_Type = 0xd377b339;
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

inline bool Gen::simsurface::Aud_RoadNoise_TransOFF(TAttrib<FXROADNOISE_TRANSITION> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(FXROADNOISE_TRANSITION, 0xcdf83544);
}
inline const FXROADNOISE_TRANSITION &Gen::simsurface::Aud_RoadNoise_TransOFF() const {
    ATTRIB_CODEGEN_GETVALUE(FXROADNOISE_TRANSITION, 0xcdf83544);
}
inline bool Gen::simsurface::Aud_RoadNoise_TransOFF(FXROADNOISE_TRANSITION &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(FXROADNOISE_TRANSITION, 0xcdf83544, result);
}
inline bool Gen::simsurface::SET_Aud_RoadNoise_TransOFF(const FXROADNOISE_TRANSITION &input) {
    ATTRIB_CODEGEN_SETVALUE(FXROADNOISE_TRANSITION, 0xcdf83544, input);
}

inline bool Gen::simsurface::Aud_RoadNoise_TransON(TAttrib<FXROADNOISE_TRANSITION> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(FXROADNOISE_TRANSITION, 0xb4c1b2cf);
}
inline const FXROADNOISE_TRANSITION &Gen::simsurface::Aud_RoadNoise_TransON() const {
    ATTRIB_CODEGEN_GETVALUE(FXROADNOISE_TRANSITION, 0xb4c1b2cf);
}
inline bool Gen::simsurface::Aud_RoadNoise_TransON(FXROADNOISE_TRANSITION &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(FXROADNOISE_TRANSITION, 0xb4c1b2cf, result);
}
inline bool Gen::simsurface::SET_Aud_RoadNoise_TransON(const FXROADNOISE_TRANSITION &input) {
    ATTRIB_CODEGEN_SETVALUE(FXROADNOISE_TRANSITION, 0xb4c1b2cf, input);
}

inline bool Gen::simsurface::Aud_Roadnoise_LOOP(TAttrib<FXROADNOISE_LOOP> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(FXROADNOISE_LOOP, 0x2907c135);
}
inline const FXROADNOISE_LOOP &Gen::simsurface::Aud_Roadnoise_LOOP() const {
    ATTRIB_CODEGEN_GETVALUE(FXROADNOISE_LOOP, 0x2907c135);
}
inline bool Gen::simsurface::Aud_Roadnoise_LOOP(FXROADNOISE_LOOP &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(FXROADNOISE_LOOP, 0x2907c135, result);
}
inline bool Gen::simsurface::SET_Aud_Roadnoise_LOOP(const FXROADNOISE_LOOP &input) {
    ATTRIB_CODEGEN_SETVALUE(FXROADNOISE_LOOP, 0x2907c135, input);
}

inline bool Gen::simsurface::Aud_Skid_Type(TAttrib<EA::Reflection::UInt32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0xd377b339);
}
inline const EA::Reflection::UInt32 &Gen::simsurface::Aud_Skid_Type() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::UInt32, 0xd377b339);
}
inline bool Gen::simsurface::Aud_Skid_Type(EA::Reflection::UInt32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::UInt32, 0xd377b339, result);
}
inline bool Gen::simsurface::SET_Aud_Skid_Type(const EA::Reflection::UInt32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::UInt32, 0xd377b339, input);
}

inline bool Gen::simsurface::CAMERA_NOISE(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xf0c9e498);
}
inline const EA::Reflection::Float &Gen::simsurface::CAMERA_NOISE(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(EA::Reflection::Float, 0xf0c9e498, index);
}
inline bool Gen::simsurface::CAMERA_NOISE(EA::Reflection::Float &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(EA::Reflection::Float, 0xf0c9e498, result, index);
}
inline unsigned int Gen::simsurface::Num_CAMERA_NOISE() const {
    ATTRIB_CODEGEN_GETLENGTH(0xf0c9e498);
}
inline bool Gen::simsurface::SET_CAMERA_NOISE(const EA::Reflection::Float &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(EA::Reflection::Float, 0xf0c9e498, input, index);
}

inline bool Gen::simsurface::CollectionName(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x9ca1c8f9);
}
inline const EA::Reflection::Text &Gen::simsurface::CollectionName() const {
    ATTRIB_CODEGEN_GETLAYOUT(CollectionName);
}
inline bool Gen::simsurface::CollectionName(EA::Reflection::Text &result) const {
    result = CollectionName();
    return true;
}
inline bool Gen::simsurface::SET_CollectionName(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETLAYOUT(CollectionName, input);
}

inline bool Gen::simsurface::DEBUG_COLOUR(TAttrib<UMath::Vector4> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x740d3125);
}
inline const UMath::Vector4 &Gen::simsurface::DEBUG_COLOUR() const {
    ATTRIB_CODEGEN_GETVALUE(UMath::Vector4, 0x740d3125);
}
inline bool Gen::simsurface::DEBUG_COLOUR(UMath::Vector4 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(UMath::Vector4, 0x740d3125, result);
}
inline bool Gen::simsurface::SET_DEBUG_COLOUR(const UMath::Vector4 &input) {
    ATTRIB_CODEGEN_SETVALUE(UMath::Vector4, 0x740d3125, input);
}

inline bool Gen::simsurface::DRIVE_GRIP(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xc68f17c2);
}
inline const EA::Reflection::Float &Gen::simsurface::DRIVE_GRIP() const {
    ATTRIB_CODEGEN_GETLAYOUT(DRIVE_GRIP);
}
inline bool Gen::simsurface::DRIVE_GRIP(EA::Reflection::Float &result) const {
    result = DRIVE_GRIP();
    return true;
}
inline bool Gen::simsurface::SET_DRIVE_GRIP(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(DRIVE_GRIP, input);
}

inline bool Gen::simsurface::FFB_ROLL(TAttrib<FFBWaveRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(FFBWaveRecord, 0xba1297da);
}
inline const FFBWaveRecord &Gen::simsurface::FFB_ROLL(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(FFBWaveRecord, 0xba1297da, index);
}
inline bool Gen::simsurface::FFB_ROLL(FFBWaveRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(FFBWaveRecord, 0xba1297da, result, index);
}
inline unsigned int Gen::simsurface::Num_FFB_ROLL() const {
    ATTRIB_CODEGEN_GETLENGTH(0xba1297da);
}
inline bool Gen::simsurface::SET_FFB_ROLL(const FFBWaveRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(FFBWaveRecord, 0xba1297da, input, index);
}

inline bool Gen::simsurface::FFB_SKID(TAttrib<FFBWaveRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(FFBWaveRecord, 0x0c149044);
}
inline const FFBWaveRecord &Gen::simsurface::FFB_SKID(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(FFBWaveRecord, 0x0c149044, index);
}
inline bool Gen::simsurface::FFB_SKID(FFBWaveRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(FFBWaveRecord, 0x0c149044, result, index);
}
inline unsigned int Gen::simsurface::Num_FFB_SKID() const {
    ATTRIB_CODEGEN_GETLENGTH(0x0c149044);
}
inline bool Gen::simsurface::SET_FFB_SKID(const FFBWaveRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(FFBWaveRecord, 0x0c149044, input, index);
}

inline bool Gen::simsurface::FFB_SLIP(TAttrib<FFBWaveRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(FFBWaveRecord, 0x8fd11d27);
}
inline const FFBWaveRecord &Gen::simsurface::FFB_SLIP(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(FFBWaveRecord, 0x8fd11d27, index);
}
inline bool Gen::simsurface::FFB_SLIP(FFBWaveRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(FFBWaveRecord, 0x8fd11d27, result, index);
}
inline unsigned int Gen::simsurface::Num_FFB_SLIP() const {
    ATTRIB_CODEGEN_GETLENGTH(0x8fd11d27);
}
inline bool Gen::simsurface::SET_FFB_SLIP(const FFBWaveRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(FFBWaveRecord, 0x8fd11d27, input, index);
}

inline bool Gen::simsurface::GROUND_FRICTION(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x3fb1b342);
}
inline const EA::Reflection::Float &Gen::simsurface::GROUND_FRICTION() const {
    ATTRIB_CODEGEN_GETLAYOUT(GROUND_FRICTION);
}
inline bool Gen::simsurface::GROUND_FRICTION(EA::Reflection::Float &result) const {
    result = GROUND_FRICTION();
    return true;
}
inline bool Gen::simsurface::SET_GROUND_FRICTION(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(GROUND_FRICTION, input);
}

inline bool Gen::simsurface::LATERAL_GRIP(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x5ce7dba8);
}
inline const EA::Reflection::Float &Gen::simsurface::LATERAL_GRIP() const {
    ATTRIB_CODEGEN_GETLAYOUT(LATERAL_GRIP);
}
inline bool Gen::simsurface::LATERAL_GRIP(EA::Reflection::Float &result) const {
    result = LATERAL_GRIP();
    return true;
}
inline bool Gen::simsurface::SET_LATERAL_GRIP(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(LATERAL_GRIP, input);
}

inline bool Gen::simsurface::MATERIAL_STRENGTH(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x9fca0b40);
}
inline const EA::Reflection::Float &Gen::simsurface::MATERIAL_STRENGTH() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x9fca0b40);
}
inline bool Gen::simsurface::MATERIAL_STRENGTH(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x9fca0b40, result);
}
inline bool Gen::simsurface::SET_MATERIAL_STRENGTH(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x9fca0b40, input);
}

inline bool Gen::simsurface::ROLLING_RESISTANCE(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x9f2b0192);
}
inline const EA::Reflection::Float &Gen::simsurface::ROLLING_RESISTANCE() const {
    ATTRIB_CODEGEN_GETLAYOUT(ROLLING_RESISTANCE);
}
inline bool Gen::simsurface::ROLLING_RESISTANCE(EA::Reflection::Float &result) const {
    result = ROLLING_RESISTANCE();
    return true;
}
inline bool Gen::simsurface::SET_ROLLING_RESISTANCE(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(ROLLING_RESISTANCE, input);
}

inline bool Gen::simsurface::RenderNoise(TAttrib<RoadNoiseRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RoadNoiseRecord, 0x940b79c4);
}
inline const RoadNoiseRecord &Gen::simsurface::RenderNoise() const {
    ATTRIB_CODEGEN_GETLAYOUT(RenderNoise);
}
inline bool Gen::simsurface::RenderNoise(RoadNoiseRecord &result) const {
    result = RenderNoise();
    return true;
}
inline bool Gen::simsurface::SET_RenderNoise(const RoadNoiseRecord &input) {
    ATTRIB_CODEGEN_SETLAYOUT(RenderNoise, input);
}

inline bool Gen::simsurface::STICK(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xd72119b4);
}
inline const EA::Reflection::Float &Gen::simsurface::STICK() const {
    ATTRIB_CODEGEN_GETLAYOUT(STICK);
}
inline bool Gen::simsurface::STICK(EA::Reflection::Float &result) const {
    result = STICK();
    return true;
}
inline bool Gen::simsurface::SET_STICK(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(STICK, input);
}

inline bool Gen::simsurface::TireDriveEffects(TAttrib<TireEffectRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(TireEffectRecord, 0xfcc3efa6);
}
inline const TireEffectRecord &Gen::simsurface::TireDriveEffects(unsigned int index) const {
    ATTRIB_CODEGEN_GETLAYOUTINDEXED(TireEffectRecord, TireDriveEffects, index);
}
inline bool Gen::simsurface::TireDriveEffects(TireEffectRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(TireDriveEffects, result, index);
}
inline unsigned int Gen::simsurface::Num_TireDriveEffects() const {
    ATTRIB_CODEGEN_GETLAYOUTLENGTH(TireDriveEffects);
}
inline bool Gen::simsurface::SET_TireDriveEffects(const TireEffectRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETLAYOUTINDEXED(TireDriveEffects, input, index);
}

inline bool Gen::simsurface::TireSlideEffects(TAttrib<TireEffectRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(TireEffectRecord, 0x27b6871c);
}
inline const TireEffectRecord &Gen::simsurface::TireSlideEffects(unsigned int index) const {
    ATTRIB_CODEGEN_GETLAYOUTINDEXED(TireEffectRecord, TireSlideEffects, index);
}
inline bool Gen::simsurface::TireSlideEffects(TireEffectRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(TireSlideEffects, result, index);
}
inline unsigned int Gen::simsurface::Num_TireSlideEffects() const {
    ATTRIB_CODEGEN_GETLAYOUTLENGTH(TireSlideEffects);
}
inline bool Gen::simsurface::SET_TireSlideEffects(const TireEffectRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETLAYOUTINDEXED(TireSlideEffects, input, index);
}

inline bool Gen::simsurface::TireSlipEffects(TAttrib<TireEffectRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(TireEffectRecord, 0x68411a69);
}
inline const TireEffectRecord &Gen::simsurface::TireSlipEffects(unsigned int index) const {
    ATTRIB_CODEGEN_GETLAYOUTINDEXED(TireEffectRecord, TireSlipEffects, index);
}
inline bool Gen::simsurface::TireSlipEffects(TireEffectRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(TireSlipEffects, result, index);
}
inline unsigned int Gen::simsurface::Num_TireSlipEffects() const {
    ATTRIB_CODEGEN_GETLAYOUTLENGTH(TireSlipEffects);
}
inline bool Gen::simsurface::SET_TireSlipEffects(const TireEffectRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETLAYOUTINDEXED(TireSlipEffects, input, index);
}

inline bool Gen::simsurface::WORLD_FRICTION(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x875165fe);
}
inline const EA::Reflection::Float &Gen::simsurface::WORLD_FRICTION() const {
    ATTRIB_CODEGEN_GETLAYOUT(WORLD_FRICTION);
}
inline bool Gen::simsurface::WORLD_FRICTION(EA::Reflection::Float &result) const {
    result = WORLD_FRICTION();
    return true;
}
inline bool Gen::simsurface::SET_WORLD_FRICTION(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(WORLD_FRICTION, input);
}

inline bool Gen::simsurface::WheelEffectFrequency(TAttrib<EA::Reflection::UInt16> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt16, 0x968c3422);
}
inline const EA::Reflection::UInt16 &Gen::simsurface::WheelEffectFrequency() const {
    ATTRIB_CODEGEN_GETLAYOUT(WheelEffectFrequency);
}
inline bool Gen::simsurface::WheelEffectFrequency(EA::Reflection::UInt16 &result) const {
    result = WheelEffectFrequency();
    return true;
}
inline bool Gen::simsurface::SET_WheelEffectFrequency(const EA::Reflection::UInt16 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(WheelEffectFrequency, input);
}

inline bool Gen::simsurface::WheelEffectIntensity(TAttrib<EA::Reflection::UInt8> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt8, 0xb7e0af1c);
}
inline const EA::Reflection::UInt8 &Gen::simsurface::WheelEffectIntensity() const {
    ATTRIB_CODEGEN_GETLAYOUT(WheelEffectIntensity);
}
inline bool Gen::simsurface::WheelEffectIntensity(EA::Reflection::UInt8 &result) const {
    result = WheelEffectIntensity();
    return true;
}
inline bool Gen::simsurface::SET_WheelEffectIntensity(const EA::Reflection::UInt8 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(WheelEffectIntensity, input);
}

inline bool Gen::simsurface::WheelSurfaceEffect(TAttrib<SurfaceEffectType> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(SurfaceEffectType, 0x46226745);
}
inline const SurfaceEffectType &Gen::simsurface::WheelSurfaceEffect() const {
    ATTRIB_CODEGEN_GETVALUE(SurfaceEffectType, 0x46226745);
}
inline bool Gen::simsurface::WheelSurfaceEffect(SurfaceEffectType &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(SurfaceEffectType, 0x46226745, result);
}
inline bool Gen::simsurface::SET_WheelSurfaceEffect(const SurfaceEffectType &input) {
    ATTRIB_CODEGEN_SETVALUE(SurfaceEffectType, 0x46226745, input);
}

}; // namespace Attrib

#endif
