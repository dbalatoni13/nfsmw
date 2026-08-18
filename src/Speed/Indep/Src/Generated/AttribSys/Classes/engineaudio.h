#ifndef _attrib_gen_engineaudio_h
#define _attrib_gen_engineaudio_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct engineaudio : Instance {
    struct _LayoutStruct {
        Attrib::StringKey Filename_GinsuDecel;        // offset 0x0, size 0x10
        Attrib::StringKey BankName_mainRAM;           // offset 0x10, size 0x10
        Attrib::StringKey Filename_GinsuAccel;        // offset 0x20, size 0x10
        RefSpec acceltrans;                           // offset 0x30, size 0xc
        EA::Reflection::Text CollectionName;          // offset 0x3c, size 0x4
        eENGINE_GROUP EngType;                        // offset 0x40, size 0x4
        EA::Reflection::UInt32 DECEL_AEMSVol;         // offset 0x44, size 0x4
        EA::Reflection::Float DECEL_GINSUMix_S_RPM;   // offset 0x48, size 0x4
        EA::Reflection::UInt32 GINSU_Decel_MaxRPM;    // offset 0x4c, size 0x4
        EA::Reflection::Float GINSU_DECEL_FADE_IN;    // offset 0x50, size 0x4
        EA::Reflection::Float Priority;               // offset 0x54, size 0x4
        EA::Reflection::Float GINSU_DECEL_FADE_OUT;   // offset 0x58, size 0x4
        EA::Reflection::UInt32 GinsuDecelVol;         // offset 0x5c, size 0x4
        EA::Reflection::UInt32 GINSU_Decel_MinRPM;    // offset 0x60, size 0x4
        EA::Reflection::Float DecelDeltaRPMThreshold; // offset 0x64, size 0x4
        EA::Reflection::Float AEMSMix_S_RPM;          // offset 0x68, size 0x4
        EA::Reflection::Float Ginsu_ACL_Neg_L_RPM;    // offset 0x6c, size 0x4
        EA::Reflection::Float AEMSMix_L_RPM;          // offset 0x70, size 0x4
        EA::Reflection::UInt32 GINSUAccelVol;         // offset 0x74, size 0x4
        EA::Reflection::Float AccelDeltaRPMThreshold; // offset 0x78, size 0x4
        EA::Reflection::Float GINSUMix_S_RPM;         // offset 0x7c, size 0x4
        EA::Reflection::Float DECEL_GINSUMix_L_RPM;   // offset 0x80, size 0x4
        EA::Reflection::UInt32 AEMSVol;               // offset 0x84, size 0x4
        EA::Reflection::Float DECEL_AEMSMix_L_RPM;    // offset 0x88, size 0x4
        EA::Reflection::Float DECEL_AEMSMix_S_RPM;    // offset 0x8c, size 0x4
        EA::Reflection::Float GINSUMix_L_RPM;         // offset 0x90, size 0x4
        EA::Reflection::UInt32 CarID;                 // offset 0x94, size 0x4
        EA::Reflection::Float MinRPM;                 // offset 0x98, size 0x4
        EA::Reflection::Float MaxRPM;                 // offset 0x9c, size 0x4
        EA::Reflection::UInt16 Master_Vol;            // offset 0xa0, size 0x2
        EA::Reflection::Bool MaybeV8;                 // offset 0xa2, size 0x1
        EA::Reflection::Bool Tranny;                  // offset 0xa3, size 0x1
    };

    typedef EA::Reflection::Float TypeOf_AEMSMix_L_RPM;
    typedef EA::Reflection::Float TypeOf_AEMSMix_S_RPM;
    typedef EA::Reflection::UInt32 TypeOf_AEMSVol;
    typedef EA::Reflection::Float TypeOf_AccelDeltaRPMThreshold;
    typedef Attrib::StringKey TypeOf_BankName_auxRAM;
    typedef Attrib::StringKey TypeOf_BankName_mainRAM;
    typedef EA::Reflection::UInt32 TypeOf_CarID;
    typedef EA::Reflection::Text TypeOf_CollectionName;
    typedef EA::Reflection::Float TypeOf_DECEL_AEMSMix_L_RPM;
    typedef EA::Reflection::Float TypeOf_DECEL_AEMSMix_S_RPM;
    typedef EA::Reflection::UInt32 TypeOf_DECEL_AEMSVol;
    typedef EA::Reflection::Float TypeOf_DECEL_GINSUMix_L_RPM;
    typedef EA::Reflection::Float TypeOf_DECEL_GINSUMix_S_RPM;
    typedef EA::Reflection::Float TypeOf_DecelDeltaRPMThreshold;
    typedef EA::Reflection::Float TypeOf_DecelPitchOffset;
    typedef eENGINE_GROUP TypeOf_EngType;
    typedef Attrib::StringKey TypeOf_Filename_GinsuAccel;
    typedef Attrib::StringKey TypeOf_Filename_GinsuDecel;
    typedef EA::Reflection::UInt32 TypeOf_GINSUAccelVol;
    typedef EA::Reflection::Float TypeOf_GINSUMix_L_RPM;
    typedef EA::Reflection::Float TypeOf_GINSUMix_S_RPM;
    typedef EA::Reflection::Float TypeOf_GINSU_DECEL_FADE_IN;
    typedef EA::Reflection::Float TypeOf_GINSU_DECEL_FADE_OUT;
    typedef EA::Reflection::UInt32 TypeOf_GINSU_Decel_MaxRPM;
    typedef EA::Reflection::UInt32 TypeOf_GINSU_Decel_MinRPM;
    typedef EA::Reflection::UInt32 TypeOf_GINSU_LowPassCutoff;
    typedef EA::Reflection::UInt32 TypeOf_GinsuDecelVol;
    typedef EA::Reflection::Float TypeOf_Ginsu_ACL_Neg_L_RPM;
    typedef EA::Reflection::Float TypeOf_Ginsu_ACL_Neg_S_RPM;
    typedef EA::Reflection::UInt16 TypeOf_Master_Vol;
    typedef EA::Reflection::Float TypeOf_MaxRPM;
    typedef EA::Reflection::Bool TypeOf_MaybeV8;
    typedef EA::Reflection::Float TypeOf_MinRPM;
    typedef UMath::Matrix4 TypeOf_PhysicsRPM_Map;
    typedef EA::Reflection::Float TypeOf_Priority;
    typedef Attrib::StringKey TypeOf_SweetBank;
    typedef EA::Reflection::Bool TypeOf_Tranny;
    typedef EA::Reflection::Int32 TypeOf_Vol_ShiftSweets;
    typedef EA::Reflection::Int32 TypeOf_Vol_Sputters;
    typedef RefSpec TypeOf_acceltrans;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("engineaudio");
    engineaudio(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    engineaudio(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    engineaudio(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    engineaudio(const engineaudio &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    engineaudio(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~engineaudio() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0x50eab0e6;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0x50eab0e6, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const engineaudio &operator=(const engineaudio &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const engineaudio &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool AEMSMix_L_RPM(TAttrib<EA::Reflection::Float> &result) const;
    bool AEMSMix_L_RPM(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &AEMSMix_L_RPM() const;
    bool SET_AEMSMix_L_RPM(const EA::Reflection::Float &input);

    bool AEMSMix_S_RPM(TAttrib<EA::Reflection::Float> &result) const;
    bool AEMSMix_S_RPM(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &AEMSMix_S_RPM() const;
    bool SET_AEMSMix_S_RPM(const EA::Reflection::Float &input);

    bool AEMSVol(TAttrib<EA::Reflection::UInt32> &result) const;
    bool AEMSVol(EA::Reflection::UInt32 &result) const;
    const EA::Reflection::UInt32 &AEMSVol() const;
    bool SET_AEMSVol(const EA::Reflection::UInt32 &input);

    bool AccelDeltaRPMThreshold(TAttrib<EA::Reflection::Float> &result) const;
    bool AccelDeltaRPMThreshold(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &AccelDeltaRPMThreshold() const;
    bool SET_AccelDeltaRPMThreshold(const EA::Reflection::Float &input);

    bool BankName_auxRAM(TAttrib<Attrib::StringKey> &result) const;
    bool BankName_auxRAM(Attrib::StringKey &result, unsigned int index) const;
    const Attrib::StringKey &BankName_auxRAM(unsigned int index) const;
    unsigned int Num_BankName_auxRAM() const;
    bool SET_BankName_auxRAM(const Attrib::StringKey &input, unsigned int index);

    bool BankName_mainRAM(TAttrib<Attrib::StringKey> &result) const;
    bool BankName_mainRAM(Attrib::StringKey &result) const;
    const Attrib::StringKey &BankName_mainRAM() const;
    bool SET_BankName_mainRAM(const Attrib::StringKey &input);

    bool CarID(TAttrib<EA::Reflection::UInt32> &result) const;
    bool CarID(EA::Reflection::UInt32 &result) const;
    const EA::Reflection::UInt32 &CarID() const;
    bool SET_CarID(const EA::Reflection::UInt32 &input);

    bool CollectionName(TAttrib<EA::Reflection::Text> &result) const;
    bool CollectionName(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &CollectionName() const;
    bool SET_CollectionName(const EA::Reflection::Text &input);

    bool DECEL_AEMSMix_L_RPM(TAttrib<EA::Reflection::Float> &result) const;
    bool DECEL_AEMSMix_L_RPM(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &DECEL_AEMSMix_L_RPM() const;
    bool SET_DECEL_AEMSMix_L_RPM(const EA::Reflection::Float &input);

    bool DECEL_AEMSMix_S_RPM(TAttrib<EA::Reflection::Float> &result) const;
    bool DECEL_AEMSMix_S_RPM(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &DECEL_AEMSMix_S_RPM() const;
    bool SET_DECEL_AEMSMix_S_RPM(const EA::Reflection::Float &input);

    bool DECEL_AEMSVol(TAttrib<EA::Reflection::UInt32> &result) const;
    bool DECEL_AEMSVol(EA::Reflection::UInt32 &result) const;
    const EA::Reflection::UInt32 &DECEL_AEMSVol() const;
    bool SET_DECEL_AEMSVol(const EA::Reflection::UInt32 &input);

    bool DECEL_GINSUMix_L_RPM(TAttrib<EA::Reflection::Float> &result) const;
    bool DECEL_GINSUMix_L_RPM(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &DECEL_GINSUMix_L_RPM() const;
    bool SET_DECEL_GINSUMix_L_RPM(const EA::Reflection::Float &input);

    bool DECEL_GINSUMix_S_RPM(TAttrib<EA::Reflection::Float> &result) const;
    bool DECEL_GINSUMix_S_RPM(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &DECEL_GINSUMix_S_RPM() const;
    bool SET_DECEL_GINSUMix_S_RPM(const EA::Reflection::Float &input);

    bool DecelDeltaRPMThreshold(TAttrib<EA::Reflection::Float> &result) const;
    bool DecelDeltaRPMThreshold(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &DecelDeltaRPMThreshold() const;
    bool SET_DecelDeltaRPMThreshold(const EA::Reflection::Float &input);

    bool DecelPitchOffset(TAttrib<EA::Reflection::Float> &result) const;
    bool DecelPitchOffset(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &DecelPitchOffset() const;
    bool SET_DecelPitchOffset(const EA::Reflection::Float &input);

    bool EngType(TAttrib<eENGINE_GROUP> &result) const;
    bool EngType(eENGINE_GROUP &result) const;
    const eENGINE_GROUP &EngType() const;
    bool SET_EngType(const eENGINE_GROUP &input);

    bool Filename_GinsuAccel(TAttrib<Attrib::StringKey> &result) const;
    bool Filename_GinsuAccel(Attrib::StringKey &result) const;
    const Attrib::StringKey &Filename_GinsuAccel() const;
    bool SET_Filename_GinsuAccel(const Attrib::StringKey &input);

    bool Filename_GinsuDecel(TAttrib<Attrib::StringKey> &result) const;
    bool Filename_GinsuDecel(Attrib::StringKey &result) const;
    const Attrib::StringKey &Filename_GinsuDecel() const;
    bool SET_Filename_GinsuDecel(const Attrib::StringKey &input);

    bool GINSUAccelVol(TAttrib<EA::Reflection::UInt32> &result) const;
    bool GINSUAccelVol(EA::Reflection::UInt32 &result) const;
    const EA::Reflection::UInt32 &GINSUAccelVol() const;
    bool SET_GINSUAccelVol(const EA::Reflection::UInt32 &input);

    bool GINSUMix_L_RPM(TAttrib<EA::Reflection::Float> &result) const;
    bool GINSUMix_L_RPM(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &GINSUMix_L_RPM() const;
    bool SET_GINSUMix_L_RPM(const EA::Reflection::Float &input);

    bool GINSUMix_S_RPM(TAttrib<EA::Reflection::Float> &result) const;
    bool GINSUMix_S_RPM(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &GINSUMix_S_RPM() const;
    bool SET_GINSUMix_S_RPM(const EA::Reflection::Float &input);

    bool GINSU_DECEL_FADE_IN(TAttrib<EA::Reflection::Float> &result) const;
    bool GINSU_DECEL_FADE_IN(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &GINSU_DECEL_FADE_IN() const;
    bool SET_GINSU_DECEL_FADE_IN(const EA::Reflection::Float &input);

    bool GINSU_DECEL_FADE_OUT(TAttrib<EA::Reflection::Float> &result) const;
    bool GINSU_DECEL_FADE_OUT(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &GINSU_DECEL_FADE_OUT() const;
    bool SET_GINSU_DECEL_FADE_OUT(const EA::Reflection::Float &input);

    bool GINSU_Decel_MaxRPM(TAttrib<EA::Reflection::UInt32> &result) const;
    bool GINSU_Decel_MaxRPM(EA::Reflection::UInt32 &result) const;
    const EA::Reflection::UInt32 &GINSU_Decel_MaxRPM() const;
    bool SET_GINSU_Decel_MaxRPM(const EA::Reflection::UInt32 &input);

    bool GINSU_Decel_MinRPM(TAttrib<EA::Reflection::UInt32> &result) const;
    bool GINSU_Decel_MinRPM(EA::Reflection::UInt32 &result) const;
    const EA::Reflection::UInt32 &GINSU_Decel_MinRPM() const;
    bool SET_GINSU_Decel_MinRPM(const EA::Reflection::UInt32 &input);

    bool GINSU_LowPassCutoff(TAttrib<EA::Reflection::UInt32> &result) const;
    bool GINSU_LowPassCutoff(EA::Reflection::UInt32 &result) const;
    const EA::Reflection::UInt32 &GINSU_LowPassCutoff() const;
    bool SET_GINSU_LowPassCutoff(const EA::Reflection::UInt32 &input);

    bool GinsuDecelVol(TAttrib<EA::Reflection::UInt32> &result) const;
    bool GinsuDecelVol(EA::Reflection::UInt32 &result) const;
    const EA::Reflection::UInt32 &GinsuDecelVol() const;
    bool SET_GinsuDecelVol(const EA::Reflection::UInt32 &input);

    bool Ginsu_ACL_Neg_L_RPM(TAttrib<EA::Reflection::Float> &result) const;
    bool Ginsu_ACL_Neg_L_RPM(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &Ginsu_ACL_Neg_L_RPM() const;
    bool SET_Ginsu_ACL_Neg_L_RPM(const EA::Reflection::Float &input);

    bool Ginsu_ACL_Neg_S_RPM(TAttrib<EA::Reflection::Float> &result) const;
    bool Ginsu_ACL_Neg_S_RPM(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &Ginsu_ACL_Neg_S_RPM() const;
    bool SET_Ginsu_ACL_Neg_S_RPM(const EA::Reflection::Float &input);

    bool Master_Vol(TAttrib<EA::Reflection::UInt16> &result) const;
    bool Master_Vol(EA::Reflection::UInt16 &result) const;
    const EA::Reflection::UInt16 &Master_Vol() const;
    bool SET_Master_Vol(const EA::Reflection::UInt16 &input);

    bool MaxRPM(TAttrib<EA::Reflection::Float> &result) const;
    bool MaxRPM(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &MaxRPM() const;
    bool SET_MaxRPM(const EA::Reflection::Float &input);

    bool MaybeV8(TAttrib<EA::Reflection::Bool> &result) const;
    bool MaybeV8(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &MaybeV8() const;
    bool SET_MaybeV8(const EA::Reflection::Bool &input);

    bool MinRPM(TAttrib<EA::Reflection::Float> &result) const;
    bool MinRPM(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &MinRPM() const;
    bool SET_MinRPM(const EA::Reflection::Float &input);

    bool PhysicsRPM_Map(TAttrib<UMath::Matrix4> &result) const;
    bool PhysicsRPM_Map(UMath::Matrix4 &result) const;
    const UMath::Matrix4 &PhysicsRPM_Map() const;
    bool SET_PhysicsRPM_Map(const UMath::Matrix4 &input);

    bool Priority(TAttrib<EA::Reflection::Float> &result) const;
    bool Priority(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &Priority() const;
    bool SET_Priority(const EA::Reflection::Float &input);

    bool SweetBank(TAttrib<Attrib::StringKey> &result) const;
    bool SweetBank(Attrib::StringKey &result, unsigned int index) const;
    const Attrib::StringKey &SweetBank(unsigned int index) const;
    unsigned int Num_SweetBank() const;
    bool SET_SweetBank(const Attrib::StringKey &input, unsigned int index);

    bool Tranny(TAttrib<EA::Reflection::Bool> &result) const;
    bool Tranny(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &Tranny() const;
    bool SET_Tranny(const EA::Reflection::Bool &input);

    bool Vol_ShiftSweets(TAttrib<EA::Reflection::Int32> &result) const;
    bool Vol_ShiftSweets(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &Vol_ShiftSweets() const;
    bool SET_Vol_ShiftSweets(const EA::Reflection::Int32 &input);

    bool Vol_Sputters(TAttrib<EA::Reflection::Int32> &result) const;
    bool Vol_Sputters(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &Vol_Sputters() const;
    bool SET_Vol_Sputters(const EA::Reflection::Int32 &input);

    bool acceltrans(TAttrib<RefSpec> &result) const;
    bool acceltrans(RefSpec &result) const;
    const RefSpec &acceltrans() const;
    bool SET_acceltrans(const RefSpec &input);

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    engineaudio &ConvertFromInstance(Instance &src) {}
    const engineaudio &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key engineaudio = 0x50eab0e6;

}; // namespace ClassName

namespace Hash {
namespace engineaudio {

static const Key AEMSMix_L_RPM = 0x014d605f;
static const Key AEMSMix_S_RPM = 0xa4a699b0;
static const Key AEMSVol = 0x974eb50a;
static const Key AccelDeltaRPMThreshold = 0x3b82c385;
static const Key BankName_auxRAM = 0x04935eab;
static const Key BankName_mainRAM = 0x2fc9c96f;
static const Key CarID = 0x0c35b607;
static const Key CollectionName = 0x9ca1c8f9;
static const Key DECEL_AEMSMix_L_RPM = 0x8c22df44;
static const Key DECEL_AEMSMix_S_RPM = 0x3c9445e9;
static const Key DECEL_AEMSVol = 0x0f7c0093;
static const Key DECEL_GINSUMix_L_RPM = 0x4fe476f3;
static const Key DECEL_GINSUMix_S_RPM = 0x791768a0;
static const Key DecelDeltaRPMThreshold = 0x6114d1a5;
static const Key DecelPitchOffset = 0x313385dc;
static const Key EngType = 0x1d64eafb;
static const Key Filename_GinsuAccel = 0x2cee0a54;
static const Key Filename_GinsuDecel = 0x3a7c2f3d;
static const Key GINSUAccelVol = 0xc3f787b1;
static const Key GINSUMix_L_RPM = 0xd996fd76;
static const Key GINSUMix_S_RPM = 0xc2f38240;
static const Key GINSU_DECEL_FADE_IN = 0x56ba0258;
static const Key GINSU_DECEL_FADE_OUT = 0x18b901f4;
static const Key GINSU_Decel_MaxRPM = 0x414946cc;
static const Key GINSU_Decel_MinRPM = 0xd1b4e1b2;
static const Key GINSU_LowPassCutoff = 0xe3836473;
static const Key GinsuDecelVol = 0x38be973d;
static const Key Ginsu_ACL_Neg_L_RPM = 0x782f433d;
static const Key Ginsu_ACL_Neg_S_RPM = 0x38afe02e;
static const Key Master_Vol = 0xecc59244;
static const Key MaxRPM = 0xa4d878ef;
static const Key MaybeV8 = 0x6659143b;
static const Key MinRPM = 0x3ac8b868;
static const Key PhysicsRPM_Map = 0x07e3c833;
static const Key Priority = 0xa9598843;
static const Key SweetBank = 0xee501c6a;
static const Key Tranny = 0x83066633;
static const Key Vol_ShiftSweets = 0x34beca33;
static const Key Vol_Sputters = 0xc1eddd78;
static const Key acceltrans = 0xff77f451;

}; // namespace engineaudio
}; // namespace Hash

inline Key Gen::engineaudio::ClassKey() {
    return ClassName::engineaudio;
}

inline bool Gen::engineaudio::AEMSMix_L_RPM(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x014d605f);
}
inline const EA::Reflection::Float &Gen::engineaudio::AEMSMix_L_RPM() const {
    ATTRIB_CODEGEN_GETLAYOUT(AEMSMix_L_RPM);
}
inline bool Gen::engineaudio::AEMSMix_L_RPM(EA::Reflection::Float &result) const {
    result = AEMSMix_L_RPM();
    return true;
}
inline bool Gen::engineaudio::SET_AEMSMix_L_RPM(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(AEMSMix_L_RPM, input);
}

inline bool Gen::engineaudio::AEMSMix_S_RPM(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xa4a699b0);
}
inline const EA::Reflection::Float &Gen::engineaudio::AEMSMix_S_RPM() const {
    ATTRIB_CODEGEN_GETLAYOUT(AEMSMix_S_RPM);
}
inline bool Gen::engineaudio::AEMSMix_S_RPM(EA::Reflection::Float &result) const {
    result = AEMSMix_S_RPM();
    return true;
}
inline bool Gen::engineaudio::SET_AEMSMix_S_RPM(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(AEMSMix_S_RPM, input);
}

inline bool Gen::engineaudio::AEMSVol(TAttrib<EA::Reflection::UInt32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0x974eb50a);
}
inline const EA::Reflection::UInt32 &Gen::engineaudio::AEMSVol() const {
    ATTRIB_CODEGEN_GETLAYOUT(AEMSVol);
}
inline bool Gen::engineaudio::AEMSVol(EA::Reflection::UInt32 &result) const {
    result = AEMSVol();
    return true;
}
inline bool Gen::engineaudio::SET_AEMSVol(const EA::Reflection::UInt32 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(AEMSVol, input);
}

inline bool Gen::engineaudio::AccelDeltaRPMThreshold(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x3b82c385);
}
inline const EA::Reflection::Float &Gen::engineaudio::AccelDeltaRPMThreshold() const {
    ATTRIB_CODEGEN_GETLAYOUT(AccelDeltaRPMThreshold);
}
inline bool Gen::engineaudio::AccelDeltaRPMThreshold(EA::Reflection::Float &result) const {
    result = AccelDeltaRPMThreshold();
    return true;
}
inline bool Gen::engineaudio::SET_AccelDeltaRPMThreshold(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(AccelDeltaRPMThreshold, input);
}

inline bool Gen::engineaudio::BankName_auxRAM(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x04935eab);
}
inline const Attrib::StringKey &Gen::engineaudio::BankName_auxRAM(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(Attrib::StringKey, 0x04935eab, index);
}
inline bool Gen::engineaudio::BankName_auxRAM(Attrib::StringKey &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(Attrib::StringKey, 0x04935eab, result, index);
}
inline unsigned int Gen::engineaudio::Num_BankName_auxRAM() const {
    ATTRIB_CODEGEN_GETLENGTH(0x04935eab);
}
inline bool Gen::engineaudio::SET_BankName_auxRAM(const Attrib::StringKey &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(Attrib::StringKey, 0x04935eab, input, index);
}

inline bool Gen::engineaudio::BankName_mainRAM(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x2fc9c96f);
}
inline const Attrib::StringKey &Gen::engineaudio::BankName_mainRAM() const {
    ATTRIB_CODEGEN_GETLAYOUT(BankName_mainRAM);
}
inline bool Gen::engineaudio::BankName_mainRAM(Attrib::StringKey &result) const {
    result = BankName_mainRAM();
    return true;
}
inline bool Gen::engineaudio::SET_BankName_mainRAM(const Attrib::StringKey &input) {
    ATTRIB_CODEGEN_SETLAYOUT(BankName_mainRAM, input);
}

inline bool Gen::engineaudio::CarID(TAttrib<EA::Reflection::UInt32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0x0c35b607);
}
inline const EA::Reflection::UInt32 &Gen::engineaudio::CarID() const {
    ATTRIB_CODEGEN_GETLAYOUT(CarID);
}
inline bool Gen::engineaudio::CarID(EA::Reflection::UInt32 &result) const {
    result = CarID();
    return true;
}
inline bool Gen::engineaudio::SET_CarID(const EA::Reflection::UInt32 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(CarID, input);
}

inline bool Gen::engineaudio::CollectionName(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x9ca1c8f9);
}
inline const EA::Reflection::Text &Gen::engineaudio::CollectionName() const {
    ATTRIB_CODEGEN_GETLAYOUT(CollectionName);
}
inline bool Gen::engineaudio::CollectionName(EA::Reflection::Text &result) const {
    result = CollectionName();
    return true;
}
inline bool Gen::engineaudio::SET_CollectionName(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETLAYOUT(CollectionName, input);
}

inline bool Gen::engineaudio::DECEL_AEMSMix_L_RPM(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x8c22df44);
}
inline const EA::Reflection::Float &Gen::engineaudio::DECEL_AEMSMix_L_RPM() const {
    ATTRIB_CODEGEN_GETLAYOUT(DECEL_AEMSMix_L_RPM);
}
inline bool Gen::engineaudio::DECEL_AEMSMix_L_RPM(EA::Reflection::Float &result) const {
    result = DECEL_AEMSMix_L_RPM();
    return true;
}
inline bool Gen::engineaudio::SET_DECEL_AEMSMix_L_RPM(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(DECEL_AEMSMix_L_RPM, input);
}

inline bool Gen::engineaudio::DECEL_AEMSMix_S_RPM(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x3c9445e9);
}
inline const EA::Reflection::Float &Gen::engineaudio::DECEL_AEMSMix_S_RPM() const {
    ATTRIB_CODEGEN_GETLAYOUT(DECEL_AEMSMix_S_RPM);
}
inline bool Gen::engineaudio::DECEL_AEMSMix_S_RPM(EA::Reflection::Float &result) const {
    result = DECEL_AEMSMix_S_RPM();
    return true;
}
inline bool Gen::engineaudio::SET_DECEL_AEMSMix_S_RPM(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(DECEL_AEMSMix_S_RPM, input);
}

inline bool Gen::engineaudio::DECEL_AEMSVol(TAttrib<EA::Reflection::UInt32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0x0f7c0093);
}
inline const EA::Reflection::UInt32 &Gen::engineaudio::DECEL_AEMSVol() const {
    ATTRIB_CODEGEN_GETLAYOUT(DECEL_AEMSVol);
}
inline bool Gen::engineaudio::DECEL_AEMSVol(EA::Reflection::UInt32 &result) const {
    result = DECEL_AEMSVol();
    return true;
}
inline bool Gen::engineaudio::SET_DECEL_AEMSVol(const EA::Reflection::UInt32 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(DECEL_AEMSVol, input);
}

inline bool Gen::engineaudio::DECEL_GINSUMix_L_RPM(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x4fe476f3);
}
inline const EA::Reflection::Float &Gen::engineaudio::DECEL_GINSUMix_L_RPM() const {
    ATTRIB_CODEGEN_GETLAYOUT(DECEL_GINSUMix_L_RPM);
}
inline bool Gen::engineaudio::DECEL_GINSUMix_L_RPM(EA::Reflection::Float &result) const {
    result = DECEL_GINSUMix_L_RPM();
    return true;
}
inline bool Gen::engineaudio::SET_DECEL_GINSUMix_L_RPM(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(DECEL_GINSUMix_L_RPM, input);
}

inline bool Gen::engineaudio::DECEL_GINSUMix_S_RPM(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x791768a0);
}
inline const EA::Reflection::Float &Gen::engineaudio::DECEL_GINSUMix_S_RPM() const {
    ATTRIB_CODEGEN_GETLAYOUT(DECEL_GINSUMix_S_RPM);
}
inline bool Gen::engineaudio::DECEL_GINSUMix_S_RPM(EA::Reflection::Float &result) const {
    result = DECEL_GINSUMix_S_RPM();
    return true;
}
inline bool Gen::engineaudio::SET_DECEL_GINSUMix_S_RPM(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(DECEL_GINSUMix_S_RPM, input);
}

inline bool Gen::engineaudio::DecelDeltaRPMThreshold(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x6114d1a5);
}
inline const EA::Reflection::Float &Gen::engineaudio::DecelDeltaRPMThreshold() const {
    ATTRIB_CODEGEN_GETLAYOUT(DecelDeltaRPMThreshold);
}
inline bool Gen::engineaudio::DecelDeltaRPMThreshold(EA::Reflection::Float &result) const {
    result = DecelDeltaRPMThreshold();
    return true;
}
inline bool Gen::engineaudio::SET_DecelDeltaRPMThreshold(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(DecelDeltaRPMThreshold, input);
}

inline bool Gen::engineaudio::DecelPitchOffset(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x313385dc);
}
inline const EA::Reflection::Float &Gen::engineaudio::DecelPitchOffset() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x313385dc);
}
inline bool Gen::engineaudio::DecelPitchOffset(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x313385dc, result);
}
inline bool Gen::engineaudio::SET_DecelPitchOffset(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x313385dc, input);
}

inline bool Gen::engineaudio::EngType(TAttrib<eENGINE_GROUP> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(eENGINE_GROUP, 0x1d64eafb);
}
inline const eENGINE_GROUP &Gen::engineaudio::EngType() const {
    ATTRIB_CODEGEN_GETLAYOUT(EngType);
}
inline bool Gen::engineaudio::EngType(eENGINE_GROUP &result) const {
    result = EngType();
    return true;
}
inline bool Gen::engineaudio::SET_EngType(const eENGINE_GROUP &input) {
    ATTRIB_CODEGEN_SETLAYOUT(EngType, input);
}

inline bool Gen::engineaudio::Filename_GinsuAccel(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x2cee0a54);
}
inline const Attrib::StringKey &Gen::engineaudio::Filename_GinsuAccel() const {
    ATTRIB_CODEGEN_GETLAYOUT(Filename_GinsuAccel);
}
inline bool Gen::engineaudio::Filename_GinsuAccel(Attrib::StringKey &result) const {
    result = Filename_GinsuAccel();
    return true;
}
inline bool Gen::engineaudio::SET_Filename_GinsuAccel(const Attrib::StringKey &input) {
    ATTRIB_CODEGEN_SETLAYOUT(Filename_GinsuAccel, input);
}

inline bool Gen::engineaudio::Filename_GinsuDecel(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x3a7c2f3d);
}
inline const Attrib::StringKey &Gen::engineaudio::Filename_GinsuDecel() const {
    ATTRIB_CODEGEN_GETLAYOUT(Filename_GinsuDecel);
}
inline bool Gen::engineaudio::Filename_GinsuDecel(Attrib::StringKey &result) const {
    result = Filename_GinsuDecel();
    return true;
}
inline bool Gen::engineaudio::SET_Filename_GinsuDecel(const Attrib::StringKey &input) {
    ATTRIB_CODEGEN_SETLAYOUT(Filename_GinsuDecel, input);
}

inline bool Gen::engineaudio::GINSUAccelVol(TAttrib<EA::Reflection::UInt32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0xc3f787b1);
}
inline const EA::Reflection::UInt32 &Gen::engineaudio::GINSUAccelVol() const {
    ATTRIB_CODEGEN_GETLAYOUT(GINSUAccelVol);
}
inline bool Gen::engineaudio::GINSUAccelVol(EA::Reflection::UInt32 &result) const {
    result = GINSUAccelVol();
    return true;
}
inline bool Gen::engineaudio::SET_GINSUAccelVol(const EA::Reflection::UInt32 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(GINSUAccelVol, input);
}

inline bool Gen::engineaudio::GINSUMix_L_RPM(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xd996fd76);
}
inline const EA::Reflection::Float &Gen::engineaudio::GINSUMix_L_RPM() const {
    ATTRIB_CODEGEN_GETLAYOUT(GINSUMix_L_RPM);
}
inline bool Gen::engineaudio::GINSUMix_L_RPM(EA::Reflection::Float &result) const {
    result = GINSUMix_L_RPM();
    return true;
}
inline bool Gen::engineaudio::SET_GINSUMix_L_RPM(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(GINSUMix_L_RPM, input);
}

inline bool Gen::engineaudio::GINSUMix_S_RPM(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xc2f38240);
}
inline const EA::Reflection::Float &Gen::engineaudio::GINSUMix_S_RPM() const {
    ATTRIB_CODEGEN_GETLAYOUT(GINSUMix_S_RPM);
}
inline bool Gen::engineaudio::GINSUMix_S_RPM(EA::Reflection::Float &result) const {
    result = GINSUMix_S_RPM();
    return true;
}
inline bool Gen::engineaudio::SET_GINSUMix_S_RPM(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(GINSUMix_S_RPM, input);
}

inline bool Gen::engineaudio::GINSU_DECEL_FADE_IN(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x56ba0258);
}
inline const EA::Reflection::Float &Gen::engineaudio::GINSU_DECEL_FADE_IN() const {
    ATTRIB_CODEGEN_GETLAYOUT(GINSU_DECEL_FADE_IN);
}
inline bool Gen::engineaudio::GINSU_DECEL_FADE_IN(EA::Reflection::Float &result) const {
    result = GINSU_DECEL_FADE_IN();
    return true;
}
inline bool Gen::engineaudio::SET_GINSU_DECEL_FADE_IN(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(GINSU_DECEL_FADE_IN, input);
}

inline bool Gen::engineaudio::GINSU_DECEL_FADE_OUT(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x18b901f4);
}
inline const EA::Reflection::Float &Gen::engineaudio::GINSU_DECEL_FADE_OUT() const {
    ATTRIB_CODEGEN_GETLAYOUT(GINSU_DECEL_FADE_OUT);
}
inline bool Gen::engineaudio::GINSU_DECEL_FADE_OUT(EA::Reflection::Float &result) const {
    result = GINSU_DECEL_FADE_OUT();
    return true;
}
inline bool Gen::engineaudio::SET_GINSU_DECEL_FADE_OUT(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(GINSU_DECEL_FADE_OUT, input);
}

inline bool Gen::engineaudio::GINSU_Decel_MaxRPM(TAttrib<EA::Reflection::UInt32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0x414946cc);
}
inline const EA::Reflection::UInt32 &Gen::engineaudio::GINSU_Decel_MaxRPM() const {
    ATTRIB_CODEGEN_GETLAYOUT(GINSU_Decel_MaxRPM);
}
inline bool Gen::engineaudio::GINSU_Decel_MaxRPM(EA::Reflection::UInt32 &result) const {
    result = GINSU_Decel_MaxRPM();
    return true;
}
inline bool Gen::engineaudio::SET_GINSU_Decel_MaxRPM(const EA::Reflection::UInt32 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(GINSU_Decel_MaxRPM, input);
}

inline bool Gen::engineaudio::GINSU_Decel_MinRPM(TAttrib<EA::Reflection::UInt32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0xd1b4e1b2);
}
inline const EA::Reflection::UInt32 &Gen::engineaudio::GINSU_Decel_MinRPM() const {
    ATTRIB_CODEGEN_GETLAYOUT(GINSU_Decel_MinRPM);
}
inline bool Gen::engineaudio::GINSU_Decel_MinRPM(EA::Reflection::UInt32 &result) const {
    result = GINSU_Decel_MinRPM();
    return true;
}
inline bool Gen::engineaudio::SET_GINSU_Decel_MinRPM(const EA::Reflection::UInt32 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(GINSU_Decel_MinRPM, input);
}

inline bool Gen::engineaudio::GINSU_LowPassCutoff(TAttrib<EA::Reflection::UInt32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0xe3836473);
}
inline const EA::Reflection::UInt32 &Gen::engineaudio::GINSU_LowPassCutoff() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::UInt32, 0xe3836473);
}
inline bool Gen::engineaudio::GINSU_LowPassCutoff(EA::Reflection::UInt32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::UInt32, 0xe3836473, result);
}
inline bool Gen::engineaudio::SET_GINSU_LowPassCutoff(const EA::Reflection::UInt32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::UInt32, 0xe3836473, input);
}

inline bool Gen::engineaudio::GinsuDecelVol(TAttrib<EA::Reflection::UInt32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0x38be973d);
}
inline const EA::Reflection::UInt32 &Gen::engineaudio::GinsuDecelVol() const {
    ATTRIB_CODEGEN_GETLAYOUT(GinsuDecelVol);
}
inline bool Gen::engineaudio::GinsuDecelVol(EA::Reflection::UInt32 &result) const {
    result = GinsuDecelVol();
    return true;
}
inline bool Gen::engineaudio::SET_GinsuDecelVol(const EA::Reflection::UInt32 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(GinsuDecelVol, input);
}

inline bool Gen::engineaudio::Ginsu_ACL_Neg_L_RPM(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x782f433d);
}
inline const EA::Reflection::Float &Gen::engineaudio::Ginsu_ACL_Neg_L_RPM() const {
    ATTRIB_CODEGEN_GETLAYOUT(Ginsu_ACL_Neg_L_RPM);
}
inline bool Gen::engineaudio::Ginsu_ACL_Neg_L_RPM(EA::Reflection::Float &result) const {
    result = Ginsu_ACL_Neg_L_RPM();
    return true;
}
inline bool Gen::engineaudio::SET_Ginsu_ACL_Neg_L_RPM(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(Ginsu_ACL_Neg_L_RPM, input);
}

inline bool Gen::engineaudio::Ginsu_ACL_Neg_S_RPM(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x38afe02e);
}
inline const EA::Reflection::Float &Gen::engineaudio::Ginsu_ACL_Neg_S_RPM() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x38afe02e);
}
inline bool Gen::engineaudio::Ginsu_ACL_Neg_S_RPM(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x38afe02e, result);
}
inline bool Gen::engineaudio::SET_Ginsu_ACL_Neg_S_RPM(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x38afe02e, input);
}

inline bool Gen::engineaudio::Master_Vol(TAttrib<EA::Reflection::UInt16> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt16, 0xecc59244);
}
inline const EA::Reflection::UInt16 &Gen::engineaudio::Master_Vol() const {
    ATTRIB_CODEGEN_GETLAYOUT(Master_Vol);
}
inline bool Gen::engineaudio::Master_Vol(EA::Reflection::UInt16 &result) const {
    result = Master_Vol();
    return true;
}
inline bool Gen::engineaudio::SET_Master_Vol(const EA::Reflection::UInt16 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(Master_Vol, input);
}

inline bool Gen::engineaudio::MaxRPM(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xa4d878ef);
}
inline const EA::Reflection::Float &Gen::engineaudio::MaxRPM() const {
    ATTRIB_CODEGEN_GETLAYOUT(MaxRPM);
}
inline bool Gen::engineaudio::MaxRPM(EA::Reflection::Float &result) const {
    result = MaxRPM();
    return true;
}
inline bool Gen::engineaudio::SET_MaxRPM(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(MaxRPM, input);
}

inline bool Gen::engineaudio::MaybeV8(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x6659143b);
}
inline const EA::Reflection::Bool &Gen::engineaudio::MaybeV8() const {
    ATTRIB_CODEGEN_GETLAYOUT(MaybeV8);
}
inline bool Gen::engineaudio::MaybeV8(EA::Reflection::Bool &result) const {
    result = MaybeV8();
    return true;
}
inline bool Gen::engineaudio::SET_MaybeV8(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETLAYOUT(MaybeV8, input);
}

inline bool Gen::engineaudio::MinRPM(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x3ac8b868);
}
inline const EA::Reflection::Float &Gen::engineaudio::MinRPM() const {
    ATTRIB_CODEGEN_GETLAYOUT(MinRPM);
}
inline bool Gen::engineaudio::MinRPM(EA::Reflection::Float &result) const {
    result = MinRPM();
    return true;
}
inline bool Gen::engineaudio::SET_MinRPM(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(MinRPM, input);
}

inline bool Gen::engineaudio::PhysicsRPM_Map(TAttrib<UMath::Matrix4> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Matrix4, 0x07e3c833);
}
inline const UMath::Matrix4 &Gen::engineaudio::PhysicsRPM_Map() const {
    ATTRIB_CODEGEN_GETVALUE(UMath::Matrix4, 0x07e3c833);
}
inline bool Gen::engineaudio::PhysicsRPM_Map(UMath::Matrix4 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(UMath::Matrix4, 0x07e3c833, result);
}
inline bool Gen::engineaudio::SET_PhysicsRPM_Map(const UMath::Matrix4 &input) {
    ATTRIB_CODEGEN_SETVALUE(UMath::Matrix4, 0x07e3c833, input);
}

inline bool Gen::engineaudio::Priority(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xa9598843);
}
inline const EA::Reflection::Float &Gen::engineaudio::Priority() const {
    ATTRIB_CODEGEN_GETLAYOUT(Priority);
}
inline bool Gen::engineaudio::Priority(EA::Reflection::Float &result) const {
    result = Priority();
    return true;
}
inline bool Gen::engineaudio::SET_Priority(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(Priority, input);
}

inline bool Gen::engineaudio::SweetBank(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0xee501c6a);
}
inline const Attrib::StringKey &Gen::engineaudio::SweetBank(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(Attrib::StringKey, 0xee501c6a, index);
}
inline bool Gen::engineaudio::SweetBank(Attrib::StringKey &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(Attrib::StringKey, 0xee501c6a, result, index);
}
inline unsigned int Gen::engineaudio::Num_SweetBank() const {
    ATTRIB_CODEGEN_GETLENGTH(0xee501c6a);
}
inline bool Gen::engineaudio::SET_SweetBank(const Attrib::StringKey &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(Attrib::StringKey, 0xee501c6a, input, index);
}

inline bool Gen::engineaudio::Tranny(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x83066633);
}
inline const EA::Reflection::Bool &Gen::engineaudio::Tranny() const {
    ATTRIB_CODEGEN_GETLAYOUT(Tranny);
}
inline bool Gen::engineaudio::Tranny(EA::Reflection::Bool &result) const {
    result = Tranny();
    return true;
}
inline bool Gen::engineaudio::SET_Tranny(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETLAYOUT(Tranny, input);
}

inline bool Gen::engineaudio::Vol_ShiftSweets(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x34beca33);
}
inline const EA::Reflection::Int32 &Gen::engineaudio::Vol_ShiftSweets() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x34beca33);
}
inline bool Gen::engineaudio::Vol_ShiftSweets(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x34beca33, result);
}
inline bool Gen::engineaudio::SET_Vol_ShiftSweets(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x34beca33, input);
}

inline bool Gen::engineaudio::Vol_Sputters(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xc1eddd78);
}
inline const EA::Reflection::Int32 &Gen::engineaudio::Vol_Sputters() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xc1eddd78);
}
inline bool Gen::engineaudio::Vol_Sputters(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xc1eddd78, result);
}
inline bool Gen::engineaudio::SET_Vol_Sputters(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xc1eddd78, input);
}

inline bool Gen::engineaudio::acceltrans(TAttrib<RefSpec> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0xff77f451);
}
inline const RefSpec &Gen::engineaudio::acceltrans() const {
    ATTRIB_CODEGEN_GETLAYOUT(acceltrans);
}
inline bool Gen::engineaudio::acceltrans(RefSpec &result) const {
    result = acceltrans();
    return true;
}
inline bool Gen::engineaudio::SET_acceltrans(const RefSpec &input) {
    ATTRIB_CODEGEN_SETLAYOUT(acceltrans, input);
}

}; // namespace Attrib

#endif
