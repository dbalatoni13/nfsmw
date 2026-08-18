#ifndef _attrib_gen_frontend_h
#define _attrib_gen_frontend_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct frontend : Instance {
    struct _LayoutStruct {
        RefSpec cam_angle;                          // offset 0x0, size 0xc
        RefSpec default_car;                        // offset 0xc, size 0xc
        EA::Reflection::Text CollectionName;        // offset 0x18, size 0x4
        EA::Reflection::Float cam_damping;          // offset 0x1c, size 0x4
        EA::Reflection::Float cam_anim_speed;       // offset 0x20, size 0x4
        EA::Reflection::Float cam_fov;              // offset 0x24, size 0x4
        EA::Reflection::Float cam_blur;             // offset 0x28, size 0x4
        EA::Reflection::Float cam_lookat_x;         // offset 0x2c, size 0x4
        EA::Reflection::Float cam_lookat_y;         // offset 0x30, size 0x4
        EA::Reflection::Float cam_lookat_z;         // offset 0x34, size 0x4
        EA::Reflection::UInt32 region;              // offset 0x38, size 0x4
        EA::Reflection::Float cam_orbit_vertical;   // offset 0x3c, size 0x4
        EA::Reflection::UInt32 manufacturer;        // offset 0x40, size 0x4
        EA::Reflection::Float cam_roll_angle;       // offset 0x44, size 0x4
        EA::Reflection::Int32 cam_periods;          // offset 0x48, size 0x4
        EA::Reflection::Int32 Cost;                 // offset 0x4c, size 0x4
        EA::Reflection::Float cam_orbit_radius;     // offset 0x50, size 0x4
        EA::Reflection::Float cam_orbit_horizontal; // offset 0x54, size 0x4
        EA::Reflection::Bool IsCustomizable;        // offset 0x58, size 0x1
        EA::Reflection::UInt8 UnlockedAt;           // offset 0x59, size 0x1
        EA::Reflection::Bool cam_user_rotate;       // offset 0x5a, size 0x1
    };

    typedef eUnlockableEntity TypeOf_CarPartName;
    typedef EA::Reflection::Text TypeOf_CollectionName;
    typedef EA::Reflection::Int32 TypeOf_Cost;
    typedef EA::Reflection::Bool TypeOf_IsCustomizable;
    typedef EA::Reflection::Int8 TypeOf_NameId;
    typedef EA::Reflection::Float TypeOf_RapSheetRanks;
    typedef EA::Reflection::UInt8 TypeOf_UnlockedAt;
    typedef EA::Reflection::UInt32 TypeOf_WarningLevel;
    typedef RefSpec TypeOf_cam_angle;
    typedef EA::Reflection::Float TypeOf_cam_anim_speed;
    typedef EA::Reflection::Float TypeOf_cam_blur;
    typedef EA::Reflection::Float TypeOf_cam_damping;
    typedef EA::Reflection::Float TypeOf_cam_fov;
    typedef EA::Reflection::Float TypeOf_cam_lookat_x;
    typedef EA::Reflection::Float TypeOf_cam_lookat_y;
    typedef EA::Reflection::Float TypeOf_cam_lookat_z;
    typedef EA::Reflection::Float TypeOf_cam_orbit_horizontal;
    typedef EA::Reflection::Float TypeOf_cam_orbit_radius;
    typedef EA::Reflection::Float TypeOf_cam_orbit_vertical;
    typedef EA::Reflection::Int32 TypeOf_cam_periods;
    typedef EA::Reflection::Float TypeOf_cam_roll_angle;
    typedef EA::Reflection::Bool TypeOf_cam_user_rotate;
    typedef RefSpec TypeOf_default_car;
    typedef FECarPartInfo TypeOf_feCarPartInfo;
    typedef eUnlockableEntity TypeOf_feCarPartName;
    typedef EA::Reflection::UInt32 TypeOf_manufacturer;
    typedef type_bStringHash TypeOf_p_brakes_1;
    typedef type_bStringHash TypeOf_p_brakes_2;
    typedef type_bStringHash TypeOf_p_brakes_3;
    typedef type_bStringHash TypeOf_p_brakes_4;
    typedef type_bStringHash TypeOf_p_engine_1;
    typedef type_bStringHash TypeOf_p_engine_2;
    typedef type_bStringHash TypeOf_p_engine_3;
    typedef type_bStringHash TypeOf_p_engine_4;
    typedef type_bStringHash TypeOf_p_induction_1;
    typedef type_bStringHash TypeOf_p_induction_2;
    typedef type_bStringHash TypeOf_p_induction_3;
    typedef type_bStringHash TypeOf_p_induction_4;
    typedef type_bStringHash TypeOf_p_nitrous_1;
    typedef type_bStringHash TypeOf_p_nitrous_2;
    typedef type_bStringHash TypeOf_p_nitrous_3;
    typedef type_bStringHash TypeOf_p_nitrous_4;
    typedef type_bStringHash TypeOf_p_suspension_1;
    typedef type_bStringHash TypeOf_p_suspension_2;
    typedef type_bStringHash TypeOf_p_suspension_3;
    typedef type_bStringHash TypeOf_p_suspension_4;
    typedef type_bStringHash TypeOf_p_tires_1;
    typedef type_bStringHash TypeOf_p_tires_2;
    typedef type_bStringHash TypeOf_p_tires_3;
    typedef type_bStringHash TypeOf_p_tires_4;
    typedef type_bStringHash TypeOf_p_transmission_1;
    typedef type_bStringHash TypeOf_p_transmission_2;
    typedef type_bStringHash TypeOf_p_transmission_3;
    typedef type_bStringHash TypeOf_p_transmission_4;
    typedef EA::Reflection::UInt32 TypeOf_region;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("frontend");
    frontend(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    frontend(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    frontend(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    frontend(const frontend &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    frontend(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~frontend() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0x85885722;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0x85885722, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const frontend &operator=(const frontend &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const frontend &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool CarPartName(TAttrib<eUnlockableEntity> &result) const;
    bool CarPartName(eUnlockableEntity &result) const;
    const eUnlockableEntity &CarPartName() const;
    bool SET_CarPartName(const eUnlockableEntity &input);

    bool CollectionName(TAttrib<EA::Reflection::Text> &result) const;
    bool CollectionName(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &CollectionName() const;
    bool SET_CollectionName(const EA::Reflection::Text &input);

    bool Cost(TAttrib<EA::Reflection::Int32> &result) const;
    bool Cost(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &Cost() const;
    bool SET_Cost(const EA::Reflection::Int32 &input);

    bool IsCustomizable(TAttrib<EA::Reflection::Bool> &result) const;
    bool IsCustomizable(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &IsCustomizable() const;
    bool SET_IsCustomizable(const EA::Reflection::Bool &input);

    bool NameId(TAttrib<EA::Reflection::Int8> &result) const;
    bool NameId(EA::Reflection::Int8 &result, unsigned int index) const;
    const EA::Reflection::Int8 &NameId(unsigned int index) const;
    unsigned int Num_NameId() const;
    bool SET_NameId(const EA::Reflection::Int8 &input, unsigned int index);

    bool RapSheetRanks(TAttrib<EA::Reflection::Float> &result) const;
    bool RapSheetRanks(EA::Reflection::Float &result, unsigned int index) const;
    const EA::Reflection::Float &RapSheetRanks(unsigned int index) const;
    unsigned int Num_RapSheetRanks() const;
    bool SET_RapSheetRanks(const EA::Reflection::Float &input, unsigned int index);

    bool UnlockedAt(TAttrib<EA::Reflection::UInt8> &result) const;
    bool UnlockedAt(EA::Reflection::UInt8 &result) const;
    const EA::Reflection::UInt8 &UnlockedAt() const;
    bool SET_UnlockedAt(const EA::Reflection::UInt8 &input);

    bool WarningLevel(TAttrib<EA::Reflection::UInt32> &result) const;
    bool WarningLevel(EA::Reflection::UInt32 &result, unsigned int index) const;
    const EA::Reflection::UInt32 &WarningLevel(unsigned int index) const;
    unsigned int Num_WarningLevel() const;
    bool SET_WarningLevel(const EA::Reflection::UInt32 &input, unsigned int index);

    bool cam_angle(RefSpec &result) const;
    const RefSpec &cam_angle() const;
    bool SET_cam_angle(const RefSpec &input);

    bool cam_anim_speed(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &cam_anim_speed() const;
    bool SET_cam_anim_speed(const EA::Reflection::Float &input);

    bool cam_blur(TAttrib<EA::Reflection::Float> &result) const;
    bool cam_blur(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &cam_blur() const;
    bool SET_cam_blur(const EA::Reflection::Float &input);

    bool cam_damping(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &cam_damping() const;
    bool SET_cam_damping(const EA::Reflection::Float &input);

    bool cam_fov(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &cam_fov() const;
    bool SET_cam_fov(const EA::Reflection::Float &input);

    bool cam_lookat_x(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &cam_lookat_x() const;
    bool SET_cam_lookat_x(const EA::Reflection::Float &input);

    bool cam_lookat_y(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &cam_lookat_y() const;
    bool SET_cam_lookat_y(const EA::Reflection::Float &input);

    bool cam_lookat_z(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &cam_lookat_z() const;
    bool SET_cam_lookat_z(const EA::Reflection::Float &input);

    bool cam_orbit_horizontal(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &cam_orbit_horizontal() const;
    bool SET_cam_orbit_horizontal(const EA::Reflection::Float &input);

    bool cam_orbit_radius(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &cam_orbit_radius() const;
    bool SET_cam_orbit_radius(const EA::Reflection::Float &input);

    bool cam_orbit_vertical(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &cam_orbit_vertical() const;
    bool SET_cam_orbit_vertical(const EA::Reflection::Float &input);

    bool cam_periods(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &cam_periods() const;
    bool SET_cam_periods(const EA::Reflection::Int32 &input);

    bool cam_roll_angle(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &cam_roll_angle() const;
    bool SET_cam_roll_angle(const EA::Reflection::Float &input);

    bool cam_user_rotate(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &cam_user_rotate() const;
    bool SET_cam_user_rotate(const EA::Reflection::Bool &input);

    bool default_car(RefSpec &result) const;
    const RefSpec &default_car() const;
    bool SET_default_car(const RefSpec &input);

    bool feCarPartInfo(TAttrib<FECarPartInfo> &result) const;
    bool feCarPartInfo(FECarPartInfo &result, unsigned int index) const;
    const FECarPartInfo &feCarPartInfo(unsigned int index) const;
    unsigned int Num_feCarPartInfo() const;
    bool SET_feCarPartInfo(const FECarPartInfo &input, unsigned int index);

    bool feCarPartName(TAttrib<eUnlockableEntity> &result) const;
    bool feCarPartName(eUnlockableEntity &result) const;
    const eUnlockableEntity &feCarPartName() const;
    bool SET_feCarPartName(const eUnlockableEntity &input);

    bool manufacturer(EA::Reflection::UInt32 &result) const;
    const EA::Reflection::UInt32 &manufacturer() const;
    bool SET_manufacturer(const EA::Reflection::UInt32 &input);

    bool p_brakes_1(TAttrib<type_bStringHash> &result) const;
    bool p_brakes_1(type_bStringHash &result, unsigned int index) const;
    const type_bStringHash &p_brakes_1(unsigned int index) const;
    unsigned int Num_p_brakes_1() const;
    bool SET_p_brakes_1(const type_bStringHash &input, unsigned int index);

    bool p_brakes_2(TAttrib<type_bStringHash> &result) const;
    bool p_brakes_2(type_bStringHash &result, unsigned int index) const;
    const type_bStringHash &p_brakes_2(unsigned int index) const;
    unsigned int Num_p_brakes_2() const;
    bool SET_p_brakes_2(const type_bStringHash &input, unsigned int index);

    bool p_brakes_3(TAttrib<type_bStringHash> &result) const;
    bool p_brakes_3(type_bStringHash &result, unsigned int index) const;
    const type_bStringHash &p_brakes_3(unsigned int index) const;
    unsigned int Num_p_brakes_3() const;
    bool SET_p_brakes_3(const type_bStringHash &input, unsigned int index);

    bool p_brakes_4(TAttrib<type_bStringHash> &result) const;
    bool p_brakes_4(type_bStringHash &result, unsigned int index) const;
    const type_bStringHash &p_brakes_4(unsigned int index) const;
    unsigned int Num_p_brakes_4() const;
    bool SET_p_brakes_4(const type_bStringHash &input, unsigned int index);

    bool p_engine_1(TAttrib<type_bStringHash> &result) const;
    bool p_engine_1(type_bStringHash &result, unsigned int index) const;
    const type_bStringHash &p_engine_1(unsigned int index) const;
    unsigned int Num_p_engine_1() const;
    bool SET_p_engine_1(const type_bStringHash &input, unsigned int index);

    bool p_engine_2(TAttrib<type_bStringHash> &result) const;
    bool p_engine_2(type_bStringHash &result, unsigned int index) const;
    const type_bStringHash &p_engine_2(unsigned int index) const;
    unsigned int Num_p_engine_2() const;
    bool SET_p_engine_2(const type_bStringHash &input, unsigned int index);

    bool p_engine_3(TAttrib<type_bStringHash> &result) const;
    bool p_engine_3(type_bStringHash &result, unsigned int index) const;
    const type_bStringHash &p_engine_3(unsigned int index) const;
    unsigned int Num_p_engine_3() const;
    bool SET_p_engine_3(const type_bStringHash &input, unsigned int index);

    bool p_engine_4(TAttrib<type_bStringHash> &result) const;
    bool p_engine_4(type_bStringHash &result, unsigned int index) const;
    const type_bStringHash &p_engine_4(unsigned int index) const;
    unsigned int Num_p_engine_4() const;
    bool SET_p_engine_4(const type_bStringHash &input, unsigned int index);

    bool p_induction_1(TAttrib<type_bStringHash> &result) const;
    bool p_induction_1(type_bStringHash &result, unsigned int index) const;
    const type_bStringHash &p_induction_1(unsigned int index) const;
    unsigned int Num_p_induction_1() const;
    bool SET_p_induction_1(const type_bStringHash &input, unsigned int index);

    bool p_induction_2(TAttrib<type_bStringHash> &result) const;
    bool p_induction_2(type_bStringHash &result, unsigned int index) const;
    const type_bStringHash &p_induction_2(unsigned int index) const;
    unsigned int Num_p_induction_2() const;
    bool SET_p_induction_2(const type_bStringHash &input, unsigned int index);

    bool p_induction_3(TAttrib<type_bStringHash> &result) const;
    bool p_induction_3(type_bStringHash &result, unsigned int index) const;
    const type_bStringHash &p_induction_3(unsigned int index) const;
    unsigned int Num_p_induction_3() const;
    bool SET_p_induction_3(const type_bStringHash &input, unsigned int index);

    bool p_induction_4(TAttrib<type_bStringHash> &result) const;
    bool p_induction_4(type_bStringHash &result, unsigned int index) const;
    const type_bStringHash &p_induction_4(unsigned int index) const;
    unsigned int Num_p_induction_4() const;
    bool SET_p_induction_4(const type_bStringHash &input, unsigned int index);

    bool p_nitrous_1(TAttrib<type_bStringHash> &result) const;
    bool p_nitrous_1(type_bStringHash &result, unsigned int index) const;
    const type_bStringHash &p_nitrous_1(unsigned int index) const;
    unsigned int Num_p_nitrous_1() const;
    bool SET_p_nitrous_1(const type_bStringHash &input, unsigned int index);

    bool p_nitrous_2(TAttrib<type_bStringHash> &result) const;
    bool p_nitrous_2(type_bStringHash &result, unsigned int index) const;
    const type_bStringHash &p_nitrous_2(unsigned int index) const;
    unsigned int Num_p_nitrous_2() const;
    bool SET_p_nitrous_2(const type_bStringHash &input, unsigned int index);

    bool p_nitrous_3(TAttrib<type_bStringHash> &result) const;
    bool p_nitrous_3(type_bStringHash &result, unsigned int index) const;
    const type_bStringHash &p_nitrous_3(unsigned int index) const;
    unsigned int Num_p_nitrous_3() const;
    bool SET_p_nitrous_3(const type_bStringHash &input, unsigned int index);

    bool p_nitrous_4(TAttrib<type_bStringHash> &result) const;
    bool p_nitrous_4(type_bStringHash &result, unsigned int index) const;
    const type_bStringHash &p_nitrous_4(unsigned int index) const;
    unsigned int Num_p_nitrous_4() const;
    bool SET_p_nitrous_4(const type_bStringHash &input, unsigned int index);

    bool p_suspension_1(TAttrib<type_bStringHash> &result) const;
    bool p_suspension_1(type_bStringHash &result, unsigned int index) const;
    const type_bStringHash &p_suspension_1(unsigned int index) const;
    unsigned int Num_p_suspension_1() const;
    bool SET_p_suspension_1(const type_bStringHash &input, unsigned int index);

    bool p_suspension_2(TAttrib<type_bStringHash> &result) const;
    bool p_suspension_2(type_bStringHash &result, unsigned int index) const;
    const type_bStringHash &p_suspension_2(unsigned int index) const;
    unsigned int Num_p_suspension_2() const;
    bool SET_p_suspension_2(const type_bStringHash &input, unsigned int index);

    bool p_suspension_3(TAttrib<type_bStringHash> &result) const;
    bool p_suspension_3(type_bStringHash &result, unsigned int index) const;
    const type_bStringHash &p_suspension_3(unsigned int index) const;
    unsigned int Num_p_suspension_3() const;
    bool SET_p_suspension_3(const type_bStringHash &input, unsigned int index);

    bool p_suspension_4(TAttrib<type_bStringHash> &result) const;
    bool p_suspension_4(type_bStringHash &result, unsigned int index) const;
    const type_bStringHash &p_suspension_4(unsigned int index) const;
    unsigned int Num_p_suspension_4() const;
    bool SET_p_suspension_4(const type_bStringHash &input, unsigned int index);

    bool p_tires_1(TAttrib<type_bStringHash> &result) const;
    bool p_tires_1(type_bStringHash &result, unsigned int index) const;
    const type_bStringHash &p_tires_1(unsigned int index) const;
    unsigned int Num_p_tires_1() const;
    bool SET_p_tires_1(const type_bStringHash &input, unsigned int index);

    bool p_tires_2(TAttrib<type_bStringHash> &result) const;
    bool p_tires_2(type_bStringHash &result, unsigned int index) const;
    const type_bStringHash &p_tires_2(unsigned int index) const;
    unsigned int Num_p_tires_2() const;
    bool SET_p_tires_2(const type_bStringHash &input, unsigned int index);

    bool p_tires_3(TAttrib<type_bStringHash> &result) const;
    bool p_tires_3(type_bStringHash &result, unsigned int index) const;
    const type_bStringHash &p_tires_3(unsigned int index) const;
    unsigned int Num_p_tires_3() const;
    bool SET_p_tires_3(const type_bStringHash &input, unsigned int index);

    bool p_tires_4(TAttrib<type_bStringHash> &result) const;
    bool p_tires_4(type_bStringHash &result, unsigned int index) const;
    const type_bStringHash &p_tires_4(unsigned int index) const;
    unsigned int Num_p_tires_4() const;
    bool SET_p_tires_4(const type_bStringHash &input, unsigned int index);

    bool p_transmission_1(TAttrib<type_bStringHash> &result) const;
    bool p_transmission_1(type_bStringHash &result, unsigned int index) const;
    const type_bStringHash &p_transmission_1(unsigned int index) const;
    unsigned int Num_p_transmission_1() const;
    bool SET_p_transmission_1(const type_bStringHash &input, unsigned int index);

    bool p_transmission_2(TAttrib<type_bStringHash> &result) const;
    bool p_transmission_2(type_bStringHash &result, unsigned int index) const;
    const type_bStringHash &p_transmission_2(unsigned int index) const;
    unsigned int Num_p_transmission_2() const;
    bool SET_p_transmission_2(const type_bStringHash &input, unsigned int index);

    bool p_transmission_3(TAttrib<type_bStringHash> &result) const;
    bool p_transmission_3(type_bStringHash &result, unsigned int index) const;
    const type_bStringHash &p_transmission_3(unsigned int index) const;
    unsigned int Num_p_transmission_3() const;
    bool SET_p_transmission_3(const type_bStringHash &input, unsigned int index);

    bool p_transmission_4(TAttrib<type_bStringHash> &result) const;
    bool p_transmission_4(type_bStringHash &result, unsigned int index) const;
    const type_bStringHash &p_transmission_4(unsigned int index) const;
    unsigned int Num_p_transmission_4() const;
    bool SET_p_transmission_4(const type_bStringHash &input, unsigned int index);

    bool region(EA::Reflection::UInt32 &result) const;
    const EA::Reflection::UInt32 &region() const;
    bool SET_region(const EA::Reflection::UInt32 &input);

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    frontend &ConvertFromInstance(Instance &src) {}
    const frontend &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key frontend = 0x85885722;

}; // namespace ClassName

namespace Hash {
namespace frontend {

static const Key CarPartName = 0xb668787d;
static const Key CollectionName = 0x9ca1c8f9;
static const Key Cost = 0xe61673e9;
static const Key IsCustomizable = 0xfbcf1a48;
static const Key NameId = 0x2c3c7feb;
static const Key RapSheetRanks = 0xf9a7d5f7;
static const Key UnlockedAt = 0x60f99f57;
static const Key WarningLevel = 0xb54d4a41;
static const Key cam_angle = 0x28b5440b;
static const Key cam_anim_speed = 0xf624544d;
static const Key cam_blur = 0xf35f7b1f;
static const Key cam_damping = 0x2581b8ba;
static const Key cam_fov = 0xc220d5ab;
static const Key cam_lookat_x = 0xa8d9a1f4;
static const Key cam_lookat_y = 0xfe9b5900;
static const Key cam_lookat_z = 0x7c9b89d3;
static const Key cam_orbit_horizontal = 0x803fa14c;
static const Key cam_orbit_radius = 0xa10c312c;
static const Key cam_orbit_vertical = 0xc8f96e5a;
static const Key cam_periods = 0x798624ca;
static const Key cam_roll_angle = 0x05b56526;
static const Key cam_user_rotate = 0x998dd0db;
static const Key default_car = 0x851874eb;
static const Key feCarPartInfo = 0x1002ea65;
static const Key feCarPartName = 0x3b874697;
static const Key manufacturer = 0xbaf4ab4c;
static const Key p_brakes_1 = 0xe4af1260;
static const Key p_brakes_2 = 0x70b14851;
static const Key p_brakes_3 = 0x8e8b78e1;
static const Key p_brakes_4 = 0xb4df5439;
static const Key p_engine_1 = 0x512303af;
static const Key p_engine_2 = 0xdb8a8a1d;
static const Key p_engine_3 = 0x4f56a655;
static const Key p_engine_4 = 0x85ab21da;
static const Key p_induction_1 = 0x0e141cde;
static const Key p_induction_2 = 0x4d3b62f3;
static const Key p_induction_3 = 0xea7f3fe4;
static const Key p_induction_4 = 0xb6be1d52;
static const Key p_nitrous_1 = 0x7f6e85a3;
static const Key p_nitrous_2 = 0xd810d2dc;
static const Key p_nitrous_3 = 0xa459ecef;
static const Key p_nitrous_4 = 0x8da087a4;
static const Key p_suspension_1 = 0x37ea2169;
static const Key p_suspension_2 = 0xe5650914;
static const Key p_suspension_3 = 0xe321687d;
static const Key p_suspension_4 = 0xfb1ef23f;
static const Key p_tires_1 = 0xf0c7c400;
static const Key p_tires_2 = 0x01e6ddf1;
static const Key p_tires_3 = 0x92378a0a;
static const Key p_tires_4 = 0x16b700d6;
static const Key p_transmission_1 = 0x1e823f0b;
static const Key p_transmission_2 = 0x79c8d7e9;
static const Key p_transmission_3 = 0xa1b53a33;
static const Key p_transmission_4 = 0xf424c06d;
static const Key region = 0x0de009be;

}; // namespace frontend
}; // namespace Hash

inline Key Gen::frontend::ClassKey() {
    return ClassName::frontend;
}

inline bool Gen::frontend::CarPartName(TAttrib<eUnlockableEntity> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(eUnlockableEntity, 0xb668787d);
}
inline const eUnlockableEntity &Gen::frontend::CarPartName() const {
    ATTRIB_CODEGEN_GETVALUE(eUnlockableEntity, 0xb668787d);
}
inline bool Gen::frontend::CarPartName(eUnlockableEntity &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(eUnlockableEntity, 0xb668787d, result);
}
inline bool Gen::frontend::SET_CarPartName(const eUnlockableEntity &input) {
    ATTRIB_CODEGEN_SETVALUE(eUnlockableEntity, 0xb668787d, input);
}

inline bool Gen::frontend::CollectionName(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x9ca1c8f9);
}
inline const EA::Reflection::Text &Gen::frontend::CollectionName() const {
    ATTRIB_CODEGEN_GETLAYOUT(CollectionName);
}
inline bool Gen::frontend::CollectionName(EA::Reflection::Text &result) const {
    result = CollectionName();
    return true;
}
inline bool Gen::frontend::SET_CollectionName(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETLAYOUT(CollectionName, input);
}

inline bool Gen::frontend::Cost(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xe61673e9);
}
inline const EA::Reflection::Int32 &Gen::frontend::Cost() const {
    ATTRIB_CODEGEN_GETLAYOUT(Cost);
}
inline bool Gen::frontend::Cost(EA::Reflection::Int32 &result) const {
    result = Cost();
    return true;
}
inline bool Gen::frontend::SET_Cost(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(Cost, input);
}

inline bool Gen::frontend::IsCustomizable(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xfbcf1a48);
}
inline const EA::Reflection::Bool &Gen::frontend::IsCustomizable() const {
    ATTRIB_CODEGEN_GETLAYOUT(IsCustomizable);
}
inline bool Gen::frontend::IsCustomizable(EA::Reflection::Bool &result) const {
    result = IsCustomizable();
    return true;
}
inline bool Gen::frontend::SET_IsCustomizable(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETLAYOUT(IsCustomizable, input);
}

inline bool Gen::frontend::NameId(TAttrib<EA::Reflection::Int8> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int8, 0x2c3c7feb);
}
inline const EA::Reflection::Int8 &Gen::frontend::NameId(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(EA::Reflection::Int8, 0x2c3c7feb, index);
}
inline bool Gen::frontend::NameId(EA::Reflection::Int8 &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(EA::Reflection::Int8, 0x2c3c7feb, result, index);
}
inline unsigned int Gen::frontend::Num_NameId() const {
    ATTRIB_CODEGEN_GETLENGTH(0x2c3c7feb);
}
inline bool Gen::frontend::SET_NameId(const EA::Reflection::Int8 &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(EA::Reflection::Int8, 0x2c3c7feb, input, index);
}

inline bool Gen::frontend::RapSheetRanks(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xf9a7d5f7);
}
inline const EA::Reflection::Float &Gen::frontend::RapSheetRanks(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(EA::Reflection::Float, 0xf9a7d5f7, index);
}
inline bool Gen::frontend::RapSheetRanks(EA::Reflection::Float &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(EA::Reflection::Float, 0xf9a7d5f7, result, index);
}
inline unsigned int Gen::frontend::Num_RapSheetRanks() const {
    ATTRIB_CODEGEN_GETLENGTH(0xf9a7d5f7);
}
inline bool Gen::frontend::SET_RapSheetRanks(const EA::Reflection::Float &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(EA::Reflection::Float, 0xf9a7d5f7, input, index);
}

inline bool Gen::frontend::UnlockedAt(TAttrib<EA::Reflection::UInt8> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt8, 0x60f99f57);
}
inline const EA::Reflection::UInt8 &Gen::frontend::UnlockedAt() const {
    ATTRIB_CODEGEN_GETLAYOUT(UnlockedAt);
}
inline bool Gen::frontend::UnlockedAt(EA::Reflection::UInt8 &result) const {
    result = UnlockedAt();
    return true;
}
inline bool Gen::frontend::SET_UnlockedAt(const EA::Reflection::UInt8 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(UnlockedAt, input);
}

inline bool Gen::frontend::WarningLevel(TAttrib<EA::Reflection::UInt32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0xb54d4a41);
}
inline const EA::Reflection::UInt32 &Gen::frontend::WarningLevel(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(EA::Reflection::UInt32, 0xb54d4a41, index);
}
inline bool Gen::frontend::WarningLevel(EA::Reflection::UInt32 &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(EA::Reflection::UInt32, 0xb54d4a41, result, index);
}
inline unsigned int Gen::frontend::Num_WarningLevel() const {
    ATTRIB_CODEGEN_GETLENGTH(0xb54d4a41);
}
inline bool Gen::frontend::SET_WarningLevel(const EA::Reflection::UInt32 &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(EA::Reflection::UInt32, 0xb54d4a41, input, index);
}

inline const RefSpec &Gen::frontend::cam_angle() const {
    ATTRIB_CODEGEN_GETLAYOUT(cam_angle);
}
inline bool Gen::frontend::cam_angle(RefSpec &result) const {
    result = cam_angle();
    return true;
}
inline bool Gen::frontend::SET_cam_angle(const RefSpec &input) {
    ATTRIB_CODEGEN_SETLAYOUT(cam_angle, input);
}

inline const EA::Reflection::Float &Gen::frontend::cam_anim_speed() const {
    ATTRIB_CODEGEN_GETLAYOUT(cam_anim_speed);
}
inline bool Gen::frontend::cam_anim_speed(EA::Reflection::Float &result) const {
    result = cam_anim_speed();
    return true;
}
inline bool Gen::frontend::SET_cam_anim_speed(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(cam_anim_speed, input);
}

inline bool Gen::frontend::cam_blur(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xf35f7b1f);
}
inline const EA::Reflection::Float &Gen::frontend::cam_blur() const {
    ATTRIB_CODEGEN_GETLAYOUT(cam_blur);
}
inline bool Gen::frontend::cam_blur(EA::Reflection::Float &result) const {
    result = cam_blur();
    return true;
}
inline bool Gen::frontend::SET_cam_blur(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(cam_blur, input);
}

inline const EA::Reflection::Float &Gen::frontend::cam_damping() const {
    ATTRIB_CODEGEN_GETLAYOUT(cam_damping);
}
inline bool Gen::frontend::cam_damping(EA::Reflection::Float &result) const {
    result = cam_damping();
    return true;
}
inline bool Gen::frontend::SET_cam_damping(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(cam_damping, input);
}

inline const EA::Reflection::Float &Gen::frontend::cam_fov() const {
    ATTRIB_CODEGEN_GETLAYOUT(cam_fov);
}
inline bool Gen::frontend::cam_fov(EA::Reflection::Float &result) const {
    result = cam_fov();
    return true;
}
inline bool Gen::frontend::SET_cam_fov(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(cam_fov, input);
}

inline const EA::Reflection::Float &Gen::frontend::cam_lookat_x() const {
    ATTRIB_CODEGEN_GETLAYOUT(cam_lookat_x);
}
inline bool Gen::frontend::cam_lookat_x(EA::Reflection::Float &result) const {
    result = cam_lookat_x();
    return true;
}
inline bool Gen::frontend::SET_cam_lookat_x(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(cam_lookat_x, input);
}

inline const EA::Reflection::Float &Gen::frontend::cam_lookat_y() const {
    ATTRIB_CODEGEN_GETLAYOUT(cam_lookat_y);
}
inline bool Gen::frontend::cam_lookat_y(EA::Reflection::Float &result) const {
    result = cam_lookat_y();
    return true;
}
inline bool Gen::frontend::SET_cam_lookat_y(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(cam_lookat_y, input);
}

inline const EA::Reflection::Float &Gen::frontend::cam_lookat_z() const {
    ATTRIB_CODEGEN_GETLAYOUT(cam_lookat_z);
}
inline bool Gen::frontend::cam_lookat_z(EA::Reflection::Float &result) const {
    result = cam_lookat_z();
    return true;
}
inline bool Gen::frontend::SET_cam_lookat_z(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(cam_lookat_z, input);
}

inline const EA::Reflection::Float &Gen::frontend::cam_orbit_horizontal() const {
    ATTRIB_CODEGEN_GETLAYOUT(cam_orbit_horizontal);
}
inline bool Gen::frontend::cam_orbit_horizontal(EA::Reflection::Float &result) const {
    result = cam_orbit_horizontal();
    return true;
}
inline bool Gen::frontend::SET_cam_orbit_horizontal(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(cam_orbit_horizontal, input);
}

inline const EA::Reflection::Float &Gen::frontend::cam_orbit_radius() const {
    ATTRIB_CODEGEN_GETLAYOUT(cam_orbit_radius);
}
inline bool Gen::frontend::cam_orbit_radius(EA::Reflection::Float &result) const {
    result = cam_orbit_radius();
    return true;
}
inline bool Gen::frontend::SET_cam_orbit_radius(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(cam_orbit_radius, input);
}

inline const EA::Reflection::Float &Gen::frontend::cam_orbit_vertical() const {
    ATTRIB_CODEGEN_GETLAYOUT(cam_orbit_vertical);
}
inline bool Gen::frontend::cam_orbit_vertical(EA::Reflection::Float &result) const {
    result = cam_orbit_vertical();
    return true;
}
inline bool Gen::frontend::SET_cam_orbit_vertical(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(cam_orbit_vertical, input);
}

inline const EA::Reflection::Int32 &Gen::frontend::cam_periods() const {
    ATTRIB_CODEGEN_GETLAYOUT(cam_periods);
}
inline bool Gen::frontend::cam_periods(EA::Reflection::Int32 &result) const {
    result = cam_periods();
    return true;
}
inline bool Gen::frontend::SET_cam_periods(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(cam_periods, input);
}

inline const EA::Reflection::Float &Gen::frontend::cam_roll_angle() const {
    ATTRIB_CODEGEN_GETLAYOUT(cam_roll_angle);
}
inline bool Gen::frontend::cam_roll_angle(EA::Reflection::Float &result) const {
    result = cam_roll_angle();
    return true;
}
inline bool Gen::frontend::SET_cam_roll_angle(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(cam_roll_angle, input);
}

inline const EA::Reflection::Bool &Gen::frontend::cam_user_rotate() const {
    ATTRIB_CODEGEN_GETLAYOUT(cam_user_rotate);
}
inline bool Gen::frontend::cam_user_rotate(EA::Reflection::Bool &result) const {
    result = cam_user_rotate();
    return true;
}
inline bool Gen::frontend::SET_cam_user_rotate(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETLAYOUT(cam_user_rotate, input);
}

inline const RefSpec &Gen::frontend::default_car() const {
    ATTRIB_CODEGEN_GETLAYOUT(default_car);
}
inline bool Gen::frontend::default_car(RefSpec &result) const {
    result = default_car();
    return true;
}
inline bool Gen::frontend::SET_default_car(const RefSpec &input) {
    ATTRIB_CODEGEN_SETLAYOUT(default_car, input);
}

inline bool Gen::frontend::feCarPartInfo(TAttrib<FECarPartInfo> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(FECarPartInfo, 0x1002ea65);
}
inline const FECarPartInfo &Gen::frontend::feCarPartInfo(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(FECarPartInfo, 0x1002ea65, index);
}
inline bool Gen::frontend::feCarPartInfo(FECarPartInfo &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(FECarPartInfo, 0x1002ea65, result, index);
}
inline unsigned int Gen::frontend::Num_feCarPartInfo() const {
    ATTRIB_CODEGEN_GETLENGTH(0x1002ea65);
}
inline bool Gen::frontend::SET_feCarPartInfo(const FECarPartInfo &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(FECarPartInfo, 0x1002ea65, input, index);
}

inline bool Gen::frontend::feCarPartName(TAttrib<eUnlockableEntity> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(eUnlockableEntity, 0x3b874697);
}
inline const eUnlockableEntity &Gen::frontend::feCarPartName() const {
    ATTRIB_CODEGEN_GETVALUE(eUnlockableEntity, 0x3b874697);
}
inline bool Gen::frontend::feCarPartName(eUnlockableEntity &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(eUnlockableEntity, 0x3b874697, result);
}
inline bool Gen::frontend::SET_feCarPartName(const eUnlockableEntity &input) {
    ATTRIB_CODEGEN_SETVALUE(eUnlockableEntity, 0x3b874697, input);
}

inline const EA::Reflection::UInt32 &Gen::frontend::manufacturer() const {
    ATTRIB_CODEGEN_GETLAYOUT(manufacturer);
}
inline bool Gen::frontend::manufacturer(EA::Reflection::UInt32 &result) const {
    result = manufacturer();
    return true;
}
inline bool Gen::frontend::SET_manufacturer(const EA::Reflection::UInt32 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(manufacturer, input);
}

inline bool Gen::frontend::p_brakes_1(TAttrib<type_bStringHash> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0xe4af1260);
}
inline const type_bStringHash &Gen::frontend::p_brakes_1(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0xe4af1260, index);
}
inline bool Gen::frontend::p_brakes_1(type_bStringHash &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0xe4af1260, result, index);
}
inline unsigned int Gen::frontend::Num_p_brakes_1() const {
    ATTRIB_CODEGEN_GETLENGTH(0xe4af1260);
}
inline bool Gen::frontend::SET_p_brakes_1(const type_bStringHash &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0xe4af1260, input, index);
}

inline bool Gen::frontend::p_brakes_2(TAttrib<type_bStringHash> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0x70b14851);
}
inline const type_bStringHash &Gen::frontend::p_brakes_2(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0x70b14851, index);
}
inline bool Gen::frontend::p_brakes_2(type_bStringHash &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0x70b14851, result, index);
}
inline unsigned int Gen::frontend::Num_p_brakes_2() const {
    ATTRIB_CODEGEN_GETLENGTH(0x70b14851);
}
inline bool Gen::frontend::SET_p_brakes_2(const type_bStringHash &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0x70b14851, input, index);
}

inline bool Gen::frontend::p_brakes_3(TAttrib<type_bStringHash> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0x8e8b78e1);
}
inline const type_bStringHash &Gen::frontend::p_brakes_3(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0x8e8b78e1, index);
}
inline bool Gen::frontend::p_brakes_3(type_bStringHash &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0x8e8b78e1, result, index);
}
inline unsigned int Gen::frontend::Num_p_brakes_3() const {
    ATTRIB_CODEGEN_GETLENGTH(0x8e8b78e1);
}
inline bool Gen::frontend::SET_p_brakes_3(const type_bStringHash &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0x8e8b78e1, input, index);
}

inline bool Gen::frontend::p_brakes_4(TAttrib<type_bStringHash> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0xb4df5439);
}
inline const type_bStringHash &Gen::frontend::p_brakes_4(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0xb4df5439, index);
}
inline bool Gen::frontend::p_brakes_4(type_bStringHash &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0xb4df5439, result, index);
}
inline unsigned int Gen::frontend::Num_p_brakes_4() const {
    ATTRIB_CODEGEN_GETLENGTH(0xb4df5439);
}
inline bool Gen::frontend::SET_p_brakes_4(const type_bStringHash &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0xb4df5439, input, index);
}

inline bool Gen::frontend::p_engine_1(TAttrib<type_bStringHash> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0x512303af);
}
inline const type_bStringHash &Gen::frontend::p_engine_1(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0x512303af, index);
}
inline bool Gen::frontend::p_engine_1(type_bStringHash &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0x512303af, result, index);
}
inline unsigned int Gen::frontend::Num_p_engine_1() const {
    ATTRIB_CODEGEN_GETLENGTH(0x512303af);
}
inline bool Gen::frontend::SET_p_engine_1(const type_bStringHash &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0x512303af, input, index);
}

inline bool Gen::frontend::p_engine_2(TAttrib<type_bStringHash> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0xdb8a8a1d);
}
inline const type_bStringHash &Gen::frontend::p_engine_2(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0xdb8a8a1d, index);
}
inline bool Gen::frontend::p_engine_2(type_bStringHash &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0xdb8a8a1d, result, index);
}
inline unsigned int Gen::frontend::Num_p_engine_2() const {
    ATTRIB_CODEGEN_GETLENGTH(0xdb8a8a1d);
}
inline bool Gen::frontend::SET_p_engine_2(const type_bStringHash &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0xdb8a8a1d, input, index);
}

inline bool Gen::frontend::p_engine_3(TAttrib<type_bStringHash> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0x4f56a655);
}
inline const type_bStringHash &Gen::frontend::p_engine_3(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0x4f56a655, index);
}
inline bool Gen::frontend::p_engine_3(type_bStringHash &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0x4f56a655, result, index);
}
inline unsigned int Gen::frontend::Num_p_engine_3() const {
    ATTRIB_CODEGEN_GETLENGTH(0x4f56a655);
}
inline bool Gen::frontend::SET_p_engine_3(const type_bStringHash &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0x4f56a655, input, index);
}

inline bool Gen::frontend::p_engine_4(TAttrib<type_bStringHash> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0x85ab21da);
}
inline const type_bStringHash &Gen::frontend::p_engine_4(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0x85ab21da, index);
}
inline bool Gen::frontend::p_engine_4(type_bStringHash &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0x85ab21da, result, index);
}
inline unsigned int Gen::frontend::Num_p_engine_4() const {
    ATTRIB_CODEGEN_GETLENGTH(0x85ab21da);
}
inline bool Gen::frontend::SET_p_engine_4(const type_bStringHash &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0x85ab21da, input, index);
}

inline bool Gen::frontend::p_induction_1(TAttrib<type_bStringHash> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0x0e141cde);
}
inline const type_bStringHash &Gen::frontend::p_induction_1(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0x0e141cde, index);
}
inline bool Gen::frontend::p_induction_1(type_bStringHash &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0x0e141cde, result, index);
}
inline unsigned int Gen::frontend::Num_p_induction_1() const {
    ATTRIB_CODEGEN_GETLENGTH(0x0e141cde);
}
inline bool Gen::frontend::SET_p_induction_1(const type_bStringHash &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0x0e141cde, input, index);
}

inline bool Gen::frontend::p_induction_2(TAttrib<type_bStringHash> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0x4d3b62f3);
}
inline const type_bStringHash &Gen::frontend::p_induction_2(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0x4d3b62f3, index);
}
inline bool Gen::frontend::p_induction_2(type_bStringHash &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0x4d3b62f3, result, index);
}
inline unsigned int Gen::frontend::Num_p_induction_2() const {
    ATTRIB_CODEGEN_GETLENGTH(0x4d3b62f3);
}
inline bool Gen::frontend::SET_p_induction_2(const type_bStringHash &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0x4d3b62f3, input, index);
}

inline bool Gen::frontend::p_induction_3(TAttrib<type_bStringHash> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0xea7f3fe4);
}
inline const type_bStringHash &Gen::frontend::p_induction_3(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0xea7f3fe4, index);
}
inline bool Gen::frontend::p_induction_3(type_bStringHash &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0xea7f3fe4, result, index);
}
inline unsigned int Gen::frontend::Num_p_induction_3() const {
    ATTRIB_CODEGEN_GETLENGTH(0xea7f3fe4);
}
inline bool Gen::frontend::SET_p_induction_3(const type_bStringHash &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0xea7f3fe4, input, index);
}

inline bool Gen::frontend::p_induction_4(TAttrib<type_bStringHash> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0xb6be1d52);
}
inline const type_bStringHash &Gen::frontend::p_induction_4(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0xb6be1d52, index);
}
inline bool Gen::frontend::p_induction_4(type_bStringHash &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0xb6be1d52, result, index);
}
inline unsigned int Gen::frontend::Num_p_induction_4() const {
    ATTRIB_CODEGEN_GETLENGTH(0xb6be1d52);
}
inline bool Gen::frontend::SET_p_induction_4(const type_bStringHash &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0xb6be1d52, input, index);
}

inline bool Gen::frontend::p_nitrous_1(TAttrib<type_bStringHash> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0x7f6e85a3);
}
inline const type_bStringHash &Gen::frontend::p_nitrous_1(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0x7f6e85a3, index);
}
inline bool Gen::frontend::p_nitrous_1(type_bStringHash &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0x7f6e85a3, result, index);
}
inline unsigned int Gen::frontend::Num_p_nitrous_1() const {
    ATTRIB_CODEGEN_GETLENGTH(0x7f6e85a3);
}
inline bool Gen::frontend::SET_p_nitrous_1(const type_bStringHash &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0x7f6e85a3, input, index);
}

inline bool Gen::frontend::p_nitrous_2(TAttrib<type_bStringHash> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0xd810d2dc);
}
inline const type_bStringHash &Gen::frontend::p_nitrous_2(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0xd810d2dc, index);
}
inline bool Gen::frontend::p_nitrous_2(type_bStringHash &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0xd810d2dc, result, index);
}
inline unsigned int Gen::frontend::Num_p_nitrous_2() const {
    ATTRIB_CODEGEN_GETLENGTH(0xd810d2dc);
}
inline bool Gen::frontend::SET_p_nitrous_2(const type_bStringHash &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0xd810d2dc, input, index);
}

inline bool Gen::frontend::p_nitrous_3(TAttrib<type_bStringHash> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0xa459ecef);
}
inline const type_bStringHash &Gen::frontend::p_nitrous_3(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0xa459ecef, index);
}
inline bool Gen::frontend::p_nitrous_3(type_bStringHash &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0xa459ecef, result, index);
}
inline unsigned int Gen::frontend::Num_p_nitrous_3() const {
    ATTRIB_CODEGEN_GETLENGTH(0xa459ecef);
}
inline bool Gen::frontend::SET_p_nitrous_3(const type_bStringHash &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0xa459ecef, input, index);
}

inline bool Gen::frontend::p_nitrous_4(TAttrib<type_bStringHash> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0x8da087a4);
}
inline const type_bStringHash &Gen::frontend::p_nitrous_4(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0x8da087a4, index);
}
inline bool Gen::frontend::p_nitrous_4(type_bStringHash &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0x8da087a4, result, index);
}
inline unsigned int Gen::frontend::Num_p_nitrous_4() const {
    ATTRIB_CODEGEN_GETLENGTH(0x8da087a4);
}
inline bool Gen::frontend::SET_p_nitrous_4(const type_bStringHash &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0x8da087a4, input, index);
}

inline bool Gen::frontend::p_suspension_1(TAttrib<type_bStringHash> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0x37ea2169);
}
inline const type_bStringHash &Gen::frontend::p_suspension_1(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0x37ea2169, index);
}
inline bool Gen::frontend::p_suspension_1(type_bStringHash &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0x37ea2169, result, index);
}
inline unsigned int Gen::frontend::Num_p_suspension_1() const {
    ATTRIB_CODEGEN_GETLENGTH(0x37ea2169);
}
inline bool Gen::frontend::SET_p_suspension_1(const type_bStringHash &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0x37ea2169, input, index);
}

inline bool Gen::frontend::p_suspension_2(TAttrib<type_bStringHash> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0xe5650914);
}
inline const type_bStringHash &Gen::frontend::p_suspension_2(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0xe5650914, index);
}
inline bool Gen::frontend::p_suspension_2(type_bStringHash &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0xe5650914, result, index);
}
inline unsigned int Gen::frontend::Num_p_suspension_2() const {
    ATTRIB_CODEGEN_GETLENGTH(0xe5650914);
}
inline bool Gen::frontend::SET_p_suspension_2(const type_bStringHash &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0xe5650914, input, index);
}

inline bool Gen::frontend::p_suspension_3(TAttrib<type_bStringHash> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0xe321687d);
}
inline const type_bStringHash &Gen::frontend::p_suspension_3(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0xe321687d, index);
}
inline bool Gen::frontend::p_suspension_3(type_bStringHash &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0xe321687d, result, index);
}
inline unsigned int Gen::frontend::Num_p_suspension_3() const {
    ATTRIB_CODEGEN_GETLENGTH(0xe321687d);
}
inline bool Gen::frontend::SET_p_suspension_3(const type_bStringHash &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0xe321687d, input, index);
}

inline bool Gen::frontend::p_suspension_4(TAttrib<type_bStringHash> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0xfb1ef23f);
}
inline const type_bStringHash &Gen::frontend::p_suspension_4(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0xfb1ef23f, index);
}
inline bool Gen::frontend::p_suspension_4(type_bStringHash &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0xfb1ef23f, result, index);
}
inline unsigned int Gen::frontend::Num_p_suspension_4() const {
    ATTRIB_CODEGEN_GETLENGTH(0xfb1ef23f);
}
inline bool Gen::frontend::SET_p_suspension_4(const type_bStringHash &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0xfb1ef23f, input, index);
}

inline bool Gen::frontend::p_tires_1(TAttrib<type_bStringHash> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0xf0c7c400);
}
inline const type_bStringHash &Gen::frontend::p_tires_1(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0xf0c7c400, index);
}
inline bool Gen::frontend::p_tires_1(type_bStringHash &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0xf0c7c400, result, index);
}
inline unsigned int Gen::frontend::Num_p_tires_1() const {
    ATTRIB_CODEGEN_GETLENGTH(0xf0c7c400);
}
inline bool Gen::frontend::SET_p_tires_1(const type_bStringHash &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0xf0c7c400, input, index);
}

inline bool Gen::frontend::p_tires_2(TAttrib<type_bStringHash> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0x01e6ddf1);
}
inline const type_bStringHash &Gen::frontend::p_tires_2(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0x01e6ddf1, index);
}
inline bool Gen::frontend::p_tires_2(type_bStringHash &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0x01e6ddf1, result, index);
}
inline unsigned int Gen::frontend::Num_p_tires_2() const {
    ATTRIB_CODEGEN_GETLENGTH(0x01e6ddf1);
}
inline bool Gen::frontend::SET_p_tires_2(const type_bStringHash &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0x01e6ddf1, input, index);
}

inline bool Gen::frontend::p_tires_3(TAttrib<type_bStringHash> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0x92378a0a);
}
inline const type_bStringHash &Gen::frontend::p_tires_3(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0x92378a0a, index);
}
inline bool Gen::frontend::p_tires_3(type_bStringHash &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0x92378a0a, result, index);
}
inline unsigned int Gen::frontend::Num_p_tires_3() const {
    ATTRIB_CODEGEN_GETLENGTH(0x92378a0a);
}
inline bool Gen::frontend::SET_p_tires_3(const type_bStringHash &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0x92378a0a, input, index);
}

inline bool Gen::frontend::p_tires_4(TAttrib<type_bStringHash> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0x16b700d6);
}
inline const type_bStringHash &Gen::frontend::p_tires_4(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0x16b700d6, index);
}
inline bool Gen::frontend::p_tires_4(type_bStringHash &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0x16b700d6, result, index);
}
inline unsigned int Gen::frontend::Num_p_tires_4() const {
    ATTRIB_CODEGEN_GETLENGTH(0x16b700d6);
}
inline bool Gen::frontend::SET_p_tires_4(const type_bStringHash &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0x16b700d6, input, index);
}

inline bool Gen::frontend::p_transmission_1(TAttrib<type_bStringHash> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0x1e823f0b);
}
inline const type_bStringHash &Gen::frontend::p_transmission_1(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0x1e823f0b, index);
}
inline bool Gen::frontend::p_transmission_1(type_bStringHash &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0x1e823f0b, result, index);
}
inline unsigned int Gen::frontend::Num_p_transmission_1() const {
    ATTRIB_CODEGEN_GETLENGTH(0x1e823f0b);
}
inline bool Gen::frontend::SET_p_transmission_1(const type_bStringHash &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0x1e823f0b, input, index);
}

inline bool Gen::frontend::p_transmission_2(TAttrib<type_bStringHash> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0x79c8d7e9);
}
inline const type_bStringHash &Gen::frontend::p_transmission_2(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0x79c8d7e9, index);
}
inline bool Gen::frontend::p_transmission_2(type_bStringHash &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0x79c8d7e9, result, index);
}
inline unsigned int Gen::frontend::Num_p_transmission_2() const {
    ATTRIB_CODEGEN_GETLENGTH(0x79c8d7e9);
}
inline bool Gen::frontend::SET_p_transmission_2(const type_bStringHash &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0x79c8d7e9, input, index);
}

inline bool Gen::frontend::p_transmission_3(TAttrib<type_bStringHash> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0xa1b53a33);
}
inline const type_bStringHash &Gen::frontend::p_transmission_3(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0xa1b53a33, index);
}
inline bool Gen::frontend::p_transmission_3(type_bStringHash &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0xa1b53a33, result, index);
}
inline unsigned int Gen::frontend::Num_p_transmission_3() const {
    ATTRIB_CODEGEN_GETLENGTH(0xa1b53a33);
}
inline bool Gen::frontend::SET_p_transmission_3(const type_bStringHash &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0xa1b53a33, input, index);
}

inline bool Gen::frontend::p_transmission_4(TAttrib<type_bStringHash> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0xf424c06d);
}
inline const type_bStringHash &Gen::frontend::p_transmission_4(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0xf424c06d, index);
}
inline bool Gen::frontend::p_transmission_4(type_bStringHash &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0xf424c06d, result, index);
}
inline unsigned int Gen::frontend::Num_p_transmission_4() const {
    ATTRIB_CODEGEN_GETLENGTH(0xf424c06d);
}
inline bool Gen::frontend::SET_p_transmission_4(const type_bStringHash &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0xf424c06d, input, index);
}

inline const EA::Reflection::UInt32 &Gen::frontend::region() const {
    ATTRIB_CODEGEN_GETLAYOUT(region);
}
inline bool Gen::frontend::region(EA::Reflection::UInt32 &result) const {
    result = region();
    return true;
}
inline bool Gen::frontend::SET_region(const EA::Reflection::UInt32 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(region, input);
}

}; // namespace Attrib

#endif
