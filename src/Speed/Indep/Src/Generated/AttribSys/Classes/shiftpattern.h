#ifndef _attrib_gen_shiftpattern_h
#define _attrib_gen_shiftpattern_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct shiftpattern : Instance {
    struct _LayoutStruct {
        Attrib::StringKey BankName;                     // offset 0x0, size 0x10
        EA::Reflection::Float Down_Shift_Sound_Delay;   // offset 0x10, size 0x4
        EA::Reflection::Float Up_Engaging_Attack_Vol;   // offset 0x14, size 0x4
        EA::Reflection::UInt32 LFO_RPM_Amp;             // offset 0x18, size 0x4
        EA::Reflection::UInt32 LFO_Vol_Amp;             // offset 0x1c, size 0x4
        EA::Reflection::UInt32 Down_Engaging_Rise_T;    // offset 0x20, size 0x4
        EA::Reflection::UInt32 Up_Engaging_Attack_T;    // offset 0x24, size 0x4
        EA::Reflection::UInt32 LFO_RPM_Decay_Time;      // offset 0x28, size 0x4
        EA::Reflection::Float Down_Reattach_Scale;      // offset 0x2c, size 0x4
        EA::Reflection::UInt32 Down_Engaging_Fall_T;    // offset 0x30, size 0x4
        EA::Reflection::Float Up_Shift_Sound_Delay;     // offset 0x34, size 0x4
        EA::Reflection::UInt32 Up_Vol_Shift;            // offset 0x38, size 0x4
        EA::Reflection::UInt32 Down_Disengage_Fall_T;   // offset 0x3c, size 0x4
        EA::Reflection::UInt32 LFO_Vol_Freq;            // offset 0x40, size 0x4
        EA::Reflection::UInt32 Down_Disengage_Fall_RPM; // offset 0x44, size 0x4
        EA::Reflection::UInt32 Down_Vol_Shift;          // offset 0x48, size 0x4
        EA::Reflection::UInt32 Down_Engaging_Rise_RPM;  // offset 0x4c, size 0x4
        EA::Reflection::UInt32 LFO_RPM_Freq;            // offset 0x50, size 0x4
        EA::Reflection::UInt32 LFO_Vol_Decay_Time;      // offset 0x54, size 0x4
    };

    typedef Attrib::StringKey TypeOf_BankName;
    typedef EA::Reflection::UInt32 TypeOf_Down_Disengage_Fall_RPM;
    typedef EA::Reflection::UInt32 TypeOf_Down_Disengage_Fall_T;
    typedef EA::Reflection::UInt32 TypeOf_Down_Engaging_Fall_RPM;
    typedef EA::Reflection::UInt32 TypeOf_Down_Engaging_Fall_T;
    typedef EA::Reflection::UInt32 TypeOf_Down_Engaging_Rise_RPM;
    typedef EA::Reflection::UInt32 TypeOf_Down_Engaging_Rise_T;
    typedef EA::Reflection::Float TypeOf_Down_Reattach_Scale;
    typedef EA::Reflection::Float TypeOf_Down_Shift_Sound_Delay;
    typedef EA::Reflection::UInt32 TypeOf_Down_Vol_Shift;
    typedef EA::Reflection::UInt32 TypeOf_LFO_RPM_Amp;
    typedef EA::Reflection::UInt32 TypeOf_LFO_RPM_Decay_Time;
    typedef EA::Reflection::UInt32 TypeOf_LFO_RPM_Freq;
    typedef EA::Reflection::UInt32 TypeOf_LFO_Vol_Amp;
    typedef EA::Reflection::UInt32 TypeOf_LFO_Vol_Decay_Time;
    typedef EA::Reflection::UInt32 TypeOf_LFO_Vol_Freq;
    typedef stShiftPair TypeOf_Up_DisengageFall;
    typedef UMath::Matrix4 TypeOf_Up_DisengageFall_Curve;
    typedef stShiftPair TypeOf_Up_Engage;
    typedef UMath::Matrix4 TypeOf_Up_Engage_Curve;
    typedef EA::Reflection::UInt32 TypeOf_Up_Engaging_Attack_T;
    typedef EA::Reflection::Float TypeOf_Up_Engaging_Attack_Vol;
    typedef EA::Reflection::Float TypeOf_Up_Shift_Sound_Delay;
    typedef EA::Reflection::UInt32 TypeOf_Up_Vol_Shift;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("shiftpattern");
    shiftpattern(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    shiftpattern(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    shiftpattern(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    shiftpattern(const shiftpattern &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    shiftpattern(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~shiftpattern() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0xdb01b754;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0xdb01b754, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const shiftpattern &operator=(const shiftpattern &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const shiftpattern &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool BankName(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0xbf49a7d9);
    }
    bool BankName(Attrib::StringKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(BankName, result);
    }
    const Attrib::StringKey &BankName() const {
        ATTRIB_CODEGEN_GETLAYOUT(BankName);
    }
    bool SET_BankName(const Attrib::StringKey &input) {
        ATTRIB_CODEGEN_SETLAYOUT(BankName, input);
    }
    bool Down_Disengage_Fall_RPM(TAttrib<EA::Reflection::UInt32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0x16d7a0b1);
    }
    bool Down_Disengage_Fall_RPM(EA::Reflection::UInt32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(Down_Disengage_Fall_RPM, result);
    }
    const EA::Reflection::UInt32 &Down_Disengage_Fall_RPM() const {
        ATTRIB_CODEGEN_GETLAYOUT(Down_Disengage_Fall_RPM);
    }
    bool SET_Down_Disengage_Fall_RPM(const EA::Reflection::UInt32 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(Down_Disengage_Fall_RPM, input);
    }
    bool Down_Disengage_Fall_T(TAttrib<EA::Reflection::UInt32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0x560c34cc);
    }
    bool Down_Disengage_Fall_T(EA::Reflection::UInt32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(Down_Disengage_Fall_T, result);
    }
    const EA::Reflection::UInt32 &Down_Disengage_Fall_T() const {
        ATTRIB_CODEGEN_GETLAYOUT(Down_Disengage_Fall_T);
    }
    bool SET_Down_Disengage_Fall_T(const EA::Reflection::UInt32 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(Down_Disengage_Fall_T, input);
    }
    bool Down_Engaging_Fall_RPM(TAttrib<EA::Reflection::UInt32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0x3e1a0db6);
    }
    bool Down_Engaging_Fall_RPM(EA::Reflection::UInt32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::UInt32, 0x3e1a0db6, result);
    }
    const EA::Reflection::UInt32 &Down_Engaging_Fall_RPM() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::UInt32, 0x3e1a0db6);
    }
    bool SET_Down_Engaging_Fall_RPM(const EA::Reflection::UInt32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::UInt32, 0x3e1a0db6, input);
    }
    bool Down_Engaging_Fall_T(TAttrib<EA::Reflection::UInt32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0x84d1b9a4);
    }
    bool Down_Engaging_Fall_T(EA::Reflection::UInt32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(Down_Engaging_Fall_T, result);
    }
    const EA::Reflection::UInt32 &Down_Engaging_Fall_T() const {
        ATTRIB_CODEGEN_GETLAYOUT(Down_Engaging_Fall_T);
    }
    bool SET_Down_Engaging_Fall_T(const EA::Reflection::UInt32 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(Down_Engaging_Fall_T, input);
    }
    bool Down_Engaging_Rise_RPM(TAttrib<EA::Reflection::UInt32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0x8818c1fe);
    }
    bool Down_Engaging_Rise_RPM(EA::Reflection::UInt32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(Down_Engaging_Rise_RPM, result);
    }
    const EA::Reflection::UInt32 &Down_Engaging_Rise_RPM() const {
        ATTRIB_CODEGEN_GETLAYOUT(Down_Engaging_Rise_RPM);
    }
    bool SET_Down_Engaging_Rise_RPM(const EA::Reflection::UInt32 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(Down_Engaging_Rise_RPM, input);
    }
    bool Down_Engaging_Rise_T(TAttrib<EA::Reflection::UInt32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0x693ebff3);
    }
    bool Down_Engaging_Rise_T(EA::Reflection::UInt32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(Down_Engaging_Rise_T, result);
    }
    const EA::Reflection::UInt32 &Down_Engaging_Rise_T() const {
        ATTRIB_CODEGEN_GETLAYOUT(Down_Engaging_Rise_T);
    }
    bool SET_Down_Engaging_Rise_T(const EA::Reflection::UInt32 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(Down_Engaging_Rise_T, input);
    }
    bool Down_Reattach_Scale(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xf1065907);
    }
    bool Down_Reattach_Scale(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(Down_Reattach_Scale, result);
    }
    const EA::Reflection::Float &Down_Reattach_Scale() const {
        ATTRIB_CODEGEN_GETLAYOUT(Down_Reattach_Scale);
    }
    bool SET_Down_Reattach_Scale(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(Down_Reattach_Scale, input);
    }
    bool Down_Shift_Sound_Delay(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x64e9dc9e);
    }
    bool Down_Shift_Sound_Delay(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(Down_Shift_Sound_Delay, result);
    }
    const EA::Reflection::Float &Down_Shift_Sound_Delay() const {
        ATTRIB_CODEGEN_GETLAYOUT(Down_Shift_Sound_Delay);
    }
    bool SET_Down_Shift_Sound_Delay(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(Down_Shift_Sound_Delay, input);
    }
    bool Down_Vol_Shift(TAttrib<EA::Reflection::UInt32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0x823fc1c5);
    }
    bool Down_Vol_Shift(EA::Reflection::UInt32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(Down_Vol_Shift, result);
    }
    const EA::Reflection::UInt32 &Down_Vol_Shift() const {
        ATTRIB_CODEGEN_GETLAYOUT(Down_Vol_Shift);
    }
    bool SET_Down_Vol_Shift(const EA::Reflection::UInt32 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(Down_Vol_Shift, input);
    }
    bool LFO_RPM_Amp(TAttrib<EA::Reflection::UInt32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0xd1172bd0);
    }
    bool LFO_RPM_Amp(EA::Reflection::UInt32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(LFO_RPM_Amp, result);
    }
    const EA::Reflection::UInt32 &LFO_RPM_Amp() const {
        ATTRIB_CODEGEN_GETLAYOUT(LFO_RPM_Amp);
    }
    bool SET_LFO_RPM_Amp(const EA::Reflection::UInt32 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(LFO_RPM_Amp, input);
    }
    bool LFO_RPM_Decay_Time(TAttrib<EA::Reflection::UInt32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0xda0dfcde);
    }
    bool LFO_RPM_Decay_Time(EA::Reflection::UInt32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(LFO_RPM_Decay_Time, result);
    }
    const EA::Reflection::UInt32 &LFO_RPM_Decay_Time() const {
        ATTRIB_CODEGEN_GETLAYOUT(LFO_RPM_Decay_Time);
    }
    bool SET_LFO_RPM_Decay_Time(const EA::Reflection::UInt32 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(LFO_RPM_Decay_Time, input);
    }
    bool LFO_RPM_Freq(TAttrib<EA::Reflection::UInt32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0x27d396ac);
    }
    bool LFO_RPM_Freq(EA::Reflection::UInt32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(LFO_RPM_Freq, result);
    }
    const EA::Reflection::UInt32 &LFO_RPM_Freq() const {
        ATTRIB_CODEGEN_GETLAYOUT(LFO_RPM_Freq);
    }
    bool SET_LFO_RPM_Freq(const EA::Reflection::UInt32 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(LFO_RPM_Freq, input);
    }
    bool LFO_Vol_Amp(TAttrib<EA::Reflection::UInt32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0x1c9e97c6);
    }
    bool LFO_Vol_Amp(EA::Reflection::UInt32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(LFO_Vol_Amp, result);
    }
    const EA::Reflection::UInt32 &LFO_Vol_Amp() const {
        ATTRIB_CODEGEN_GETLAYOUT(LFO_Vol_Amp);
    }
    bool SET_LFO_Vol_Amp(const EA::Reflection::UInt32 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(LFO_Vol_Amp, input);
    }
    bool LFO_Vol_Decay_Time(TAttrib<EA::Reflection::UInt32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0x748eb252);
    }
    bool LFO_Vol_Decay_Time(EA::Reflection::UInt32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(LFO_Vol_Decay_Time, result);
    }
    const EA::Reflection::UInt32 &LFO_Vol_Decay_Time() const {
        ATTRIB_CODEGEN_GETLAYOUT(LFO_Vol_Decay_Time);
    }
    bool SET_LFO_Vol_Decay_Time(const EA::Reflection::UInt32 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(LFO_Vol_Decay_Time, input);
    }
    bool LFO_Vol_Freq(TAttrib<EA::Reflection::UInt32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0x0c531256);
    }
    bool LFO_Vol_Freq(EA::Reflection::UInt32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(LFO_Vol_Freq, result);
    }
    const EA::Reflection::UInt32 &LFO_Vol_Freq() const {
        ATTRIB_CODEGEN_GETLAYOUT(LFO_Vol_Freq);
    }
    bool SET_LFO_Vol_Freq(const EA::Reflection::UInt32 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(LFO_Vol_Freq, input);
    }
    bool Up_DisengageFall(TAttrib<stShiftPair> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(stShiftPair, 0xf2d90101);
    }
    bool Up_DisengageFall(stShiftPair &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(stShiftPair, 0xf2d90101, result, index);
    }
    const stShiftPair &Up_DisengageFall(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(stShiftPair, 0xf2d90101, index);
    }
    unsigned int Num_Up_DisengageFall() const {
        ATTRIB_CODEGEN_GETLENGTH(0xf2d90101);
    }
    bool SET_Up_DisengageFall(const stShiftPair &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(stShiftPair, 0xf2d90101, input, index);
    }
    bool Up_DisengageFall_Curve(TAttrib<UMath::Matrix4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Matrix4, 0xf040e6b0);
    }
    bool Up_DisengageFall_Curve(UMath::Matrix4 &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(UMath::Matrix4, 0xf040e6b0, result, index);
    }
    const UMath::Matrix4 &Up_DisengageFall_Curve(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(UMath::Matrix4, 0xf040e6b0, index);
    }
    unsigned int Num_Up_DisengageFall_Curve() const {
        ATTRIB_CODEGEN_GETLENGTH(0xf040e6b0);
    }
    bool SET_Up_DisengageFall_Curve(const UMath::Matrix4 &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(UMath::Matrix4, 0xf040e6b0, input, index);
    }
    bool Up_Engage(TAttrib<stShiftPair> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(stShiftPair, 0xcb89b8c8);
    }
    bool Up_Engage(stShiftPair &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(stShiftPair, 0xcb89b8c8, result);
    }
    const stShiftPair &Up_Engage() const {
        ATTRIB_CODEGEN_GETVALUE(stShiftPair, 0xcb89b8c8);
    }
    bool SET_Up_Engage(const stShiftPair &input) {
        ATTRIB_CODEGEN_SETVALUE(stShiftPair, 0xcb89b8c8, input);
    }
    bool Up_Engage_Curve(TAttrib<UMath::Matrix4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Matrix4, 0x68da6275);
    }
    bool Up_Engage_Curve(UMath::Matrix4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(UMath::Matrix4, 0x68da6275, result);
    }
    const UMath::Matrix4 &Up_Engage_Curve() const {
        ATTRIB_CODEGEN_GETVALUE(UMath::Matrix4, 0x68da6275);
    }
    bool SET_Up_Engage_Curve(const UMath::Matrix4 &input) {
        ATTRIB_CODEGEN_SETVALUE(UMath::Matrix4, 0x68da6275, input);
    }
    bool Up_Engaging_Attack_T(TAttrib<EA::Reflection::UInt32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0x50b23fbc);
    }
    bool Up_Engaging_Attack_T(EA::Reflection::UInt32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(Up_Engaging_Attack_T, result);
    }
    const EA::Reflection::UInt32 &Up_Engaging_Attack_T() const {
        ATTRIB_CODEGEN_GETLAYOUT(Up_Engaging_Attack_T);
    }
    bool SET_Up_Engaging_Attack_T(const EA::Reflection::UInt32 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(Up_Engaging_Attack_T, input);
    }
    bool Up_Engaging_Attack_Vol(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x419c66c9);
    }
    bool Up_Engaging_Attack_Vol(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(Up_Engaging_Attack_Vol, result);
    }
    const EA::Reflection::Float &Up_Engaging_Attack_Vol() const {
        ATTRIB_CODEGEN_GETLAYOUT(Up_Engaging_Attack_Vol);
    }
    bool SET_Up_Engaging_Attack_Vol(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(Up_Engaging_Attack_Vol, input);
    }
    bool Up_Shift_Sound_Delay(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xed3007a0);
    }
    bool Up_Shift_Sound_Delay(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(Up_Shift_Sound_Delay, result);
    }
    const EA::Reflection::Float &Up_Shift_Sound_Delay() const {
        ATTRIB_CODEGEN_GETLAYOUT(Up_Shift_Sound_Delay);
    }
    bool SET_Up_Shift_Sound_Delay(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(Up_Shift_Sound_Delay, input);
    }
    bool Up_Vol_Shift(TAttrib<EA::Reflection::UInt32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0xbdcb972f);
    }
    bool Up_Vol_Shift(EA::Reflection::UInt32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(Up_Vol_Shift, result);
    }
    const EA::Reflection::UInt32 &Up_Vol_Shift() const {
        ATTRIB_CODEGEN_GETLAYOUT(Up_Vol_Shift);
    }
    bool SET_Up_Vol_Shift(const EA::Reflection::UInt32 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(Up_Vol_Shift, input);
    }

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    shiftpattern &ConvertFromInstance(Instance &src) {}
    const shiftpattern &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key shiftpattern = 0xdb01b754;

}; // namespace ClassName

namespace Hash {
namespace shiftpattern {

static const Key BankName = 0xbf49a7d9;
static const Key Down_Disengage_Fall_RPM = 0x16d7a0b1;
static const Key Down_Disengage_Fall_T = 0x560c34cc;
static const Key Down_Engaging_Fall_RPM = 0x3e1a0db6;
static const Key Down_Engaging_Fall_T = 0x84d1b9a4;
static const Key Down_Engaging_Rise_RPM = 0x8818c1fe;
static const Key Down_Engaging_Rise_T = 0x693ebff3;
static const Key Down_Reattach_Scale = 0xf1065907;
static const Key Down_Shift_Sound_Delay = 0x64e9dc9e;
static const Key Down_Vol_Shift = 0x823fc1c5;
static const Key LFO_RPM_Amp = 0xd1172bd0;
static const Key LFO_RPM_Decay_Time = 0xda0dfcde;
static const Key LFO_RPM_Freq = 0x27d396ac;
static const Key LFO_Vol_Amp = 0x1c9e97c6;
static const Key LFO_Vol_Decay_Time = 0x748eb252;
static const Key LFO_Vol_Freq = 0x0c531256;
static const Key Up_DisengageFall = 0xf2d90101;
static const Key Up_DisengageFall_Curve = 0xf040e6b0;
static const Key Up_Engage = 0xcb89b8c8;
static const Key Up_Engage_Curve = 0x68da6275;
static const Key Up_Engaging_Attack_T = 0x50b23fbc;
static const Key Up_Engaging_Attack_Vol = 0x419c66c9;
static const Key Up_Shift_Sound_Delay = 0xed3007a0;
static const Key Up_Vol_Shift = 0xbdcb972f;

}; // namespace shiftpattern
}; // namespace Hash

inline Key Gen::shiftpattern::ClassKey() {
    return ClassName::shiftpattern;
}

}; // namespace Attrib

#endif
