#ifndef _attrib_gen_rigidbodyspecs_h
#define _attrib_gen_rigidbodyspecs_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct rigidbodyspecs : Instance {
    struct _LayoutStruct {
        UMath::Vector4 COLLISION_BOX_PAD;                // offset 0x0, size 0x10
        UMath::Vector4 DRAG;                             // offset 0x10, size 0x10
        UMath::Vector4 WORLD_MOMENT_SCALE;               // offset 0x20, size 0x10
        UMath::Vector4 OBJ_MOMENT_SCALE;                 // offset 0x30, size 0x10
        UMath::Vector4 GROUND_ELASTICITY;                // offset 0x40, size 0x10
        UMath::Vector4 OBJ_ELASTICITY;                   // offset 0x50, size 0x10
        UMath::Vector4 DRAG_ANGULAR;                     // offset 0x60, size 0x10
        UMath::Vector4 WALL_ELASTICITY;                  // offset 0x70, size 0x10
        UMath::Vector4 GROUND_MOMENT_SCALE;              // offset 0x80, size 0x10
        UMath::Vector4 CG;                               // offset 0x90, size 0x10
        Attrib::StringKey BASE_MATERIAL;                 // offset 0xa0, size 0x10
        Attrib::StringKey DEFAULT_COL_BOX;               // offset 0xb0, size 0x10
        Private _Array_OBJ_FRICTION;                     // offset 0xc0, size 0x8
        EA::Reflection::Float OBJ_FRICTION[2];           // offset 0xc8, size 0x8
        Private _Array_WALL_FRICTION;                    // offset 0xd0, size 0x8
        EA::Reflection::Float WALL_FRICTION[2];          // offset 0xd8, size 0x8
        Private _Array_GROUND_FRICTION;                  // offset 0xe0, size 0x8
        EA::Reflection::Float GROUND_FRICTION[2];        // offset 0xe8, size 0x8
        EA::Reflection::Float GRAVITY;                   // offset 0xf0, size 0x4
        EA::Reflection::Float NATURAL_ANGULAR_DAMPING;   // offset 0xf4, size 0x4
        EA::Reflection::Float SLEEP_VELOCITY;            // offset 0xf8, size 0x4
        EA::Reflection::Bool NO_GROUND_COLLISIONS;       // offset 0xfc, size 0x1
        EA::Reflection::Bool IMMOBILE_OBJECT_COLLISIONS; // offset 0xfd, size 0x1
        EA::Reflection::Bool NO_WORLD_COLLISIONS;        // offset 0xfe, size 0x1
        EA::Reflection::Bool INSTANCE_COLLISIONS_3D;     // offset 0xff, size 0x1
        EA::Reflection::Bool NO_OBJ_COLLISIONS;          // offset 0x100, size 0x1
    };

    typedef Attrib::StringKey TypeOf_BASE_MATERIAL;
    typedef UMath::Vector4 TypeOf_CG;
    typedef UMath::Vector4 TypeOf_COLLISION_BOX_PAD;
    typedef Attrib::StringKey TypeOf_DEFAULT_COL_BOX;
    typedef UMath::Vector4 TypeOf_DRAG;
    typedef UMath::Vector4 TypeOf_DRAG_ANGULAR;
    typedef EA::Reflection::Float TypeOf_GRAVITY;
    typedef UMath::Vector4 TypeOf_GROUND_ELASTICITY;
    typedef EA::Reflection::Float TypeOf_GROUND_FRICTION;
    typedef UMath::Vector4 TypeOf_GROUND_MOMENT_SCALE;
    typedef EA::Reflection::Bool TypeOf_IMMOBILE_OBJECT_COLLISIONS;
    typedef EA::Reflection::Bool TypeOf_INSTANCE_COLLISIONS_3D;
    typedef EA::Reflection::Float TypeOf_NATURAL_ANGULAR_DAMPING;
    typedef EA::Reflection::Bool TypeOf_NO_GROUND_COLLISIONS;
    typedef EA::Reflection::Bool TypeOf_NO_OBJ_COLLISIONS;
    typedef EA::Reflection::Bool TypeOf_NO_WORLD_COLLISIONS;
    typedef UMath::Vector4 TypeOf_OBJ_ELASTICITY;
    typedef EA::Reflection::Float TypeOf_OBJ_FRICTION;
    typedef UMath::Vector4 TypeOf_OBJ_MOMENT_SCALE;
    typedef EA::Reflection::Float TypeOf_SLEEP_VELOCITY;
    typedef UMath::Vector4 TypeOf_WALL_ELASTICITY;
    typedef EA::Reflection::Float TypeOf_WALL_FRICTION;
    typedef UMath::Vector4 TypeOf_WORLD_MOMENT_SCALE;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("rigidbodyspecs");
    rigidbodyspecs(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    rigidbodyspecs(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    rigidbodyspecs(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    rigidbodyspecs(const rigidbodyspecs &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    rigidbodyspecs(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~rigidbodyspecs() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0x7c90bb38;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0x7c90bb38, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const rigidbodyspecs &operator=(const rigidbodyspecs &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const rigidbodyspecs &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool BASE_MATERIAL(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x4164f5f9);
    }
    bool BASE_MATERIAL(Attrib::StringKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(BASE_MATERIAL, result);
    }
    const Attrib::StringKey &BASE_MATERIAL() const {
        ATTRIB_CODEGEN_GETLAYOUT(BASE_MATERIAL);
    }
    bool SET_BASE_MATERIAL(const Attrib::StringKey &input) {
        ATTRIB_CODEGEN_SETLAYOUT(BASE_MATERIAL, input);
    }
    bool CG(TAttrib<UMath::Vector4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x91eb5d15);
    }
    bool CG(UMath::Vector4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(CG, result);
    }
    const UMath::Vector4 &CG() const {
        ATTRIB_CODEGEN_GETLAYOUT(CG);
    }
    bool SET_CG(const UMath::Vector4 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(CG, input);
    }
    bool COLLISION_BOX_PAD(TAttrib<UMath::Vector4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x897ccf01);
    }
    bool COLLISION_BOX_PAD(UMath::Vector4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(COLLISION_BOX_PAD, result);
    }
    const UMath::Vector4 &COLLISION_BOX_PAD() const {
        ATTRIB_CODEGEN_GETLAYOUT(COLLISION_BOX_PAD);
    }
    bool SET_COLLISION_BOX_PAD(const UMath::Vector4 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(COLLISION_BOX_PAD, input);
    }
    bool DEFAULT_COL_BOX(TAttrib<Attrib::StringKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x52a236e0);
    }
    bool DEFAULT_COL_BOX(Attrib::StringKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(DEFAULT_COL_BOX, result);
    }
    const Attrib::StringKey &DEFAULT_COL_BOX() const {
        ATTRIB_CODEGEN_GETLAYOUT(DEFAULT_COL_BOX);
    }
    bool SET_DEFAULT_COL_BOX(const Attrib::StringKey &input) {
        ATTRIB_CODEGEN_SETLAYOUT(DEFAULT_COL_BOX, input);
    }
    bool DRAG(TAttrib<UMath::Vector4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0xf9f0f071);
    }
    bool DRAG(UMath::Vector4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(DRAG, result);
    }
    const UMath::Vector4 &DRAG() const {
        ATTRIB_CODEGEN_GETLAYOUT(DRAG);
    }
    bool SET_DRAG(const UMath::Vector4 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(DRAG, input);
    }
    bool DRAG_ANGULAR(TAttrib<UMath::Vector4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x1ad92dda);
    }
    bool DRAG_ANGULAR(UMath::Vector4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(DRAG_ANGULAR, result);
    }
    const UMath::Vector4 &DRAG_ANGULAR() const {
        ATTRIB_CODEGEN_GETLAYOUT(DRAG_ANGULAR);
    }
    bool SET_DRAG_ANGULAR(const UMath::Vector4 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(DRAG_ANGULAR, input);
    }
    bool GRAVITY(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x07a7e841);
    }
    bool GRAVITY(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(GRAVITY, result);
    }
    const EA::Reflection::Float &GRAVITY() const {
        ATTRIB_CODEGEN_GETLAYOUT(GRAVITY);
    }
    bool SET_GRAVITY(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(GRAVITY, input);
    }
    bool GROUND_ELASTICITY(TAttrib<UMath::Vector4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x97ed84dd);
    }
    bool GROUND_ELASTICITY(UMath::Vector4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(GROUND_ELASTICITY, result);
    }
    const UMath::Vector4 &GROUND_ELASTICITY() const {
        ATTRIB_CODEGEN_GETLAYOUT(GROUND_ELASTICITY);
    }
    bool SET_GROUND_ELASTICITY(const UMath::Vector4 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(GROUND_ELASTICITY, input);
    }
    bool GROUND_FRICTION(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x3fb1b342);
    }
    bool GROUND_FRICTION(EA::Reflection::Float &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(GROUND_FRICTION, result, index);
    }
    const EA::Reflection::Float &GROUND_FRICTION(unsigned int index) const {
        ATTRIB_CODEGEN_GETLAYOUTINDEXED(EA::Reflection::Float, GROUND_FRICTION, index);
    }
    unsigned int Num_GROUND_FRICTION() const {
        ATTRIB_CODEGEN_GETLAYOUTLENGTH(GROUND_FRICTION);
    }
    bool SET_GROUND_FRICTION(const EA::Reflection::Float &input, unsigned int index) {
        ATTRIB_CODEGEN_SETLAYOUTINDEXED(GROUND_FRICTION, input, index);
    }
    bool GROUND_MOMENT_SCALE(TAttrib<UMath::Vector4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0xb7dff090);
    }
    bool GROUND_MOMENT_SCALE(UMath::Vector4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(GROUND_MOMENT_SCALE, result);
    }
    const UMath::Vector4 &GROUND_MOMENT_SCALE() const {
        ATTRIB_CODEGEN_GETLAYOUT(GROUND_MOMENT_SCALE);
    }
    bool SET_GROUND_MOMENT_SCALE(const UMath::Vector4 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(GROUND_MOMENT_SCALE, input);
    }
    bool IMMOBILE_OBJECT_COLLISIONS(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x4793843b);
    }
    bool IMMOBILE_OBJECT_COLLISIONS(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(IMMOBILE_OBJECT_COLLISIONS, result);
    }
    const EA::Reflection::Bool &IMMOBILE_OBJECT_COLLISIONS() const {
        ATTRIB_CODEGEN_GETLAYOUT(IMMOBILE_OBJECT_COLLISIONS);
    }
    bool SET_IMMOBILE_OBJECT_COLLISIONS(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETLAYOUT(IMMOBILE_OBJECT_COLLISIONS, input);
    }
    bool INSTANCE_COLLISIONS_3D(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x22c5d3f9);
    }
    bool INSTANCE_COLLISIONS_3D(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(INSTANCE_COLLISIONS_3D, result);
    }
    const EA::Reflection::Bool &INSTANCE_COLLISIONS_3D() const {
        ATTRIB_CODEGEN_GETLAYOUT(INSTANCE_COLLISIONS_3D);
    }
    bool SET_INSTANCE_COLLISIONS_3D(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETLAYOUT(INSTANCE_COLLISIONS_3D, input);
    }
    bool NATURAL_ANGULAR_DAMPING(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xae9322de);
    }
    bool NATURAL_ANGULAR_DAMPING(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(NATURAL_ANGULAR_DAMPING, result);
    }
    const EA::Reflection::Float &NATURAL_ANGULAR_DAMPING() const {
        ATTRIB_CODEGEN_GETLAYOUT(NATURAL_ANGULAR_DAMPING);
    }
    bool SET_NATURAL_ANGULAR_DAMPING(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(NATURAL_ANGULAR_DAMPING, input);
    }
    bool NO_GROUND_COLLISIONS(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x43a505ee);
    }
    bool NO_GROUND_COLLISIONS(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(NO_GROUND_COLLISIONS, result);
    }
    const EA::Reflection::Bool &NO_GROUND_COLLISIONS() const {
        ATTRIB_CODEGEN_GETLAYOUT(NO_GROUND_COLLISIONS);
    }
    bool SET_NO_GROUND_COLLISIONS(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETLAYOUT(NO_GROUND_COLLISIONS, input);
    }
    bool NO_OBJ_COLLISIONS(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x45b11648);
    }
    bool NO_OBJ_COLLISIONS(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(NO_OBJ_COLLISIONS, result);
    }
    const EA::Reflection::Bool &NO_OBJ_COLLISIONS() const {
        ATTRIB_CODEGEN_GETLAYOUT(NO_OBJ_COLLISIONS);
    }
    bool SET_NO_OBJ_COLLISIONS(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETLAYOUT(NO_OBJ_COLLISIONS, input);
    }
    bool NO_WORLD_COLLISIONS(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x1b5ea2c7);
    }
    bool NO_WORLD_COLLISIONS(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(NO_WORLD_COLLISIONS, result);
    }
    const EA::Reflection::Bool &NO_WORLD_COLLISIONS() const {
        ATTRIB_CODEGEN_GETLAYOUT(NO_WORLD_COLLISIONS);
    }
    bool SET_NO_WORLD_COLLISIONS(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETLAYOUT(NO_WORLD_COLLISIONS, input);
    }
    bool OBJ_ELASTICITY(TAttrib<UMath::Vector4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x99b357bb);
    }
    bool OBJ_ELASTICITY(UMath::Vector4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(OBJ_ELASTICITY, result);
    }
    const UMath::Vector4 &OBJ_ELASTICITY() const {
        ATTRIB_CODEGEN_GETLAYOUT(OBJ_ELASTICITY);
    }
    bool SET_OBJ_ELASTICITY(const UMath::Vector4 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(OBJ_ELASTICITY, input);
    }
    bool OBJ_FRICTION(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xe1a55d9b);
    }
    bool OBJ_FRICTION(EA::Reflection::Float &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(OBJ_FRICTION, result, index);
    }
    const EA::Reflection::Float &OBJ_FRICTION(unsigned int index) const {
        ATTRIB_CODEGEN_GETLAYOUTINDEXED(EA::Reflection::Float, OBJ_FRICTION, index);
    }
    unsigned int Num_OBJ_FRICTION() const {
        ATTRIB_CODEGEN_GETLAYOUTLENGTH(OBJ_FRICTION);
    }
    bool SET_OBJ_FRICTION(const EA::Reflection::Float &input, unsigned int index) {
        ATTRIB_CODEGEN_SETLAYOUTINDEXED(OBJ_FRICTION, input, index);
    }
    bool OBJ_MOMENT_SCALE(TAttrib<UMath::Vector4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0xd4167df9);
    }
    bool OBJ_MOMENT_SCALE(UMath::Vector4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(OBJ_MOMENT_SCALE, result);
    }
    const UMath::Vector4 &OBJ_MOMENT_SCALE() const {
        ATTRIB_CODEGEN_GETLAYOUT(OBJ_MOMENT_SCALE);
    }
    bool SET_OBJ_MOMENT_SCALE(const UMath::Vector4 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(OBJ_MOMENT_SCALE, input);
    }
    bool SLEEP_VELOCITY(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x9f4709c6);
    }
    bool SLEEP_VELOCITY(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(SLEEP_VELOCITY, result);
    }
    const EA::Reflection::Float &SLEEP_VELOCITY() const {
        ATTRIB_CODEGEN_GETLAYOUT(SLEEP_VELOCITY);
    }
    bool SET_SLEEP_VELOCITY(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(SLEEP_VELOCITY, input);
    }
    bool WALL_ELASTICITY(TAttrib<UMath::Vector4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0xbe3faeb0);
    }
    bool WALL_ELASTICITY(UMath::Vector4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(WALL_ELASTICITY, result);
    }
    const UMath::Vector4 &WALL_ELASTICITY() const {
        ATTRIB_CODEGEN_GETLAYOUT(WALL_ELASTICITY);
    }
    bool SET_WALL_ELASTICITY(const UMath::Vector4 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(WALL_ELASTICITY, input);
    }
    bool WALL_FRICTION(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x519c53b1);
    }
    bool WALL_FRICTION(EA::Reflection::Float &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(WALL_FRICTION, result, index);
    }
    const EA::Reflection::Float &WALL_FRICTION(unsigned int index) const {
        ATTRIB_CODEGEN_GETLAYOUTINDEXED(EA::Reflection::Float, WALL_FRICTION, index);
    }
    unsigned int Num_WALL_FRICTION() const {
        ATTRIB_CODEGEN_GETLAYOUTLENGTH(WALL_FRICTION);
    }
    bool SET_WALL_FRICTION(const EA::Reflection::Float &input, unsigned int index) {
        ATTRIB_CODEGEN_SETLAYOUTINDEXED(WALL_FRICTION, input, index);
    }
    bool WORLD_MOMENT_SCALE(TAttrib<UMath::Vector4> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x048fa05d);
    }
    bool WORLD_MOMENT_SCALE(UMath::Vector4 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(WORLD_MOMENT_SCALE, result);
    }
    const UMath::Vector4 &WORLD_MOMENT_SCALE() const {
        ATTRIB_CODEGEN_GETLAYOUT(WORLD_MOMENT_SCALE);
    }
    bool SET_WORLD_MOMENT_SCALE(const UMath::Vector4 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(WORLD_MOMENT_SCALE, input);
    }

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    rigidbodyspecs &ConvertFromInstance(Instance &src) {}
    const rigidbodyspecs &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key rigidbodyspecs = 0x7c90bb38;

}; // namespace ClassName

namespace Hash {
namespace rigidbodyspecs {

static const Key BASE_MATERIAL = 0x4164f5f9;
static const Key CG = 0x91eb5d15;
static const Key COLLISION_BOX_PAD = 0x897ccf01;
static const Key DEFAULT_COL_BOX = 0x52a236e0;
static const Key DRAG = 0xf9f0f071;
static const Key DRAG_ANGULAR = 0x1ad92dda;
static const Key GRAVITY = 0x07a7e841;
static const Key GROUND_ELASTICITY = 0x97ed84dd;
static const Key GROUND_FRICTION = 0x3fb1b342;
static const Key GROUND_MOMENT_SCALE = 0xb7dff090;
static const Key IMMOBILE_OBJECT_COLLISIONS = 0x4793843b;
static const Key INSTANCE_COLLISIONS_3D = 0x22c5d3f9;
static const Key NATURAL_ANGULAR_DAMPING = 0xae9322de;
static const Key NO_GROUND_COLLISIONS = 0x43a505ee;
static const Key NO_OBJ_COLLISIONS = 0x45b11648;
static const Key NO_WORLD_COLLISIONS = 0x1b5ea2c7;
static const Key OBJ_ELASTICITY = 0x99b357bb;
static const Key OBJ_FRICTION = 0xe1a55d9b;
static const Key OBJ_MOMENT_SCALE = 0xd4167df9;
static const Key SLEEP_VELOCITY = 0x9f4709c6;
static const Key WALL_ELASTICITY = 0xbe3faeb0;
static const Key WALL_FRICTION = 0x519c53b1;
static const Key WORLD_MOMENT_SCALE = 0x048fa05d;

}; // namespace rigidbodyspecs
}; // namespace Hash

inline Key Gen::rigidbodyspecs::ClassKey() {
    return ClassName::rigidbodyspecs;
}

}; // namespace Attrib

#endif
