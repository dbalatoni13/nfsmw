#ifndef _attrib_gen_transmission_h
#define _attrib_gen_transmission_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct transmission : Instance {
    struct _LayoutStruct {
        Private _Array_GEAR_RATIO;                // offset 0x0, size 0x8
        EA::Reflection::Float GEAR_RATIO[9];      // offset 0x8, size 0x24
        Private _Array_DIFFERENTIAL;              // offset 0x2c, size 0x8
        EA::Reflection::Float DIFFERENTIAL[3];    // offset 0x34, size 0xc
        Private _Array_GEAR_EFFICIENCY;           // offset 0x40, size 0x8
        EA::Reflection::Float GEAR_EFFICIENCY[9]; // offset 0x48, size 0x24
        EA::Reflection::Float TORQUE_CONVERTER;   // offset 0x6c, size 0x4
        EA::Reflection::Float TORQUE_SPLIT;       // offset 0x70, size 0x4
        EA::Reflection::Float CLUTCH_SLIP;        // offset 0x74, size 0x4
        EA::Reflection::Float OPTIMAL_SHIFT;      // offset 0x78, size 0x4
        EA::Reflection::Float SHIFT_SPEED;        // offset 0x7c, size 0x4
        EA::Reflection::Float FINAL_GEAR;         // offset 0x80, size 0x4
    };

    typedef EA::Reflection::Float TypeOf_CLUTCH_SLIP;
    typedef EA::Reflection::Float TypeOf_DIFFERENTIAL;
    typedef EA::Reflection::Float TypeOf_FINAL_GEAR;
    typedef EA::Reflection::Float TypeOf_GEAR_EFFICIENCY;
    typedef EA::Reflection::Float TypeOf_GEAR_RATIO;
    typedef EA::Reflection::Float TypeOf_OPTIMAL_SHIFT;
    typedef EA::Reflection::Float TypeOf_SHIFT_SPEED;
    typedef EA::Reflection::Float TypeOf_TORQUE_CONVERTER;
    typedef EA::Reflection::Float TypeOf_TORQUE_SPLIT;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("transmission");
    transmission(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    transmission(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    transmission(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    transmission(const transmission &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    transmission(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~transmission() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0x07a7a3e5;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0x07a7a3e5, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const transmission &operator=(const transmission &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const transmission &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool CLUTCH_SLIP(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xcf5dd4e9);
    }
    bool CLUTCH_SLIP(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(CLUTCH_SLIP, result);
    }
    const EA::Reflection::Float &CLUTCH_SLIP() const {
        ATTRIB_CODEGEN_GETLAYOUT(CLUTCH_SLIP);
    }
    bool SET_CLUTCH_SLIP(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(CLUTCH_SLIP, input);
    }
    bool DIFFERENTIAL(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x30b5e627);
    }
    bool DIFFERENTIAL(EA::Reflection::Float &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(DIFFERENTIAL, result, index);
    }
    const EA::Reflection::Float &DIFFERENTIAL(unsigned int index) const {
        ATTRIB_CODEGEN_GETLAYOUTINDEXED(EA::Reflection::Float, DIFFERENTIAL, index);
    }
    unsigned int Num_DIFFERENTIAL() const {
        ATTRIB_CODEGEN_GETLAYOUTLENGTH(DIFFERENTIAL);
    }
    bool SET_DIFFERENTIAL(const EA::Reflection::Float &input, unsigned int index) {
        ATTRIB_CODEGEN_SETLAYOUTINDEXED(DIFFERENTIAL, input, index);
    }
    bool FINAL_GEAR(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xddb32b64);
    }
    bool FINAL_GEAR(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(FINAL_GEAR, result);
    }
    const EA::Reflection::Float &FINAL_GEAR() const {
        ATTRIB_CODEGEN_GETLAYOUT(FINAL_GEAR);
    }
    bool SET_FINAL_GEAR(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(FINAL_GEAR, input);
    }
    bool GEAR_EFFICIENCY(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x5250eb92);
    }
    bool GEAR_EFFICIENCY(EA::Reflection::Float &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(GEAR_EFFICIENCY, result, index);
    }
    const EA::Reflection::Float &GEAR_EFFICIENCY(unsigned int index) const {
        ATTRIB_CODEGEN_GETLAYOUTINDEXED(EA::Reflection::Float, GEAR_EFFICIENCY, index);
    }
    unsigned int Num_GEAR_EFFICIENCY() const {
        ATTRIB_CODEGEN_GETLAYOUTLENGTH(GEAR_EFFICIENCY);
    }
    bool SET_GEAR_EFFICIENCY(const EA::Reflection::Float &input, unsigned int index) {
        ATTRIB_CODEGEN_SETLAYOUTINDEXED(GEAR_EFFICIENCY, input, index);
    }
    bool GEAR_RATIO(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x17144a84);
    }
    bool GEAR_RATIO(EA::Reflection::Float &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(GEAR_RATIO, result, index);
    }
    const EA::Reflection::Float &GEAR_RATIO(unsigned int index) const {
        ATTRIB_CODEGEN_GETLAYOUTINDEXED(EA::Reflection::Float, GEAR_RATIO, index);
    }
    unsigned int Num_GEAR_RATIO() const {
        ATTRIB_CODEGEN_GETLAYOUTLENGTH(GEAR_RATIO);
    }
    bool SET_GEAR_RATIO(const EA::Reflection::Float &input, unsigned int index) {
        ATTRIB_CODEGEN_SETLAYOUTINDEXED(GEAR_RATIO, input, index);
    }
    bool OPTIMAL_SHIFT(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x8dc16371);
    }
    bool OPTIMAL_SHIFT(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(OPTIMAL_SHIFT, result);
    }
    const EA::Reflection::Float &OPTIMAL_SHIFT() const {
        ATTRIB_CODEGEN_GETLAYOUT(OPTIMAL_SHIFT);
    }
    bool SET_OPTIMAL_SHIFT(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(OPTIMAL_SHIFT, input);
    }
    bool SHIFT_SPEED(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xdd806309);
    }
    bool SHIFT_SPEED(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(SHIFT_SPEED, result);
    }
    const EA::Reflection::Float &SHIFT_SPEED() const {
        ATTRIB_CODEGEN_GETLAYOUT(SHIFT_SPEED);
    }
    bool SET_SHIFT_SPEED(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(SHIFT_SPEED, input);
    }
    bool TORQUE_CONVERTER(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x53c717ec);
    }
    bool TORQUE_CONVERTER(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(TORQUE_CONVERTER, result);
    }
    const EA::Reflection::Float &TORQUE_CONVERTER() const {
        ATTRIB_CODEGEN_GETLAYOUT(TORQUE_CONVERTER);
    }
    bool SET_TORQUE_CONVERTER(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(TORQUE_CONVERTER, input);
    }
    bool TORQUE_SPLIT(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x7381ee38);
    }
    bool TORQUE_SPLIT(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(TORQUE_SPLIT, result);
    }
    const EA::Reflection::Float &TORQUE_SPLIT() const {
        ATTRIB_CODEGEN_GETLAYOUT(TORQUE_SPLIT);
    }
    bool SET_TORQUE_SPLIT(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(TORQUE_SPLIT, input);
    }

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    transmission &ConvertFromInstance(Instance &src) {}
    const transmission &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key transmission = 0x07a7a3e5;

}; // namespace ClassName

namespace Hash {
namespace transmission {

static const Key CLUTCH_SLIP = 0xcf5dd4e9;
static const Key DIFFERENTIAL = 0x30b5e627;
static const Key FINAL_GEAR = 0xddb32b64;
static const Key GEAR_EFFICIENCY = 0x5250eb92;
static const Key GEAR_RATIO = 0x17144a84;
static const Key OPTIMAL_SHIFT = 0x8dc16371;
static const Key SHIFT_SPEED = 0xdd806309;
static const Key TORQUE_CONVERTER = 0x53c717ec;
static const Key TORQUE_SPLIT = 0x7381ee38;

}; // namespace transmission
}; // namespace Hash

inline Key Gen::transmission::ClassKey() {
    return ClassName::transmission;
}

}; // namespace Attrib

#endif
