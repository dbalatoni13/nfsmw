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
    bool BodyDive(TAttrib<CarBodyMotion> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(CarBodyMotion, 0x08e34911);
    }
    bool BodyDive(CarBodyMotion &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(BodyDive, result);
    }
    const CarBodyMotion &BodyDive() const {
        ATTRIB_CODEGEN_GETLAYOUT(BodyDive);
    }
    bool SET_BodyDive(const CarBodyMotion &input) {
        ATTRIB_CODEGEN_SETLAYOUT(BodyDive, input);
    }
    bool BodyRoll(TAttrib<CarBodyMotion> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(CarBodyMotion, 0xf9d98a1b);
    }
    bool BodyRoll(CarBodyMotion &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(BodyRoll, result);
    }
    const CarBodyMotion &BodyRoll() const {
        ATTRIB_CODEGEN_GETLAYOUT(BodyRoll);
    }
    bool SET_BodyRoll(const CarBodyMotion &input) {
        ATTRIB_CODEGEN_SETLAYOUT(BodyRoll, input);
    }
    bool BodySquat(TAttrib<CarBodyMotion> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(CarBodyMotion, 0x73117117);
    }
    bool BodySquat(CarBodyMotion &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(BodySquat, result);
    }
    const CarBodyMotion &BodySquat() const {
        ATTRIB_CODEGEN_GETLAYOUT(BodySquat);
    }
    bool SET_BodySquat(const CarBodyMotion &input) {
        ATTRIB_CODEGEN_SETLAYOUT(BodySquat, input);
    }
    bool CamberFront(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x8c032c9f);
    }
    bool CamberFront(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(CamberFront, result);
    }
    const EA::Reflection::Float &CamberFront() const {
        ATTRIB_CODEGEN_GETLAYOUT(CamberFront);
    }
    bool SET_CamberFront(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(CamberFront, input);
    }
    bool CamberRear(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x87d39b4f);
    }
    bool CamberRear(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(CamberRear, result);
    }
    const EA::Reflection::Float &CamberRear() const {
        ATTRIB_CODEGEN_GETLAYOUT(CamberRear);
    }
    bool SET_CamberRear(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(CamberRear, input);
    }
    bool CameraInfo_Bumper(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x585517f3);
    }
    bool CameraInfo_Bumper(RefSpec &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0x585517f3, result);
    }
    const RefSpec &CameraInfo_Bumper() const {
        ATTRIB_CODEGEN_GETVALUE(RefSpec, 0x585517f3);
    }
    bool SET_CameraInfo_Bumper(const RefSpec &input) {
        ATTRIB_CODEGEN_SETVALUE(RefSpec, 0x585517f3, input);
    }
    bool CameraInfo_Close(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x0c2da793);
    }
    bool CameraInfo_Close(RefSpec &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0x0c2da793, result);
    }
    const RefSpec &CameraInfo_Close() const {
        ATTRIB_CODEGEN_GETVALUE(RefSpec, 0x0c2da793);
    }
    bool SET_CameraInfo_Close(const RefSpec &input) {
        ATTRIB_CODEGEN_SETVALUE(RefSpec, 0x0c2da793, input);
    }
    bool CameraInfo_Drift(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x4b675dc8);
    }
    bool CameraInfo_Drift(RefSpec &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0x4b675dc8, result);
    }
    const RefSpec &CameraInfo_Drift() const {
        ATTRIB_CODEGEN_GETVALUE(RefSpec, 0x4b675dc8);
    }
    bool SET_CameraInfo_Drift(const RefSpec &input) {
        ATTRIB_CODEGEN_SETVALUE(RefSpec, 0x4b675dc8, input);
    }
    bool CameraInfo_Far(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0xccf03cb3);
    }
    bool CameraInfo_Far(RefSpec &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0xccf03cb3, result);
    }
    const RefSpec &CameraInfo_Far() const {
        ATTRIB_CODEGEN_GETVALUE(RefSpec, 0xccf03cb3);
    }
    bool SET_CameraInfo_Far(const RefSpec &input) {
        ATTRIB_CODEGEN_SETVALUE(RefSpec, 0xccf03cb3, input);
    }
    bool CameraInfo_Hood(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0xd74c1435);
    }
    bool CameraInfo_Hood(RefSpec &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0xd74c1435, result);
    }
    const RefSpec &CameraInfo_Hood() const {
        ATTRIB_CODEGEN_GETVALUE(RefSpec, 0xd74c1435);
    }
    bool SET_CameraInfo_Hood(const RefSpec &input) {
        ATTRIB_CODEGEN_SETVALUE(RefSpec, 0xd74c1435, input);
    }
    bool CameraInfo_Pursuit(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0xd76a6fad);
    }
    bool CameraInfo_Pursuit(RefSpec &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0xd76a6fad, result);
    }
    const RefSpec &CameraInfo_Pursuit() const {
        ATTRIB_CODEGEN_GETVALUE(RefSpec, 0xd76a6fad);
    }
    bool SET_CameraInfo_Pursuit(const RefSpec &input) {
        ATTRIB_CODEGEN_SETVALUE(RefSpec, 0xd76a6fad, input);
    }
    bool CameraInfo_SuperFar(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x10204a90);
    }
    bool CameraInfo_SuperFar(RefSpec &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0x10204a90, result);
    }
    const RefSpec &CameraInfo_SuperFar() const {
        ATTRIB_CODEGEN_GETVALUE(RefSpec, 0x10204a90);
    }
    bool SET_CameraInfo_SuperFar(const RefSpec &input) {
        ATTRIB_CODEGEN_SETVALUE(RefSpec, 0x10204a90, input);
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
    bool DamageEffect(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x30b2997b);
    }
    bool DamageEffect(RefSpec &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0x30b2997b, result);
    }
    const RefSpec &DamageEffect() const {
        ATTRIB_CODEGEN_GETVALUE(RefSpec, 0x30b2997b);
    }
    bool SET_DamageEffect(const RefSpec &input) {
        ATTRIB_CODEGEN_SETVALUE(RefSpec, 0x30b2997b, input);
    }
    bool DeathEffect(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0xf7b59fc7);
    }
    bool DeathEffect(RefSpec &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0xf7b59fc7, result);
    }
    const RefSpec &DeathEffect() const {
        ATTRIB_CODEGEN_GETVALUE(RefSpec, 0xf7b59fc7);
    }
    bool SET_DeathEffect(const RefSpec &input) {
        ATTRIB_CODEGEN_SETVALUE(RefSpec, 0xf7b59fc7, input);
    }
    bool EngineBlownEffect(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0xd9cca9a3);
    }
    bool EngineBlownEffect(RefSpec &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0xd9cca9a3, result);
    }
    const RefSpec &EngineBlownEffect() const {
        ATTRIB_CODEGEN_GETVALUE(RefSpec, 0xd9cca9a3);
    }
    bool SET_EngineBlownEffect(const RefSpec &input) {
        ATTRIB_CODEGEN_SETVALUE(RefSpec, 0xd9cca9a3, input);
    }
    bool EngineRev(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x564773be);
    }
    bool EngineRev(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x564773be, result);
    }
    const EA::Reflection::Float &EngineRev() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x564773be);
    }
    bool SET_EngineRev(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x564773be, input);
    }
    bool EngineRevAngle(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xe2c63383);
    }
    bool EngineRevAngle(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xe2c63383, result);
    }
    const EA::Reflection::Float &EngineRevAngle() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xe2c63383);
    }
    bool SET_EngineRevAngle(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xe2c63383, input);
    }
    bool EngineRevSpeed(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xfaad3e61);
    }
    bool EngineRevSpeed(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xfaad3e61, result);
    }
    const EA::Reflection::Float &EngineRevSpeed() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xfaad3e61);
    }
    bool SET_EngineRevSpeed(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xfaad3e61, input);
    }
    bool EngineVibrationFreq(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x80e657ff);
    }
    bool EngineVibrationFreq(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x80e657ff, result);
    }
    const EA::Reflection::Float &EngineVibrationFreq() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x80e657ff);
    }
    bool SET_EngineVibrationFreq(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x80e657ff, input);
    }
    bool EngineVibrationMax(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xf947fe58);
    }
    bool EngineVibrationMax(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xf947fe58, result);
    }
    const EA::Reflection::Float &EngineVibrationMax() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xf947fe58);
    }
    bool SET_EngineVibrationMax(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xf947fe58, input);
    }
    bool EngineVibrationMin(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xe91f59bd);
    }
    bool EngineVibrationMin(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xe91f59bd, result);
    }
    const EA::Reflection::Float &EngineVibrationMin() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xe91f59bd);
    }
    bool SET_EngineVibrationMin(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xe91f59bd, input);
    }
    bool ExtraPitch(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xe95257c2);
    }
    bool ExtraPitch(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xe95257c2, result);
    }
    const EA::Reflection::Float &ExtraPitch() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xe95257c2);
    }
    bool SET_ExtraPitch(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xe95257c2, input);
    }
    bool ExtraRearTireOffset(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xb1304fde);
    }
    bool ExtraRearTireOffset(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xb1304fde, result);
    }
    const EA::Reflection::Float &ExtraRearTireOffset() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xb1304fde);
    }
    bool SET_ExtraRearTireOffset(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xb1304fde, input);
    }
    bool FECompressions(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x8d5beb72);
    }
    bool FECompressions(EA::Reflection::Float &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(EA::Reflection::Float, 0x8d5beb72, result, index);
    }
    const EA::Reflection::Float &FECompressions(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(EA::Reflection::Float, 0x8d5beb72, index);
    }
    unsigned int Num_FECompressions() const {
        ATTRIB_CODEGEN_GETLENGTH(0x8d5beb72);
    }
    bool SET_FECompressions(const EA::Reflection::Float &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(EA::Reflection::Float, 0x8d5beb72, input, index);
    }
    bool FrontCamber(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xeed9ca5d);
    }
    bool FrontCamber(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xeed9ca5d, result);
    }
    const EA::Reflection::Float &FrontCamber() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xeed9ca5d);
    }
    bool SET_FrontCamber(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xeed9ca5d, input);
    }
    bool IsSkinned(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xd9102c65);
    }
    bool IsSkinned(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xd9102c65, result);
    }
    const EA::Reflection::Bool &IsSkinned() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xd9102c65);
    }
    bool SET_IsSkinned(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xd9102c65, input);
    }
    bool KitWheelOffsetFront(TAttrib<EA::Reflection::UInt8> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt8, 0xfd3077e8);
    }
    bool KitWheelOffsetFront(EA::Reflection::UInt8 &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(KitWheelOffsetFront, result, index);
    }
    const EA::Reflection::UInt8 &KitWheelOffsetFront(unsigned int index) const {
        ATTRIB_CODEGEN_GETLAYOUTINDEXED(EA::Reflection::UInt8, KitWheelOffsetFront, index);
    }
    unsigned int Num_KitWheelOffsetFront() const {
        ATTRIB_CODEGEN_GETLAYOUTLENGTH(KitWheelOffsetFront);
    }
    bool SET_KitWheelOffsetFront(const EA::Reflection::UInt8 &input, unsigned int index) {
        ATTRIB_CODEGEN_SETLAYOUTINDEXED(KitWheelOffsetFront, input, index);
    }
    bool KitWheelOffsetRear(TAttrib<EA::Reflection::UInt8> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt8, 0xccc484dc);
    }
    bool KitWheelOffsetRear(EA::Reflection::UInt8 &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(KitWheelOffsetRear, result, index);
    }
    const EA::Reflection::UInt8 &KitWheelOffsetRear(unsigned int index) const {
        ATTRIB_CODEGEN_GETLAYOUTINDEXED(EA::Reflection::UInt8, KitWheelOffsetRear, index);
    }
    unsigned int Num_KitWheelOffsetRear() const {
        ATTRIB_CODEGEN_GETLAYOUTLENGTH(KitWheelOffsetRear);
    }
    bool SET_KitWheelOffsetRear(const EA::Reflection::UInt8 &input, unsigned int index) {
        ATTRIB_CODEGEN_SETLAYOUTINDEXED(KitWheelOffsetRear, input, index);
    }
    bool LIGHT_COPBLUE(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x82094cc9);
    }
    bool LIGHT_COPBLUE(RefSpec &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0x82094cc9, result);
    }
    const RefSpec &LIGHT_COPBLUE() const {
        ATTRIB_CODEGEN_GETVALUE(RefSpec, 0x82094cc9);
    }
    bool SET_LIGHT_COPBLUE(const RefSpec &input) {
        ATTRIB_CODEGEN_SETVALUE(RefSpec, 0x82094cc9, input);
    }
    bool LIGHT_COPRED(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x16437af7);
    }
    bool LIGHT_COPRED(RefSpec &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0x16437af7, result);
    }
    const RefSpec &LIGHT_COPRED() const {
        ATTRIB_CODEGEN_GETVALUE(RefSpec, 0x16437af7);
    }
    bool SET_LIGHT_COPRED(const RefSpec &input) {
        ATTRIB_CODEGEN_SETVALUE(RefSpec, 0x16437af7, input);
    }
    bool LIGHT_COPWHITE(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x1ee00adf);
    }
    bool LIGHT_COPWHITE(RefSpec &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0x1ee00adf, result);
    }
    const RefSpec &LIGHT_COPWHITE() const {
        ATTRIB_CODEGEN_GETVALUE(RefSpec, 0x1ee00adf);
    }
    bool SET_LIGHT_COPWHITE(const RefSpec &input) {
        ATTRIB_CODEGEN_SETVALUE(RefSpec, 0x1ee00adf, input);
    }
    bool MaxTireSteer(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xa9633fde);
    }
    bool MaxTireSteer(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xa9633fde, result);
    }
    const EA::Reflection::Float &MaxTireSteer() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xa9633fde);
    }
    bool SET_MaxTireSteer(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xa9633fde, input);
    }
    bool MissShiftEffect(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0xb699b7be);
    }
    bool MissShiftEffect(RefSpec &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0xb699b7be, result);
    }
    const RefSpec &MissShiftEffect() const {
        ATTRIB_CODEGEN_GETVALUE(RefSpec, 0xb699b7be);
    }
    bool SET_MissShiftEffect(const RefSpec &input) {
        ATTRIB_CODEGEN_SETVALUE(RefSpec, 0xb699b7be, input);
    }
    bool NOSEffect(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x60cec115);
    }
    bool NOSEffect(RefSpec &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(RefSpec, 0x60cec115, result);
    }
    const RefSpec &NOSEffect() const {
        ATTRIB_CODEGEN_GETVALUE(RefSpec, 0x60cec115);
    }
    bool SET_NOSEffect(const RefSpec &input) {
        ATTRIB_CODEGEN_SETVALUE(RefSpec, 0x60cec115, input);
    }
    bool ReflectionOffset(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xae298718);
    }
    bool ReflectionOffset(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(ReflectionOffset, result);
    }
    const EA::Reflection::Float &ReflectionOffset() const {
        ATTRIB_CODEGEN_GETLAYOUT(ReflectionOffset);
    }
    bool SET_ReflectionOffset(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(ReflectionOffset, input);
    }
    bool RideHeight(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x3996ab3b);
    }
    bool RideHeight(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(RideHeight, result);
    }
    const EA::Reflection::Float &RideHeight() const {
        ATTRIB_CODEGEN_GETLAYOUT(RideHeight);
    }
    bool SET_RideHeight(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(RideHeight, input);
    }
    bool RoadNoise(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x13eb7da2);
    }
    bool RoadNoise(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x13eb7da2, result);
    }
    const EA::Reflection::Float &RoadNoise() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x13eb7da2);
    }
    bool SET_RoadNoise(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x13eb7da2, input);
    }
    bool ShiftAngle(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x24e25513);
    }
    bool ShiftAngle(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x24e25513, result);
    }
    const EA::Reflection::Float &ShiftAngle() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x24e25513);
    }
    bool SET_ShiftAngle(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x24e25513, input);
    }
    bool ShiftSpeed(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xda27cd35);
    }
    bool ShiftSpeed(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xda27cd35, result);
    }
    const EA::Reflection::Float &ShiftSpeed() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xda27cd35);
    }
    bool SET_ShiftSpeed(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xda27cd35, input);
    }
    bool SkidFX(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xadb52f00);
    }
    bool SkidFX(EA::Reflection::Float &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(SkidFX, result, index);
    }
    const EA::Reflection::Float &SkidFX(unsigned int index) const {
        ATTRIB_CODEGEN_GETLAYOUTINDEXED(EA::Reflection::Float, SkidFX, index);
    }
    unsigned int Num_SkidFX() const {
        ATTRIB_CODEGEN_GETLAYOUTLENGTH(SkidFX);
    }
    bool SET_SkidFX(const EA::Reflection::Float &input, unsigned int index) {
        ATTRIB_CODEGEN_SETLAYOUTINDEXED(SkidFX, input, index);
    }
    bool SlipFX(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x01ac1449);
    }
    bool SlipFX(EA::Reflection::Float &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(SlipFX, result, index);
    }
    const EA::Reflection::Float &SlipFX(unsigned int index) const {
        ATTRIB_CODEGEN_GETLAYOUTINDEXED(EA::Reflection::Float, SlipFX, index);
    }
    unsigned int Num_SlipFX() const {
        ATTRIB_CODEGEN_GETLAYOUTLENGTH(SlipFX);
    }
    bool SET_SlipFX(const EA::Reflection::Float &input, unsigned int index) {
        ATTRIB_CODEGEN_SETLAYOUTINDEXED(SlipFX, input, index);
    }
    bool SteerSpeed(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x79356463);
    }
    bool SteerSpeed(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x79356463, result);
    }
    const EA::Reflection::Float &SteerSpeed() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x79356463);
    }
    bool SET_SteerSpeed(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x79356463, input);
    }
    bool TireOffsets(TAttrib<UMath::Vector4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x5e39d209);
    }
    bool TireOffsets(UMath::Vector4 &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(TireOffsets, result, index);
    }
    const UMath::Vector4 &TireOffsets(unsigned int index) const {
        ATTRIB_CODEGEN_GETLAYOUTINDEXED(UMath::Vector4, TireOffsets, index);
    }
    unsigned int Num_TireOffsets() const {
        ATTRIB_CODEGEN_GETLAYOUTLENGTH(TireOffsets);
    }
    bool SET_TireOffsets(const UMath::Vector4 &input, unsigned int index) {
        ATTRIB_CODEGEN_SETLAYOUTINDEXED(TireOffsets, input, index);
    }
    bool TireSkidWidth(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x8f952090);
    }
    bool TireSkidWidth(EA::Reflection::Float &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(TireSkidWidth, result, index);
    }
    const EA::Reflection::Float &TireSkidWidth(unsigned int index) const {
        ATTRIB_CODEGEN_GETLAYOUTINDEXED(EA::Reflection::Float, TireSkidWidth, index);
    }
    unsigned int Num_TireSkidWidth() const {
        ATTRIB_CODEGEN_GETLAYOUTLENGTH(TireSkidWidth);
    }
    bool SET_TireSkidWidth(const EA::Reflection::Float &input, unsigned int index) {
        ATTRIB_CODEGEN_SETLAYOUTINDEXED(TireSkidWidth, input, index);
    }
    bool TireSkidWidthKitScale(TAttrib<UMath::Vector2> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Vector2, 0x7ce83504);
    }
    bool TireSkidWidthKitScale(UMath::Vector2 &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(TireSkidWidthKitScale, result, index);
    }
    const UMath::Vector2 &TireSkidWidthKitScale(unsigned int index) const {
        ATTRIB_CODEGEN_GETLAYOUTINDEXED(UMath::Vector2, TireSkidWidthKitScale, index);
    }
    unsigned int Num_TireSkidWidthKitScale() const {
        ATTRIB_CODEGEN_GETLAYOUTLENGTH(TireSkidWidthKitScale);
    }
    bool SET_TireSkidWidthKitScale(const UMath::Vector2 &input, unsigned int index) {
        ATTRIB_CODEGEN_SETLAYOUTINDEXED(TireSkidWidthKitScale, input, index);
    }
    bool WheelHopScale(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x44e52948);
    }
    bool WheelHopScale(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x44e52948, result);
    }
    const EA::Reflection::Float &WheelHopScale() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x44e52948);
    }
    bool SET_WheelHopScale(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x44e52948, input);
    }
    bool WheelSpokeCount(TAttrib<EA::Reflection::Int8> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int8, 0x96a8896c);
    }
    bool WheelSpokeCount(EA::Reflection::Int8 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(WheelSpokeCount, result);
    }
    const EA::Reflection::Int8 &WheelSpokeCount() const {
        ATTRIB_CODEGEN_GETLAYOUT(WheelSpokeCount);
    }
    bool SET_WheelSpokeCount(const EA::Reflection::Int8 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(WheelSpokeCount, input);
    }
    bool WheelWell(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x24cb15ea);
    }
    bool WheelWell(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x24cb15ea, result);
    }
    const EA::Reflection::Float &WheelWell() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x24cb15ea);
    }
    bool SET_WheelWell(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x24cb15ea, input);
    }

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

}; // namespace Attrib

#endif
