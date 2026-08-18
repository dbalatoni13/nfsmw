#ifndef _attrib_gen_ecar_h
#define _attrib_gen_ecar_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct ecar : Instance {
    struct _LayoutStruct {
        Private _Array_TireOffsets;                   // offset 0x0, size 0x8
        char _Pad_TireOffsets[8];                     // offset 0x8, size 0x8
        UMath::Vector4 TireOffsets[4];                // offset 0x10, size 0x40
        Private _Array_TireSkidWidthKitScale;         // offset 0x50, size 0x8
        UMath::Vector2 TireSkidWidthKitScale[7];      // offset 0x58, size 0x38
        Private _Array_SkidFX;                        // offset 0x90, size 0x8
        EA::Reflection::Float SkidFX[2];              // offset 0x98, size 0x8
        CarBodyMotion BodyRoll;                       // offset 0xa0, size 0xc
        CarBodyMotion BodySquat;                      // offset 0xac, size 0xc
        CarBodyMotion BodyDive;                       // offset 0xb8, size 0xc
        Private _Array_SlipFX;                        // offset 0xc4, size 0x8
        EA::Reflection::Float SlipFX[2];              // offset 0xcc, size 0x8
        Private _Array_TireSkidWidth;                 // offset 0xd4, size 0x8
        EA::Reflection::Float TireSkidWidth[4];       // offset 0xdc, size 0x10
        EA::Reflection::Text CollectionName;          // offset 0xec, size 0x4
        EA::Reflection::Float CamberFront;            // offset 0xf0, size 0x4
        EA::Reflection::Float ReflectionOffset;       // offset 0xf4, size 0x4
        EA::Reflection::Float CamberRear;             // offset 0xf8, size 0x4
        EA::Reflection::Float RideHeight;             // offset 0xfc, size 0x4
        Private _Array_KitWheelOffsetRear;            // offset 0x100, size 0x8
        EA::Reflection::UInt8 KitWheelOffsetRear[6];  // offset 0x108, size 0x6
        Private _Array_KitWheelOffsetFront;           // offset 0x10e, size 0x8
        EA::Reflection::UInt8 KitWheelOffsetFront[6]; // offset 0x116, size 0x6
        EA::Reflection::Int8 WheelSpokeCount;         // offset 0x11c, size 0x1
    };

    typedef CarBodyMotion TypeOf_BodyDive;
    typedef CarBodyMotion TypeOf_BodyRoll;
    typedef CarBodyMotion TypeOf_BodySquat;
    typedef EA::Reflection::Float TypeOf_CamberFront;
    typedef EA::Reflection::Float TypeOf_CamberRear;
    typedef RefSpec TypeOf_CameraInfo_Bumper;
    typedef RefSpec TypeOf_CameraInfo_Close;
    typedef RefSpec TypeOf_CameraInfo_Drift;
    typedef RefSpec TypeOf_CameraInfo_Far;
    typedef RefSpec TypeOf_CameraInfo_Hood;
    typedef RefSpec TypeOf_CameraInfo_Pursuit;
    typedef RefSpec TypeOf_CameraInfo_SuperFar;
    typedef EA::Reflection::Text TypeOf_CollectionName;
    typedef RefSpec TypeOf_DamageEffect;
    typedef RefSpec TypeOf_DeathEffect;
    typedef RefSpec TypeOf_EngineBlownEffect;
    typedef EA::Reflection::Float TypeOf_EngineRev;
    typedef EA::Reflection::Float TypeOf_EngineRevAngle;
    typedef EA::Reflection::Float TypeOf_EngineRevSpeed;
    typedef EA::Reflection::Float TypeOf_EngineVibrationFreq;
    typedef EA::Reflection::Float TypeOf_EngineVibrationMax;
    typedef EA::Reflection::Float TypeOf_EngineVibrationMin;
    typedef EA::Reflection::Float TypeOf_ExtraPitch;
    typedef EA::Reflection::Float TypeOf_ExtraRearTireOffset;
    typedef EA::Reflection::Float TypeOf_FECompressions;
    typedef EA::Reflection::Float TypeOf_FrontCamber;
    typedef EA::Reflection::Bool TypeOf_IsSkinned;
    typedef EA::Reflection::UInt8 TypeOf_KitWheelOffsetFront;
    typedef EA::Reflection::UInt8 TypeOf_KitWheelOffsetRear;
    typedef RefSpec TypeOf_LIGHT_COPBLUE;
    typedef RefSpec TypeOf_LIGHT_COPRED;
    typedef RefSpec TypeOf_LIGHT_COPWHITE;
    typedef EA::Reflection::Float TypeOf_MaxTireSteer;
    typedef RefSpec TypeOf_MissShiftEffect;
    typedef RefSpec TypeOf_NOSEffect;
    typedef EA::Reflection::Float TypeOf_ReflectionOffset;
    typedef EA::Reflection::Float TypeOf_RideHeight;
    typedef EA::Reflection::Float TypeOf_RoadNoise;
    typedef EA::Reflection::Float TypeOf_ShiftAngle;
    typedef EA::Reflection::Float TypeOf_ShiftSpeed;
    typedef EA::Reflection::Float TypeOf_SkidFX;
    typedef EA::Reflection::Float TypeOf_SlipFX;
    typedef EA::Reflection::Float TypeOf_SteerSpeed;
    typedef UMath::Vector4 TypeOf_TireOffsets;
    typedef EA::Reflection::Float TypeOf_TireSkidWidth;
    typedef UMath::Vector2 TypeOf_TireSkidWidthKitScale;
    typedef EA::Reflection::Float TypeOf_WheelHopScale;
    typedef EA::Reflection::Int8 TypeOf_WheelSpokeCount;
    typedef EA::Reflection::Float TypeOf_WheelWell;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("ecar");
    ecar(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ecar(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ecar(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ecar(const ecar &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ecar(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~ecar() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0xa5b543b7;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0xa5b543b7, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const ecar &operator=(const ecar &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const ecar &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool BodyDive(TAttrib<CarBodyMotion> &result) const;
    bool BodyDive(CarBodyMotion &result) const;
    const CarBodyMotion &BodyDive() const;
    bool SET_BodyDive(const CarBodyMotion &input);

    bool BodyRoll(TAttrib<CarBodyMotion> &result) const;
    bool BodyRoll(CarBodyMotion &result) const;
    const CarBodyMotion &BodyRoll() const;
    bool SET_BodyRoll(const CarBodyMotion &input);

    bool BodySquat(TAttrib<CarBodyMotion> &result) const;
    bool BodySquat(CarBodyMotion &result) const;
    const CarBodyMotion &BodySquat() const;
    bool SET_BodySquat(const CarBodyMotion &input);

    bool CamberFront(TAttrib<EA::Reflection::Float> &result) const;
    bool CamberFront(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &CamberFront() const;
    bool SET_CamberFront(const EA::Reflection::Float &input);

    bool CamberRear(TAttrib<EA::Reflection::Float> &result) const;
    bool CamberRear(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &CamberRear() const;
    bool SET_CamberRear(const EA::Reflection::Float &input);

    bool CameraInfo_Bumper(TAttrib<RefSpec> &result) const;
    bool CameraInfo_Bumper(RefSpec &result) const;
    const RefSpec &CameraInfo_Bumper() const;
    bool SET_CameraInfo_Bumper(const RefSpec &input);

    bool CameraInfo_Close(TAttrib<RefSpec> &result) const;
    bool CameraInfo_Close(RefSpec &result) const;
    const RefSpec &CameraInfo_Close() const;
    bool SET_CameraInfo_Close(const RefSpec &input);

    bool CameraInfo_Drift(TAttrib<RefSpec> &result) const;
    bool CameraInfo_Drift(RefSpec &result) const;
    const RefSpec &CameraInfo_Drift() const;
    bool SET_CameraInfo_Drift(const RefSpec &input);

    bool CameraInfo_Far(TAttrib<RefSpec> &result) const;
    bool CameraInfo_Far(RefSpec &result) const;
    const RefSpec &CameraInfo_Far() const;
    bool SET_CameraInfo_Far(const RefSpec &input);

    bool CameraInfo_Hood(TAttrib<RefSpec> &result) const;
    bool CameraInfo_Hood(RefSpec &result) const;
    const RefSpec &CameraInfo_Hood() const;
    bool SET_CameraInfo_Hood(const RefSpec &input);

    bool CameraInfo_Pursuit(TAttrib<RefSpec> &result) const;
    bool CameraInfo_Pursuit(RefSpec &result) const;
    const RefSpec &CameraInfo_Pursuit() const;
    bool SET_CameraInfo_Pursuit(const RefSpec &input);

    bool CameraInfo_SuperFar(TAttrib<RefSpec> &result) const;
    bool CameraInfo_SuperFar(RefSpec &result) const;
    const RefSpec &CameraInfo_SuperFar() const;
    bool SET_CameraInfo_SuperFar(const RefSpec &input);

    bool CollectionName(TAttrib<EA::Reflection::Text> &result) const;
    bool CollectionName(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &CollectionName() const;
    bool SET_CollectionName(const EA::Reflection::Text &input);

    bool DamageEffect(TAttrib<RefSpec> &result) const;
    bool DamageEffect(RefSpec &result) const;
    const RefSpec &DamageEffect() const;
    bool SET_DamageEffect(const RefSpec &input);

    bool DeathEffect(TAttrib<RefSpec> &result) const;
    bool DeathEffect(RefSpec &result) const;
    const RefSpec &DeathEffect() const;
    bool SET_DeathEffect(const RefSpec &input);

    bool EngineBlownEffect(TAttrib<RefSpec> &result) const;
    bool EngineBlownEffect(RefSpec &result) const;
    const RefSpec &EngineBlownEffect() const;
    bool SET_EngineBlownEffect(const RefSpec &input);

    bool EngineRev(TAttrib<EA::Reflection::Float> &result) const;
    bool EngineRev(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &EngineRev() const;
    bool SET_EngineRev(const EA::Reflection::Float &input);

    bool EngineRevAngle(TAttrib<EA::Reflection::Float> &result) const;
    bool EngineRevAngle(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &EngineRevAngle() const;
    bool SET_EngineRevAngle(const EA::Reflection::Float &input);

    bool EngineRevSpeed(TAttrib<EA::Reflection::Float> &result) const;
    bool EngineRevSpeed(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &EngineRevSpeed() const;
    bool SET_EngineRevSpeed(const EA::Reflection::Float &input);

    bool EngineVibrationFreq(TAttrib<EA::Reflection::Float> &result) const;
    bool EngineVibrationFreq(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &EngineVibrationFreq() const;
    bool SET_EngineVibrationFreq(const EA::Reflection::Float &input);

    bool EngineVibrationMax(TAttrib<EA::Reflection::Float> &result) const;
    bool EngineVibrationMax(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &EngineVibrationMax() const;
    bool SET_EngineVibrationMax(const EA::Reflection::Float &input);

    bool EngineVibrationMin(TAttrib<EA::Reflection::Float> &result) const;
    bool EngineVibrationMin(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &EngineVibrationMin() const;
    bool SET_EngineVibrationMin(const EA::Reflection::Float &input);

    bool ExtraPitch(TAttrib<EA::Reflection::Float> &result) const;
    bool ExtraPitch(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &ExtraPitch() const;
    bool SET_ExtraPitch(const EA::Reflection::Float &input);

    bool ExtraRearTireOffset(TAttrib<EA::Reflection::Float> &result) const;
    bool ExtraRearTireOffset(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &ExtraRearTireOffset() const;
    bool SET_ExtraRearTireOffset(const EA::Reflection::Float &input);

    bool FECompressions(TAttrib<EA::Reflection::Float> &result) const;
    bool FECompressions(EA::Reflection::Float &result, unsigned int index) const;
    const EA::Reflection::Float &FECompressions(unsigned int index) const;
    unsigned int Num_FECompressions() const;
    bool SET_FECompressions(const EA::Reflection::Float &input, unsigned int index);

    bool FrontCamber(TAttrib<EA::Reflection::Float> &result) const;
    bool FrontCamber(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &FrontCamber() const;
    bool SET_FrontCamber(const EA::Reflection::Float &input);

    bool IsSkinned(TAttrib<EA::Reflection::Bool> &result) const;
    bool IsSkinned(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &IsSkinned() const;
    bool SET_IsSkinned(const EA::Reflection::Bool &input);

    bool KitWheelOffsetFront(TAttrib<EA::Reflection::UInt8> &result) const;
    bool KitWheelOffsetFront(EA::Reflection::UInt8 &result, unsigned int index) const;
    const EA::Reflection::UInt8 &KitWheelOffsetFront(unsigned int index) const;
    unsigned int Num_KitWheelOffsetFront() const;
    bool SET_KitWheelOffsetFront(const EA::Reflection::UInt8 &input, unsigned int index);

    bool KitWheelOffsetRear(TAttrib<EA::Reflection::UInt8> &result) const;
    bool KitWheelOffsetRear(EA::Reflection::UInt8 &result, unsigned int index) const;
    const EA::Reflection::UInt8 &KitWheelOffsetRear(unsigned int index) const;
    unsigned int Num_KitWheelOffsetRear() const;
    bool SET_KitWheelOffsetRear(const EA::Reflection::UInt8 &input, unsigned int index);

    bool LIGHT_COPBLUE(TAttrib<RefSpec> &result) const;
    bool LIGHT_COPBLUE(RefSpec &result) const;
    const RefSpec &LIGHT_COPBLUE() const;
    bool SET_LIGHT_COPBLUE(const RefSpec &input);

    bool LIGHT_COPRED(TAttrib<RefSpec> &result) const;
    bool LIGHT_COPRED(RefSpec &result) const;
    const RefSpec &LIGHT_COPRED() const;
    bool SET_LIGHT_COPRED(const RefSpec &input);

    bool LIGHT_COPWHITE(TAttrib<RefSpec> &result) const;
    bool LIGHT_COPWHITE(RefSpec &result) const;
    const RefSpec &LIGHT_COPWHITE() const;
    bool SET_LIGHT_COPWHITE(const RefSpec &input);

    bool MaxTireSteer(TAttrib<EA::Reflection::Float> &result) const;
    bool MaxTireSteer(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &MaxTireSteer() const;
    bool SET_MaxTireSteer(const EA::Reflection::Float &input);

    bool MissShiftEffect(TAttrib<RefSpec> &result) const;
    bool MissShiftEffect(RefSpec &result) const;
    const RefSpec &MissShiftEffect() const;
    bool SET_MissShiftEffect(const RefSpec &input);

    bool NOSEffect(TAttrib<RefSpec> &result) const;
    bool NOSEffect(RefSpec &result) const;
    const RefSpec &NOSEffect() const;
    bool SET_NOSEffect(const RefSpec &input);

    bool ReflectionOffset(TAttrib<EA::Reflection::Float> &result) const;
    bool ReflectionOffset(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &ReflectionOffset() const;
    bool SET_ReflectionOffset(const EA::Reflection::Float &input);

    bool RideHeight(TAttrib<EA::Reflection::Float> &result) const;
    bool RideHeight(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &RideHeight() const;
    bool SET_RideHeight(const EA::Reflection::Float &input);

    bool RoadNoise(TAttrib<EA::Reflection::Float> &result) const;
    bool RoadNoise(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &RoadNoise() const;
    bool SET_RoadNoise(const EA::Reflection::Float &input);

    bool ShiftAngle(TAttrib<EA::Reflection::Float> &result) const;
    bool ShiftAngle(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &ShiftAngle() const;
    bool SET_ShiftAngle(const EA::Reflection::Float &input);

    bool ShiftSpeed(TAttrib<EA::Reflection::Float> &result) const;
    bool ShiftSpeed(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &ShiftSpeed() const;
    bool SET_ShiftSpeed(const EA::Reflection::Float &input);

    bool SkidFX(TAttrib<EA::Reflection::Float> &result) const;
    bool SkidFX(EA::Reflection::Float &result, unsigned int index) const;
    const EA::Reflection::Float &SkidFX(unsigned int index) const;
    unsigned int Num_SkidFX() const;
    bool SET_SkidFX(const EA::Reflection::Float &input, unsigned int index);

    bool SlipFX(TAttrib<EA::Reflection::Float> &result) const;
    bool SlipFX(EA::Reflection::Float &result, unsigned int index) const;
    const EA::Reflection::Float &SlipFX(unsigned int index) const;
    unsigned int Num_SlipFX() const;
    bool SET_SlipFX(const EA::Reflection::Float &input, unsigned int index);

    bool SteerSpeed(TAttrib<EA::Reflection::Float> &result) const;
    bool SteerSpeed(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &SteerSpeed() const;
    bool SET_SteerSpeed(const EA::Reflection::Float &input);

    bool TireOffsets(TAttrib<UMath::Vector4> &result) const;
    bool TireOffsets(UMath::Vector4 &result, unsigned int index) const;
    const UMath::Vector4 &TireOffsets(unsigned int index) const;
    unsigned int Num_TireOffsets() const;
    bool SET_TireOffsets(const UMath::Vector4 &input, unsigned int index);

    bool TireSkidWidth(TAttrib<EA::Reflection::Float> &result) const;
    bool TireSkidWidth(EA::Reflection::Float &result, unsigned int index) const;
    const EA::Reflection::Float &TireSkidWidth(unsigned int index) const;
    unsigned int Num_TireSkidWidth() const;
    bool SET_TireSkidWidth(const EA::Reflection::Float &input, unsigned int index);

    bool TireSkidWidthKitScale(TAttrib<UMath::Vector2> &result) const;
    bool TireSkidWidthKitScale(UMath::Vector2 &result, unsigned int index) const;
    const UMath::Vector2 &TireSkidWidthKitScale(unsigned int index) const;
    unsigned int Num_TireSkidWidthKitScale() const;
    bool SET_TireSkidWidthKitScale(const UMath::Vector2 &input, unsigned int index);

    bool WheelHopScale(TAttrib<EA::Reflection::Float> &result) const;
    bool WheelHopScale(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &WheelHopScale() const;
    bool SET_WheelHopScale(const EA::Reflection::Float &input);

    bool WheelSpokeCount(TAttrib<EA::Reflection::Int8> &result) const;
    bool WheelSpokeCount(EA::Reflection::Int8 &result) const;
    const EA::Reflection::Int8 &WheelSpokeCount() const;
    bool SET_WheelSpokeCount(const EA::Reflection::Int8 &input);

    bool WheelWell(TAttrib<EA::Reflection::Float> &result) const;
    bool WheelWell(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &WheelWell() const;
    bool SET_WheelWell(const EA::Reflection::Float &input);

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    ecar &ConvertFromInstance(Instance &src) {}
    const ecar &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key ecar = 0xa5b543b7;

}; // namespace ClassName

namespace Hash {
namespace ecar {

static const Key BodyDive = 0x08e34911;
static const Key BodyRoll = 0xf9d98a1b;
static const Key BodySquat = 0x73117117;
static const Key CamberFront = 0x8c032c9f;
static const Key CamberRear = 0x87d39b4f;
static const Key CameraInfo_Bumper = 0x585517f3;
static const Key CameraInfo_Close = 0x0c2da793;
static const Key CameraInfo_Drift = 0x4b675dc8;
static const Key CameraInfo_Far = 0xccf03cb3;
static const Key CameraInfo_Hood = 0xd74c1435;
static const Key CameraInfo_Pursuit = 0xd76a6fad;
static const Key CameraInfo_SuperFar = 0x10204a90;
static const Key CollectionName = 0x9ca1c8f9;
static const Key DamageEffect = 0x30b2997b;
static const Key DeathEffect = 0xf7b59fc7;
static const Key EngineBlownEffect = 0xd9cca9a3;
static const Key EngineRev = 0x564773be;
static const Key EngineRevAngle = 0xe2c63383;
static const Key EngineRevSpeed = 0xfaad3e61;
static const Key EngineVibrationFreq = 0x80e657ff;
static const Key EngineVibrationMax = 0xf947fe58;
static const Key EngineVibrationMin = 0xe91f59bd;
static const Key ExtraPitch = 0xe95257c2;
static const Key ExtraRearTireOffset = 0xb1304fde;
static const Key FECompressions = 0x8d5beb72;
static const Key FrontCamber = 0xeed9ca5d;
static const Key IsSkinned = 0xd9102c65;
static const Key KitWheelOffsetFront = 0xfd3077e8;
static const Key KitWheelOffsetRear = 0xccc484dc;
static const Key LIGHT_COPBLUE = 0x82094cc9;
static const Key LIGHT_COPRED = 0x16437af7;
static const Key LIGHT_COPWHITE = 0x1ee00adf;
static const Key MaxTireSteer = 0xa9633fde;
static const Key MissShiftEffect = 0xb699b7be;
static const Key NOSEffect = 0x60cec115;
static const Key ReflectionOffset = 0xae298718;
static const Key RideHeight = 0x3996ab3b;
static const Key RoadNoise = 0x13eb7da2;
static const Key ShiftAngle = 0x24e25513;
static const Key ShiftSpeed = 0xda27cd35;
static const Key SkidFX = 0xadb52f00;
static const Key SlipFX = 0x01ac1449;
static const Key SteerSpeed = 0x79356463;
static const Key TireOffsets = 0x5e39d209;
static const Key TireSkidWidth = 0x8f952090;
static const Key TireSkidWidthKitScale = 0x7ce83504;
static const Key WheelHopScale = 0x44e52948;
static const Key WheelSpokeCount = 0x96a8896c;
static const Key WheelWell = 0x24cb15ea;

}; // namespace ecar
}; // namespace Hash

inline Key Gen::ecar::ClassKey() {
    return ClassName::ecar;
}

inline bool Gen::ecar::BodyDive(TAttrib<CarBodyMotion> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(CarBodyMotion, 0x08e34911);
}
inline const CarBodyMotion &Gen::ecar::BodyDive() const {
    ATTRIB_CODEGEN_GETLAYOUT(BodyDive);
}
inline bool Gen::ecar::BodyDive(CarBodyMotion &result) const {
    result = BodyDive();
    return true;
}
inline bool Gen::ecar::SET_BodyDive(const CarBodyMotion &input) {
    ATTRIB_CODEGEN_SETLAYOUT(BodyDive, input);
}

inline bool Gen::ecar::BodyRoll(TAttrib<CarBodyMotion> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(CarBodyMotion, 0xf9d98a1b);
}
inline const CarBodyMotion &Gen::ecar::BodyRoll() const {
    ATTRIB_CODEGEN_GETLAYOUT(BodyRoll);
}
inline bool Gen::ecar::BodyRoll(CarBodyMotion &result) const {
    result = BodyRoll();
    return true;
}
inline bool Gen::ecar::SET_BodyRoll(const CarBodyMotion &input) {
    ATTRIB_CODEGEN_SETLAYOUT(BodyRoll, input);
}

inline bool Gen::ecar::BodySquat(TAttrib<CarBodyMotion> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(CarBodyMotion, 0x73117117);
}
inline const CarBodyMotion &Gen::ecar::BodySquat() const {
    ATTRIB_CODEGEN_GETLAYOUT(BodySquat);
}
inline bool Gen::ecar::BodySquat(CarBodyMotion &result) const {
    result = BodySquat();
    return true;
}
inline bool Gen::ecar::SET_BodySquat(const CarBodyMotion &input) {
    ATTRIB_CODEGEN_SETLAYOUT(BodySquat, input);
}

inline bool Gen::ecar::CamberFront(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x8c032c9f);
}
inline const EA::Reflection::Float &Gen::ecar::CamberFront() const {
    ATTRIB_CODEGEN_GETLAYOUT(CamberFront);
}
inline bool Gen::ecar::CamberFront(EA::Reflection::Float &result) const {
    result = CamberFront();
    return true;
}
inline bool Gen::ecar::SET_CamberFront(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(CamberFront, input);
}

inline bool Gen::ecar::CamberRear(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x87d39b4f);
}
inline const EA::Reflection::Float &Gen::ecar::CamberRear() const {
    ATTRIB_CODEGEN_GETLAYOUT(CamberRear);
}
inline bool Gen::ecar::CamberRear(EA::Reflection::Float &result) const {
    result = CamberRear();
    return true;
}
inline bool Gen::ecar::SET_CamberRear(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(CamberRear, input);
}

inline bool Gen::ecar::CameraInfo_Bumper(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x585517f3);
}
inline const RefSpec &Gen::ecar::CameraInfo_Bumper() const {
    ATTRIB_CODEGEN_GETVALUE(RefSpec, 0x585517f3);
}
inline bool Gen::ecar::CameraInfo_Bumper(RefSpec &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0x585517f3, result);
}
inline bool Gen::ecar::SET_CameraInfo_Bumper(const RefSpec &input) {
    ATTRIB_CODEGEN_SETVALUE(RefSpec, 0x585517f3, input);
}

inline bool Gen::ecar::CameraInfo_Close(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x0c2da793);
}
inline const RefSpec &Gen::ecar::CameraInfo_Close() const {
    ATTRIB_CODEGEN_GETVALUE(RefSpec, 0x0c2da793);
}
inline bool Gen::ecar::CameraInfo_Close(RefSpec &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0x0c2da793, result);
}
inline bool Gen::ecar::SET_CameraInfo_Close(const RefSpec &input) {
    ATTRIB_CODEGEN_SETVALUE(RefSpec, 0x0c2da793, input);
}

inline bool Gen::ecar::CameraInfo_Drift(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x4b675dc8);
}
inline const RefSpec &Gen::ecar::CameraInfo_Drift() const {
    ATTRIB_CODEGEN_GETVALUE(RefSpec, 0x4b675dc8);
}
inline bool Gen::ecar::CameraInfo_Drift(RefSpec &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0x4b675dc8, result);
}
inline bool Gen::ecar::SET_CameraInfo_Drift(const RefSpec &input) {
    ATTRIB_CODEGEN_SETVALUE(RefSpec, 0x4b675dc8, input);
}

inline bool Gen::ecar::CameraInfo_Far(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0xccf03cb3);
}
inline const RefSpec &Gen::ecar::CameraInfo_Far() const {
    ATTRIB_CODEGEN_GETVALUE(RefSpec, 0xccf03cb3);
}
inline bool Gen::ecar::CameraInfo_Far(RefSpec &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0xccf03cb3, result);
}
inline bool Gen::ecar::SET_CameraInfo_Far(const RefSpec &input) {
    ATTRIB_CODEGEN_SETVALUE(RefSpec, 0xccf03cb3, input);
}

inline bool Gen::ecar::CameraInfo_Hood(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0xd74c1435);
}
inline const RefSpec &Gen::ecar::CameraInfo_Hood() const {
    ATTRIB_CODEGEN_GETVALUE(RefSpec, 0xd74c1435);
}
inline bool Gen::ecar::CameraInfo_Hood(RefSpec &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0xd74c1435, result);
}
inline bool Gen::ecar::SET_CameraInfo_Hood(const RefSpec &input) {
    ATTRIB_CODEGEN_SETVALUE(RefSpec, 0xd74c1435, input);
}

inline bool Gen::ecar::CameraInfo_Pursuit(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0xd76a6fad);
}
inline const RefSpec &Gen::ecar::CameraInfo_Pursuit() const {
    ATTRIB_CODEGEN_GETVALUE(RefSpec, 0xd76a6fad);
}
inline bool Gen::ecar::CameraInfo_Pursuit(RefSpec &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0xd76a6fad, result);
}
inline bool Gen::ecar::SET_CameraInfo_Pursuit(const RefSpec &input) {
    ATTRIB_CODEGEN_SETVALUE(RefSpec, 0xd76a6fad, input);
}

inline bool Gen::ecar::CameraInfo_SuperFar(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x10204a90);
}
inline const RefSpec &Gen::ecar::CameraInfo_SuperFar() const {
    ATTRIB_CODEGEN_GETVALUE(RefSpec, 0x10204a90);
}
inline bool Gen::ecar::CameraInfo_SuperFar(RefSpec &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0x10204a90, result);
}
inline bool Gen::ecar::SET_CameraInfo_SuperFar(const RefSpec &input) {
    ATTRIB_CODEGEN_SETVALUE(RefSpec, 0x10204a90, input);
}

inline bool Gen::ecar::CollectionName(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x9ca1c8f9);
}
inline const EA::Reflection::Text &Gen::ecar::CollectionName() const {
    ATTRIB_CODEGEN_GETLAYOUT(CollectionName);
}
inline bool Gen::ecar::CollectionName(EA::Reflection::Text &result) const {
    result = CollectionName();
    return true;
}
inline bool Gen::ecar::SET_CollectionName(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETLAYOUT(CollectionName, input);
}

inline bool Gen::ecar::DamageEffect(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x30b2997b);
}
inline const RefSpec &Gen::ecar::DamageEffect() const {
    ATTRIB_CODEGEN_GETVALUE(RefSpec, 0x30b2997b);
}
inline bool Gen::ecar::DamageEffect(RefSpec &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0x30b2997b, result);
}
inline bool Gen::ecar::SET_DamageEffect(const RefSpec &input) {
    ATTRIB_CODEGEN_SETVALUE(RefSpec, 0x30b2997b, input);
}

inline bool Gen::ecar::DeathEffect(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0xf7b59fc7);
}
inline const RefSpec &Gen::ecar::DeathEffect() const {
    ATTRIB_CODEGEN_GETVALUE(RefSpec, 0xf7b59fc7);
}
inline bool Gen::ecar::DeathEffect(RefSpec &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0xf7b59fc7, result);
}
inline bool Gen::ecar::SET_DeathEffect(const RefSpec &input) {
    ATTRIB_CODEGEN_SETVALUE(RefSpec, 0xf7b59fc7, input);
}

inline bool Gen::ecar::EngineBlownEffect(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0xd9cca9a3);
}
inline const RefSpec &Gen::ecar::EngineBlownEffect() const {
    ATTRIB_CODEGEN_GETVALUE(RefSpec, 0xd9cca9a3);
}
inline bool Gen::ecar::EngineBlownEffect(RefSpec &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0xd9cca9a3, result);
}
inline bool Gen::ecar::SET_EngineBlownEffect(const RefSpec &input) {
    ATTRIB_CODEGEN_SETVALUE(RefSpec, 0xd9cca9a3, input);
}

inline bool Gen::ecar::EngineRev(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x564773be);
}
inline const EA::Reflection::Float &Gen::ecar::EngineRev() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x564773be);
}
inline bool Gen::ecar::EngineRev(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x564773be, result);
}
inline bool Gen::ecar::SET_EngineRev(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x564773be, input);
}

inline bool Gen::ecar::EngineRevAngle(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xe2c63383);
}
inline const EA::Reflection::Float &Gen::ecar::EngineRevAngle() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xe2c63383);
}
inline bool Gen::ecar::EngineRevAngle(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xe2c63383, result);
}
inline bool Gen::ecar::SET_EngineRevAngle(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xe2c63383, input);
}

inline bool Gen::ecar::EngineRevSpeed(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xfaad3e61);
}
inline const EA::Reflection::Float &Gen::ecar::EngineRevSpeed() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xfaad3e61);
}
inline bool Gen::ecar::EngineRevSpeed(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xfaad3e61, result);
}
inline bool Gen::ecar::SET_EngineRevSpeed(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xfaad3e61, input);
}

inline bool Gen::ecar::EngineVibrationFreq(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x80e657ff);
}
inline const EA::Reflection::Float &Gen::ecar::EngineVibrationFreq() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x80e657ff);
}
inline bool Gen::ecar::EngineVibrationFreq(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x80e657ff, result);
}
inline bool Gen::ecar::SET_EngineVibrationFreq(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x80e657ff, input);
}

inline bool Gen::ecar::EngineVibrationMax(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xf947fe58);
}
inline const EA::Reflection::Float &Gen::ecar::EngineVibrationMax() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xf947fe58);
}
inline bool Gen::ecar::EngineVibrationMax(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xf947fe58, result);
}
inline bool Gen::ecar::SET_EngineVibrationMax(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xf947fe58, input);
}

inline bool Gen::ecar::EngineVibrationMin(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xe91f59bd);
}
inline const EA::Reflection::Float &Gen::ecar::EngineVibrationMin() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xe91f59bd);
}
inline bool Gen::ecar::EngineVibrationMin(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xe91f59bd, result);
}
inline bool Gen::ecar::SET_EngineVibrationMin(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xe91f59bd, input);
}

inline bool Gen::ecar::ExtraPitch(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xe95257c2);
}
inline const EA::Reflection::Float &Gen::ecar::ExtraPitch() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xe95257c2);
}
inline bool Gen::ecar::ExtraPitch(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xe95257c2, result);
}
inline bool Gen::ecar::SET_ExtraPitch(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xe95257c2, input);
}

inline bool Gen::ecar::ExtraRearTireOffset(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xb1304fde);
}
inline const EA::Reflection::Float &Gen::ecar::ExtraRearTireOffset() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xb1304fde);
}
inline bool Gen::ecar::ExtraRearTireOffset(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xb1304fde, result);
}
inline bool Gen::ecar::SET_ExtraRearTireOffset(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xb1304fde, input);
}

inline bool Gen::ecar::FECompressions(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x8d5beb72);
}
inline const EA::Reflection::Float &Gen::ecar::FECompressions(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(EA::Reflection::Float, 0x8d5beb72, index);
}
inline bool Gen::ecar::FECompressions(EA::Reflection::Float &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(EA::Reflection::Float, 0x8d5beb72, result, index);
}
inline unsigned int Gen::ecar::Num_FECompressions() const {
    ATTRIB_CODEGEN_GETLENGTH(0x8d5beb72);
}
inline bool Gen::ecar::SET_FECompressions(const EA::Reflection::Float &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(EA::Reflection::Float, 0x8d5beb72, input, index);
}

inline bool Gen::ecar::FrontCamber(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xeed9ca5d);
}
inline const EA::Reflection::Float &Gen::ecar::FrontCamber() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xeed9ca5d);
}
inline bool Gen::ecar::FrontCamber(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xeed9ca5d, result);
}
inline bool Gen::ecar::SET_FrontCamber(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xeed9ca5d, input);
}

inline bool Gen::ecar::IsSkinned(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xd9102c65);
}
inline const EA::Reflection::Bool &Gen::ecar::IsSkinned() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xd9102c65);
}
inline bool Gen::ecar::IsSkinned(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xd9102c65, result);
}
inline bool Gen::ecar::SET_IsSkinned(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xd9102c65, input);
}

inline bool Gen::ecar::KitWheelOffsetFront(TAttrib<EA::Reflection::UInt8> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt8, 0xfd3077e8);
}
inline const EA::Reflection::UInt8 &Gen::ecar::KitWheelOffsetFront(unsigned int index) const {
    ATTRIB_CODEGEN_GETLAYOUTINDEXED(EA::Reflection::UInt8, KitWheelOffsetFront, index);
}
inline bool Gen::ecar::KitWheelOffsetFront(EA::Reflection::UInt8 &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(KitWheelOffsetFront, result, index);
}
inline unsigned int Gen::ecar::Num_KitWheelOffsetFront() const {
    ATTRIB_CODEGEN_GETLAYOUTLENGTH(KitWheelOffsetFront);
}
inline bool Gen::ecar::SET_KitWheelOffsetFront(const EA::Reflection::UInt8 &input, unsigned int index) {
    ATTRIB_CODEGEN_SETLAYOUTINDEXED(KitWheelOffsetFront, input, index);
}

inline bool Gen::ecar::KitWheelOffsetRear(TAttrib<EA::Reflection::UInt8> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt8, 0xccc484dc);
}
inline const EA::Reflection::UInt8 &Gen::ecar::KitWheelOffsetRear(unsigned int index) const {
    ATTRIB_CODEGEN_GETLAYOUTINDEXED(EA::Reflection::UInt8, KitWheelOffsetRear, index);
}
inline bool Gen::ecar::KitWheelOffsetRear(EA::Reflection::UInt8 &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(KitWheelOffsetRear, result, index);
}
inline unsigned int Gen::ecar::Num_KitWheelOffsetRear() const {
    ATTRIB_CODEGEN_GETLAYOUTLENGTH(KitWheelOffsetRear);
}
inline bool Gen::ecar::SET_KitWheelOffsetRear(const EA::Reflection::UInt8 &input, unsigned int index) {
    ATTRIB_CODEGEN_SETLAYOUTINDEXED(KitWheelOffsetRear, input, index);
}

inline bool Gen::ecar::LIGHT_COPBLUE(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x82094cc9);
}
inline const RefSpec &Gen::ecar::LIGHT_COPBLUE() const {
    ATTRIB_CODEGEN_GETVALUE(RefSpec, 0x82094cc9);
}
inline bool Gen::ecar::LIGHT_COPBLUE(RefSpec &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0x82094cc9, result);
}
inline bool Gen::ecar::SET_LIGHT_COPBLUE(const RefSpec &input) {
    ATTRIB_CODEGEN_SETVALUE(RefSpec, 0x82094cc9, input);
}

inline bool Gen::ecar::LIGHT_COPRED(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x16437af7);
}
inline const RefSpec &Gen::ecar::LIGHT_COPRED() const {
    ATTRIB_CODEGEN_GETVALUE(RefSpec, 0x16437af7);
}
inline bool Gen::ecar::LIGHT_COPRED(RefSpec &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0x16437af7, result);
}
inline bool Gen::ecar::SET_LIGHT_COPRED(const RefSpec &input) {
    ATTRIB_CODEGEN_SETVALUE(RefSpec, 0x16437af7, input);
}

inline bool Gen::ecar::LIGHT_COPWHITE(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x1ee00adf);
}
inline const RefSpec &Gen::ecar::LIGHT_COPWHITE() const {
    ATTRIB_CODEGEN_GETVALUE(RefSpec, 0x1ee00adf);
}
inline bool Gen::ecar::LIGHT_COPWHITE(RefSpec &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0x1ee00adf, result);
}
inline bool Gen::ecar::SET_LIGHT_COPWHITE(const RefSpec &input) {
    ATTRIB_CODEGEN_SETVALUE(RefSpec, 0x1ee00adf, input);
}

inline bool Gen::ecar::MaxTireSteer(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xa9633fde);
}
inline const EA::Reflection::Float &Gen::ecar::MaxTireSteer() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xa9633fde);
}
inline bool Gen::ecar::MaxTireSteer(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xa9633fde, result);
}
inline bool Gen::ecar::SET_MaxTireSteer(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xa9633fde, input);
}

inline bool Gen::ecar::MissShiftEffect(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0xb699b7be);
}
inline const RefSpec &Gen::ecar::MissShiftEffect() const {
    ATTRIB_CODEGEN_GETVALUE(RefSpec, 0xb699b7be);
}
inline bool Gen::ecar::MissShiftEffect(RefSpec &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0xb699b7be, result);
}
inline bool Gen::ecar::SET_MissShiftEffect(const RefSpec &input) {
    ATTRIB_CODEGEN_SETVALUE(RefSpec, 0xb699b7be, input);
}

inline bool Gen::ecar::NOSEffect(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x60cec115);
}
inline const RefSpec &Gen::ecar::NOSEffect() const {
    ATTRIB_CODEGEN_GETVALUE(RefSpec, 0x60cec115);
}
inline bool Gen::ecar::NOSEffect(RefSpec &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0x60cec115, result);
}
inline bool Gen::ecar::SET_NOSEffect(const RefSpec &input) {
    ATTRIB_CODEGEN_SETVALUE(RefSpec, 0x60cec115, input);
}

inline bool Gen::ecar::ReflectionOffset(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xae298718);
}
inline const EA::Reflection::Float &Gen::ecar::ReflectionOffset() const {
    ATTRIB_CODEGEN_GETLAYOUT(ReflectionOffset);
}
inline bool Gen::ecar::ReflectionOffset(EA::Reflection::Float &result) const {
    result = ReflectionOffset();
    return true;
}
inline bool Gen::ecar::SET_ReflectionOffset(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(ReflectionOffset, input);
}

inline bool Gen::ecar::RideHeight(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x3996ab3b);
}
inline const EA::Reflection::Float &Gen::ecar::RideHeight() const {
    ATTRIB_CODEGEN_GETLAYOUT(RideHeight);
}
inline bool Gen::ecar::RideHeight(EA::Reflection::Float &result) const {
    result = RideHeight();
    return true;
}
inline bool Gen::ecar::SET_RideHeight(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(RideHeight, input);
}

inline bool Gen::ecar::RoadNoise(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x13eb7da2);
}
inline const EA::Reflection::Float &Gen::ecar::RoadNoise() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x13eb7da2);
}
inline bool Gen::ecar::RoadNoise(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x13eb7da2, result);
}
inline bool Gen::ecar::SET_RoadNoise(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x13eb7da2, input);
}

inline bool Gen::ecar::ShiftAngle(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x24e25513);
}
inline const EA::Reflection::Float &Gen::ecar::ShiftAngle() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x24e25513);
}
inline bool Gen::ecar::ShiftAngle(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x24e25513, result);
}
inline bool Gen::ecar::SET_ShiftAngle(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x24e25513, input);
}

inline bool Gen::ecar::ShiftSpeed(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xda27cd35);
}
inline const EA::Reflection::Float &Gen::ecar::ShiftSpeed() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xda27cd35);
}
inline bool Gen::ecar::ShiftSpeed(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xda27cd35, result);
}
inline bool Gen::ecar::SET_ShiftSpeed(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xda27cd35, input);
}

inline bool Gen::ecar::SkidFX(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xadb52f00);
}
inline const EA::Reflection::Float &Gen::ecar::SkidFX(unsigned int index) const {
    ATTRIB_CODEGEN_GETLAYOUTINDEXED(EA::Reflection::Float, SkidFX, index);
}
inline bool Gen::ecar::SkidFX(EA::Reflection::Float &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(SkidFX, result, index);
}
inline unsigned int Gen::ecar::Num_SkidFX() const {
    ATTRIB_CODEGEN_GETLAYOUTLENGTH(SkidFX);
}
inline bool Gen::ecar::SET_SkidFX(const EA::Reflection::Float &input, unsigned int index) {
    ATTRIB_CODEGEN_SETLAYOUTINDEXED(SkidFX, input, index);
}

inline bool Gen::ecar::SlipFX(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x01ac1449);
}
inline const EA::Reflection::Float &Gen::ecar::SlipFX(unsigned int index) const {
    ATTRIB_CODEGEN_GETLAYOUTINDEXED(EA::Reflection::Float, SlipFX, index);
}
inline bool Gen::ecar::SlipFX(EA::Reflection::Float &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(SlipFX, result, index);
}
inline unsigned int Gen::ecar::Num_SlipFX() const {
    ATTRIB_CODEGEN_GETLAYOUTLENGTH(SlipFX);
}
inline bool Gen::ecar::SET_SlipFX(const EA::Reflection::Float &input, unsigned int index) {
    ATTRIB_CODEGEN_SETLAYOUTINDEXED(SlipFX, input, index);
}

inline bool Gen::ecar::SteerSpeed(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x79356463);
}
inline const EA::Reflection::Float &Gen::ecar::SteerSpeed() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x79356463);
}
inline bool Gen::ecar::SteerSpeed(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x79356463, result);
}
inline bool Gen::ecar::SET_SteerSpeed(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x79356463, input);
}

inline bool Gen::ecar::TireOffsets(TAttrib<UMath::Vector4> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x5e39d209);
}
inline const UMath::Vector4 &Gen::ecar::TireOffsets(unsigned int index) const {
    ATTRIB_CODEGEN_GETLAYOUTINDEXED(UMath::Vector4, TireOffsets, index);
}
inline bool Gen::ecar::TireOffsets(UMath::Vector4 &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(TireOffsets, result, index);
}
inline unsigned int Gen::ecar::Num_TireOffsets() const {
    ATTRIB_CODEGEN_GETLAYOUTLENGTH(TireOffsets);
}
inline bool Gen::ecar::SET_TireOffsets(const UMath::Vector4 &input, unsigned int index) {
    ATTRIB_CODEGEN_SETLAYOUTINDEXED(TireOffsets, input, index);
}

inline bool Gen::ecar::TireSkidWidth(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x8f952090);
}
inline const EA::Reflection::Float &Gen::ecar::TireSkidWidth(unsigned int index) const {
    ATTRIB_CODEGEN_GETLAYOUTINDEXED(EA::Reflection::Float, TireSkidWidth, index);
}
inline bool Gen::ecar::TireSkidWidth(EA::Reflection::Float &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(TireSkidWidth, result, index);
}
inline unsigned int Gen::ecar::Num_TireSkidWidth() const {
    ATTRIB_CODEGEN_GETLAYOUTLENGTH(TireSkidWidth);
}
inline bool Gen::ecar::SET_TireSkidWidth(const EA::Reflection::Float &input, unsigned int index) {
    ATTRIB_CODEGEN_SETLAYOUTINDEXED(TireSkidWidth, input, index);
}

inline bool Gen::ecar::TireSkidWidthKitScale(TAttrib<UMath::Vector2> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector2, 0x7ce83504);
}
inline const UMath::Vector2 &Gen::ecar::TireSkidWidthKitScale(unsigned int index) const {
    ATTRIB_CODEGEN_GETLAYOUTINDEXED(UMath::Vector2, TireSkidWidthKitScale, index);
}
inline bool Gen::ecar::TireSkidWidthKitScale(UMath::Vector2 &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(TireSkidWidthKitScale, result, index);
}
inline unsigned int Gen::ecar::Num_TireSkidWidthKitScale() const {
    ATTRIB_CODEGEN_GETLAYOUTLENGTH(TireSkidWidthKitScale);
}
inline bool Gen::ecar::SET_TireSkidWidthKitScale(const UMath::Vector2 &input, unsigned int index) {
    ATTRIB_CODEGEN_SETLAYOUTINDEXED(TireSkidWidthKitScale, input, index);
}

inline bool Gen::ecar::WheelHopScale(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x44e52948);
}
inline const EA::Reflection::Float &Gen::ecar::WheelHopScale() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x44e52948);
}
inline bool Gen::ecar::WheelHopScale(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x44e52948, result);
}
inline bool Gen::ecar::SET_WheelHopScale(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x44e52948, input);
}

inline bool Gen::ecar::WheelSpokeCount(TAttrib<EA::Reflection::Int8> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int8, 0x96a8896c);
}
inline const EA::Reflection::Int8 &Gen::ecar::WheelSpokeCount() const {
    ATTRIB_CODEGEN_GETLAYOUT(WheelSpokeCount);
}
inline bool Gen::ecar::WheelSpokeCount(EA::Reflection::Int8 &result) const {
    result = WheelSpokeCount();
    return true;
}
inline bool Gen::ecar::SET_WheelSpokeCount(const EA::Reflection::Int8 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(WheelSpokeCount, input);
}

inline bool Gen::ecar::WheelWell(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x24cb15ea);
}
inline const EA::Reflection::Float &Gen::ecar::WheelWell() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x24cb15ea);
}
inline bool Gen::ecar::WheelWell(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x24cb15ea, result);
}
inline bool Gen::ecar::SET_WheelWell(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x24cb15ea, input);
}

}; // namespace Attrib

#endif
