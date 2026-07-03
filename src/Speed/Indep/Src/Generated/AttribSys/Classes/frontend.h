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
    bool CarPartName(TAttrib<eUnlockableEntity> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(eUnlockableEntity, 0xb668787d);
    }
    bool CarPartName(eUnlockableEntity &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(eUnlockableEntity, 0xb668787d, result);
    }
    const eUnlockableEntity &CarPartName() const {
        ATTRIB_CODEGEN_GETVALUE(eUnlockableEntity, 0xb668787d);
    }
    bool SET_CarPartName(const eUnlockableEntity &input) {
        ATTRIB_CODEGEN_SETVALUE(eUnlockableEntity, 0xb668787d, input);
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
    bool Cost(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xe61673e9);
    }
    bool Cost(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(Cost, result);
    }
    const EA::Reflection::Int32 &Cost() const {
        ATTRIB_CODEGEN_GETLAYOUT(Cost);
    }
    bool SET_Cost(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(Cost, input);
    }
    bool IsCustomizable(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xfbcf1a48);
    }
    bool IsCustomizable(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(IsCustomizable, result);
    }
    const EA::Reflection::Bool &IsCustomizable() const {
        ATTRIB_CODEGEN_GETLAYOUT(IsCustomizable);
    }
    bool SET_IsCustomizable(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETLAYOUT(IsCustomizable, input);
    }
    bool NameId(TAttrib<EA::Reflection::Int8> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int8, 0x2c3c7feb);
    }
    bool NameId(EA::Reflection::Int8 &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(EA::Reflection::Int8, 0x2c3c7feb, result, index);
    }
    const EA::Reflection::Int8 &NameId(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(EA::Reflection::Int8, 0x2c3c7feb, index);
    }
    unsigned int Num_NameId() const {
        ATTRIB_CODEGEN_GETLENGTH(0x2c3c7feb);
    }
    bool SET_NameId(const EA::Reflection::Int8 &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(EA::Reflection::Int8, 0x2c3c7feb, input, index);
    }
    bool RapSheetRanks(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xf9a7d5f7);
    }
    bool RapSheetRanks(EA::Reflection::Float &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(EA::Reflection::Float, 0xf9a7d5f7, result, index);
    }
    const EA::Reflection::Float &RapSheetRanks(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(EA::Reflection::Float, 0xf9a7d5f7, index);
    }
    unsigned int Num_RapSheetRanks() const {
        ATTRIB_CODEGEN_GETLENGTH(0xf9a7d5f7);
    }
    bool SET_RapSheetRanks(const EA::Reflection::Float &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(EA::Reflection::Float, 0xf9a7d5f7, input, index);
    }
    bool UnlockedAt(TAttrib<EA::Reflection::UInt8> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt8, 0x60f99f57);
    }
    bool UnlockedAt(EA::Reflection::UInt8 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(UnlockedAt, result);
    }
    const EA::Reflection::UInt8 &UnlockedAt() const {
        ATTRIB_CODEGEN_GETLAYOUT(UnlockedAt);
    }
    bool SET_UnlockedAt(const EA::Reflection::UInt8 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(UnlockedAt, input);
    }
    bool WarningLevel(TAttrib<EA::Reflection::UInt32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0xb54d4a41);
    }
    bool WarningLevel(EA::Reflection::UInt32 &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(EA::Reflection::UInt32, 0xb54d4a41, result, index);
    }
    const EA::Reflection::UInt32 &WarningLevel(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(EA::Reflection::UInt32, 0xb54d4a41, index);
    }
    unsigned int Num_WarningLevel() const {
        ATTRIB_CODEGEN_GETLENGTH(0xb54d4a41);
    }
    bool SET_WarningLevel(const EA::Reflection::UInt32 &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(EA::Reflection::UInt32, 0xb54d4a41, input, index);
    }
    bool cam_angle(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x28b5440b);
    }
    bool cam_angle(RefSpec &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(cam_angle, result);
    }
    const RefSpec &cam_angle() const {
        ATTRIB_CODEGEN_GETLAYOUT(cam_angle);
    }
    bool SET_cam_angle(const RefSpec &input) {
        ATTRIB_CODEGEN_SETLAYOUT(cam_angle, input);
    }
    bool cam_anim_speed(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xf624544d);
    }
    bool cam_anim_speed(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(cam_anim_speed, result);
    }
    const EA::Reflection::Float &cam_anim_speed() const {
        ATTRIB_CODEGEN_GETLAYOUT(cam_anim_speed);
    }
    bool SET_cam_anim_speed(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(cam_anim_speed, input);
    }
    bool cam_blur(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xf35f7b1f);
    }
    bool cam_blur(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(cam_blur, result);
    }
    const EA::Reflection::Float &cam_blur() const {
        ATTRIB_CODEGEN_GETLAYOUT(cam_blur);
    }
    bool SET_cam_blur(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(cam_blur, input);
    }
    bool cam_damping(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x2581b8ba);
    }
    bool cam_damping(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(cam_damping, result);
    }
    const EA::Reflection::Float &cam_damping() const {
        ATTRIB_CODEGEN_GETLAYOUT(cam_damping);
    }
    bool SET_cam_damping(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(cam_damping, input);
    }
    bool cam_fov(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xc220d5ab);
    }
    bool cam_fov(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(cam_fov, result);
    }
    const EA::Reflection::Float &cam_fov() const {
        ATTRIB_CODEGEN_GETLAYOUT(cam_fov);
    }
    bool SET_cam_fov(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(cam_fov, input);
    }
    bool cam_lookat_x(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xa8d9a1f4);
    }
    bool cam_lookat_x(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(cam_lookat_x, result);
    }
    const EA::Reflection::Float &cam_lookat_x() const {
        ATTRIB_CODEGEN_GETLAYOUT(cam_lookat_x);
    }
    bool SET_cam_lookat_x(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(cam_lookat_x, input);
    }
    bool cam_lookat_y(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xfe9b5900);
    }
    bool cam_lookat_y(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(cam_lookat_y, result);
    }
    const EA::Reflection::Float &cam_lookat_y() const {
        ATTRIB_CODEGEN_GETLAYOUT(cam_lookat_y);
    }
    bool SET_cam_lookat_y(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(cam_lookat_y, input);
    }
    bool cam_lookat_z(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x7c9b89d3);
    }
    bool cam_lookat_z(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(cam_lookat_z, result);
    }
    const EA::Reflection::Float &cam_lookat_z() const {
        ATTRIB_CODEGEN_GETLAYOUT(cam_lookat_z);
    }
    bool SET_cam_lookat_z(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(cam_lookat_z, input);
    }
    bool cam_orbit_horizontal(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x803fa14c);
    }
    bool cam_orbit_horizontal(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(cam_orbit_horizontal, result);
    }
    const EA::Reflection::Float &cam_orbit_horizontal() const {
        ATTRIB_CODEGEN_GETLAYOUT(cam_orbit_horizontal);
    }
    bool SET_cam_orbit_horizontal(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(cam_orbit_horizontal, input);
    }
    bool cam_orbit_radius(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xa10c312c);
    }
    bool cam_orbit_radius(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(cam_orbit_radius, result);
    }
    const EA::Reflection::Float &cam_orbit_radius() const {
        ATTRIB_CODEGEN_GETLAYOUT(cam_orbit_radius);
    }
    bool SET_cam_orbit_radius(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(cam_orbit_radius, input);
    }
    bool cam_orbit_vertical(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xc8f96e5a);
    }
    bool cam_orbit_vertical(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(cam_orbit_vertical, result);
    }
    const EA::Reflection::Float &cam_orbit_vertical() const {
        ATTRIB_CODEGEN_GETLAYOUT(cam_orbit_vertical);
    }
    bool SET_cam_orbit_vertical(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(cam_orbit_vertical, input);
    }
    bool cam_periods(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x798624ca);
    }
    bool cam_periods(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(cam_periods, result);
    }
    const EA::Reflection::Int32 &cam_periods() const {
        ATTRIB_CODEGEN_GETLAYOUT(cam_periods);
    }
    bool SET_cam_periods(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(cam_periods, input);
    }
    bool cam_roll_angle(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x05b56526);
    }
    bool cam_roll_angle(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(cam_roll_angle, result);
    }
    const EA::Reflection::Float &cam_roll_angle() const {
        ATTRIB_CODEGEN_GETLAYOUT(cam_roll_angle);
    }
    bool SET_cam_roll_angle(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(cam_roll_angle, input);
    }
    bool cam_user_rotate(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x998dd0db);
    }
    bool cam_user_rotate(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(cam_user_rotate, result);
    }
    const EA::Reflection::Bool &cam_user_rotate() const {
        ATTRIB_CODEGEN_GETLAYOUT(cam_user_rotate);
    }
    bool SET_cam_user_rotate(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETLAYOUT(cam_user_rotate, input);
    }
    bool default_car(TAttrib<RefSpec> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(RefSpec, 0x851874eb);
    }
    bool default_car(RefSpec &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(default_car, result);
    }
    const RefSpec &default_car() const {
        ATTRIB_CODEGEN_GETLAYOUT(default_car);
    }
    bool SET_default_car(const RefSpec &input) {
        ATTRIB_CODEGEN_SETLAYOUT(default_car, input);
    }
    bool feCarPartInfo(TAttrib<FECarPartInfo> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(FECarPartInfo, 0x1002ea65);
    }
    bool feCarPartInfo(FECarPartInfo &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(FECarPartInfo, 0x1002ea65, result, index);
    }
    const FECarPartInfo &feCarPartInfo(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(FECarPartInfo, 0x1002ea65, index);
    }
    unsigned int Num_feCarPartInfo() const {
        ATTRIB_CODEGEN_GETLENGTH(0x1002ea65);
    }
    bool SET_feCarPartInfo(const FECarPartInfo &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(FECarPartInfo, 0x1002ea65, input, index);
    }
    bool feCarPartName(TAttrib<eUnlockableEntity> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(eUnlockableEntity, 0x3b874697);
    }
    bool feCarPartName(eUnlockableEntity &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(eUnlockableEntity, 0x3b874697, result);
    }
    const eUnlockableEntity &feCarPartName() const {
        ATTRIB_CODEGEN_GETVALUE(eUnlockableEntity, 0x3b874697);
    }
    bool SET_feCarPartName(const eUnlockableEntity &input) {
        ATTRIB_CODEGEN_SETVALUE(eUnlockableEntity, 0x3b874697, input);
    }
    bool manufacturer(TAttrib<EA::Reflection::UInt32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0xbaf4ab4c);
    }
    bool manufacturer(EA::Reflection::UInt32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(manufacturer, result);
    }
    const EA::Reflection::UInt32 &manufacturer() const {
        ATTRIB_CODEGEN_GETLAYOUT(manufacturer);
    }
    bool SET_manufacturer(const EA::Reflection::UInt32 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(manufacturer, input);
    }
    bool p_brakes_1(TAttrib<type_bStringHash> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0xe4af1260);
    }
    bool p_brakes_1(type_bStringHash &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0xe4af1260, result, index);
    }
    const type_bStringHash &p_brakes_1(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0xe4af1260, index);
    }
    unsigned int Num_p_brakes_1() const {
        ATTRIB_CODEGEN_GETLENGTH(0xe4af1260);
    }
    bool SET_p_brakes_1(const type_bStringHash &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0xe4af1260, input, index);
    }
    bool p_brakes_2(TAttrib<type_bStringHash> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0x70b14851);
    }
    bool p_brakes_2(type_bStringHash &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0x70b14851, result, index);
    }
    const type_bStringHash &p_brakes_2(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0x70b14851, index);
    }
    unsigned int Num_p_brakes_2() const {
        ATTRIB_CODEGEN_GETLENGTH(0x70b14851);
    }
    bool SET_p_brakes_2(const type_bStringHash &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0x70b14851, input, index);
    }
    bool p_brakes_3(TAttrib<type_bStringHash> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0x8e8b78e1);
    }
    bool p_brakes_3(type_bStringHash &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0x8e8b78e1, result, index);
    }
    const type_bStringHash &p_brakes_3(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0x8e8b78e1, index);
    }
    unsigned int Num_p_brakes_3() const {
        ATTRIB_CODEGEN_GETLENGTH(0x8e8b78e1);
    }
    bool SET_p_brakes_3(const type_bStringHash &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0x8e8b78e1, input, index);
    }
    bool p_brakes_4(TAttrib<type_bStringHash> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0xb4df5439);
    }
    bool p_brakes_4(type_bStringHash &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0xb4df5439, result, index);
    }
    const type_bStringHash &p_brakes_4(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0xb4df5439, index);
    }
    unsigned int Num_p_brakes_4() const {
        ATTRIB_CODEGEN_GETLENGTH(0xb4df5439);
    }
    bool SET_p_brakes_4(const type_bStringHash &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0xb4df5439, input, index);
    }
    bool p_engine_1(TAttrib<type_bStringHash> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0x512303af);
    }
    bool p_engine_1(type_bStringHash &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0x512303af, result, index);
    }
    const type_bStringHash &p_engine_1(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0x512303af, index);
    }
    unsigned int Num_p_engine_1() const {
        ATTRIB_CODEGEN_GETLENGTH(0x512303af);
    }
    bool SET_p_engine_1(const type_bStringHash &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0x512303af, input, index);
    }
    bool p_engine_2(TAttrib<type_bStringHash> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0xdb8a8a1d);
    }
    bool p_engine_2(type_bStringHash &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0xdb8a8a1d, result, index);
    }
    const type_bStringHash &p_engine_2(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0xdb8a8a1d, index);
    }
    unsigned int Num_p_engine_2() const {
        ATTRIB_CODEGEN_GETLENGTH(0xdb8a8a1d);
    }
    bool SET_p_engine_2(const type_bStringHash &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0xdb8a8a1d, input, index);
    }
    bool p_engine_3(TAttrib<type_bStringHash> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0x4f56a655);
    }
    bool p_engine_3(type_bStringHash &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0x4f56a655, result, index);
    }
    const type_bStringHash &p_engine_3(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0x4f56a655, index);
    }
    unsigned int Num_p_engine_3() const {
        ATTRIB_CODEGEN_GETLENGTH(0x4f56a655);
    }
    bool SET_p_engine_3(const type_bStringHash &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0x4f56a655, input, index);
    }
    bool p_engine_4(TAttrib<type_bStringHash> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0x85ab21da);
    }
    bool p_engine_4(type_bStringHash &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0x85ab21da, result, index);
    }
    const type_bStringHash &p_engine_4(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0x85ab21da, index);
    }
    unsigned int Num_p_engine_4() const {
        ATTRIB_CODEGEN_GETLENGTH(0x85ab21da);
    }
    bool SET_p_engine_4(const type_bStringHash &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0x85ab21da, input, index);
    }
    bool p_induction_1(TAttrib<type_bStringHash> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0x0e141cde);
    }
    bool p_induction_1(type_bStringHash &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0x0e141cde, result, index);
    }
    const type_bStringHash &p_induction_1(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0x0e141cde, index);
    }
    unsigned int Num_p_induction_1() const {
        ATTRIB_CODEGEN_GETLENGTH(0x0e141cde);
    }
    bool SET_p_induction_1(const type_bStringHash &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0x0e141cde, input, index);
    }
    bool p_induction_2(TAttrib<type_bStringHash> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0x4d3b62f3);
    }
    bool p_induction_2(type_bStringHash &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0x4d3b62f3, result, index);
    }
    const type_bStringHash &p_induction_2(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0x4d3b62f3, index);
    }
    unsigned int Num_p_induction_2() const {
        ATTRIB_CODEGEN_GETLENGTH(0x4d3b62f3);
    }
    bool SET_p_induction_2(const type_bStringHash &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0x4d3b62f3, input, index);
    }
    bool p_induction_3(TAttrib<type_bStringHash> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0xea7f3fe4);
    }
    bool p_induction_3(type_bStringHash &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0xea7f3fe4, result, index);
    }
    const type_bStringHash &p_induction_3(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0xea7f3fe4, index);
    }
    unsigned int Num_p_induction_3() const {
        ATTRIB_CODEGEN_GETLENGTH(0xea7f3fe4);
    }
    bool SET_p_induction_3(const type_bStringHash &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0xea7f3fe4, input, index);
    }
    bool p_induction_4(TAttrib<type_bStringHash> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0xb6be1d52);
    }
    bool p_induction_4(type_bStringHash &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0xb6be1d52, result, index);
    }
    const type_bStringHash &p_induction_4(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0xb6be1d52, index);
    }
    unsigned int Num_p_induction_4() const {
        ATTRIB_CODEGEN_GETLENGTH(0xb6be1d52);
    }
    bool SET_p_induction_4(const type_bStringHash &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0xb6be1d52, input, index);
    }
    bool p_nitrous_1(TAttrib<type_bStringHash> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0x7f6e85a3);
    }
    bool p_nitrous_1(type_bStringHash &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0x7f6e85a3, result, index);
    }
    const type_bStringHash &p_nitrous_1(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0x7f6e85a3, index);
    }
    unsigned int Num_p_nitrous_1() const {
        ATTRIB_CODEGEN_GETLENGTH(0x7f6e85a3);
    }
    bool SET_p_nitrous_1(const type_bStringHash &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0x7f6e85a3, input, index);
    }
    bool p_nitrous_2(TAttrib<type_bStringHash> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0xd810d2dc);
    }
    bool p_nitrous_2(type_bStringHash &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0xd810d2dc, result, index);
    }
    const type_bStringHash &p_nitrous_2(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0xd810d2dc, index);
    }
    unsigned int Num_p_nitrous_2() const {
        ATTRIB_CODEGEN_GETLENGTH(0xd810d2dc);
    }
    bool SET_p_nitrous_2(const type_bStringHash &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0xd810d2dc, input, index);
    }
    bool p_nitrous_3(TAttrib<type_bStringHash> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0xa459ecef);
    }
    bool p_nitrous_3(type_bStringHash &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0xa459ecef, result, index);
    }
    const type_bStringHash &p_nitrous_3(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0xa459ecef, index);
    }
    unsigned int Num_p_nitrous_3() const {
        ATTRIB_CODEGEN_GETLENGTH(0xa459ecef);
    }
    bool SET_p_nitrous_3(const type_bStringHash &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0xa459ecef, input, index);
    }
    bool p_nitrous_4(TAttrib<type_bStringHash> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0x8da087a4);
    }
    bool p_nitrous_4(type_bStringHash &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0x8da087a4, result, index);
    }
    const type_bStringHash &p_nitrous_4(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0x8da087a4, index);
    }
    unsigned int Num_p_nitrous_4() const {
        ATTRIB_CODEGEN_GETLENGTH(0x8da087a4);
    }
    bool SET_p_nitrous_4(const type_bStringHash &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0x8da087a4, input, index);
    }
    bool p_suspension_1(TAttrib<type_bStringHash> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0x37ea2169);
    }
    bool p_suspension_1(type_bStringHash &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0x37ea2169, result, index);
    }
    const type_bStringHash &p_suspension_1(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0x37ea2169, index);
    }
    unsigned int Num_p_suspension_1() const {
        ATTRIB_CODEGEN_GETLENGTH(0x37ea2169);
    }
    bool SET_p_suspension_1(const type_bStringHash &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0x37ea2169, input, index);
    }
    bool p_suspension_2(TAttrib<type_bStringHash> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0xe5650914);
    }
    bool p_suspension_2(type_bStringHash &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0xe5650914, result, index);
    }
    const type_bStringHash &p_suspension_2(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0xe5650914, index);
    }
    unsigned int Num_p_suspension_2() const {
        ATTRIB_CODEGEN_GETLENGTH(0xe5650914);
    }
    bool SET_p_suspension_2(const type_bStringHash &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0xe5650914, input, index);
    }
    bool p_suspension_3(TAttrib<type_bStringHash> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0xe321687d);
    }
    bool p_suspension_3(type_bStringHash &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0xe321687d, result, index);
    }
    const type_bStringHash &p_suspension_3(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0xe321687d, index);
    }
    unsigned int Num_p_suspension_3() const {
        ATTRIB_CODEGEN_GETLENGTH(0xe321687d);
    }
    bool SET_p_suspension_3(const type_bStringHash &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0xe321687d, input, index);
    }
    bool p_suspension_4(TAttrib<type_bStringHash> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0xfb1ef23f);
    }
    bool p_suspension_4(type_bStringHash &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0xfb1ef23f, result, index);
    }
    const type_bStringHash &p_suspension_4(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0xfb1ef23f, index);
    }
    unsigned int Num_p_suspension_4() const {
        ATTRIB_CODEGEN_GETLENGTH(0xfb1ef23f);
    }
    bool SET_p_suspension_4(const type_bStringHash &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0xfb1ef23f, input, index);
    }
    bool p_tires_1(TAttrib<type_bStringHash> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0xf0c7c400);
    }
    bool p_tires_1(type_bStringHash &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0xf0c7c400, result, index);
    }
    const type_bStringHash &p_tires_1(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0xf0c7c400, index);
    }
    unsigned int Num_p_tires_1() const {
        ATTRIB_CODEGEN_GETLENGTH(0xf0c7c400);
    }
    bool SET_p_tires_1(const type_bStringHash &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0xf0c7c400, input, index);
    }
    bool p_tires_2(TAttrib<type_bStringHash> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0x01e6ddf1);
    }
    bool p_tires_2(type_bStringHash &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0x01e6ddf1, result, index);
    }
    const type_bStringHash &p_tires_2(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0x01e6ddf1, index);
    }
    unsigned int Num_p_tires_2() const {
        ATTRIB_CODEGEN_GETLENGTH(0x01e6ddf1);
    }
    bool SET_p_tires_2(const type_bStringHash &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0x01e6ddf1, input, index);
    }
    bool p_tires_3(TAttrib<type_bStringHash> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0x92378a0a);
    }
    bool p_tires_3(type_bStringHash &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0x92378a0a, result, index);
    }
    const type_bStringHash &p_tires_3(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0x92378a0a, index);
    }
    unsigned int Num_p_tires_3() const {
        ATTRIB_CODEGEN_GETLENGTH(0x92378a0a);
    }
    bool SET_p_tires_3(const type_bStringHash &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0x92378a0a, input, index);
    }
    bool p_tires_4(TAttrib<type_bStringHash> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0x16b700d6);
    }
    bool p_tires_4(type_bStringHash &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0x16b700d6, result, index);
    }
    const type_bStringHash &p_tires_4(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0x16b700d6, index);
    }
    unsigned int Num_p_tires_4() const {
        ATTRIB_CODEGEN_GETLENGTH(0x16b700d6);
    }
    bool SET_p_tires_4(const type_bStringHash &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0x16b700d6, input, index);
    }
    bool p_transmission_1(TAttrib<type_bStringHash> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0x1e823f0b);
    }
    bool p_transmission_1(type_bStringHash &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0x1e823f0b, result, index);
    }
    const type_bStringHash &p_transmission_1(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0x1e823f0b, index);
    }
    unsigned int Num_p_transmission_1() const {
        ATTRIB_CODEGEN_GETLENGTH(0x1e823f0b);
    }
    bool SET_p_transmission_1(const type_bStringHash &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0x1e823f0b, input, index);
    }
    bool p_transmission_2(TAttrib<type_bStringHash> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0x79c8d7e9);
    }
    bool p_transmission_2(type_bStringHash &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0x79c8d7e9, result, index);
    }
    const type_bStringHash &p_transmission_2(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0x79c8d7e9, index);
    }
    unsigned int Num_p_transmission_2() const {
        ATTRIB_CODEGEN_GETLENGTH(0x79c8d7e9);
    }
    bool SET_p_transmission_2(const type_bStringHash &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0x79c8d7e9, input, index);
    }
    bool p_transmission_3(TAttrib<type_bStringHash> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0xa1b53a33);
    }
    bool p_transmission_3(type_bStringHash &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0xa1b53a33, result, index);
    }
    const type_bStringHash &p_transmission_3(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0xa1b53a33, index);
    }
    unsigned int Num_p_transmission_3() const {
        ATTRIB_CODEGEN_GETLENGTH(0xa1b53a33);
    }
    bool SET_p_transmission_3(const type_bStringHash &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0xa1b53a33, input, index);
    }
    bool p_transmission_4(TAttrib<type_bStringHash> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(type_bStringHash, 0xf424c06d);
    }
    bool p_transmission_4(type_bStringHash &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(type_bStringHash, 0xf424c06d, result, index);
    }
    const type_bStringHash &p_transmission_4(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(type_bStringHash, 0xf424c06d, index);
    }
    unsigned int Num_p_transmission_4() const {
        ATTRIB_CODEGEN_GETLENGTH(0xf424c06d);
    }
    bool SET_p_transmission_4(const type_bStringHash &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(type_bStringHash, 0xf424c06d, input, index);
    }
    bool region(TAttrib<EA::Reflection::UInt32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0x0de009be);
    }
    bool region(EA::Reflection::UInt32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(region, result);
    }
    const EA::Reflection::UInt32 &region() const {
        ATTRIB_CODEGEN_GETLAYOUT(region);
    }
    bool SET_region(const EA::Reflection::UInt32 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(region, input);
    }

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

}; // namespace Attrib

#endif
