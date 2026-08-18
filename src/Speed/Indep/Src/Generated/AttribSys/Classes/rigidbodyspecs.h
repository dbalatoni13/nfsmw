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
    bool BASE_MATERIAL(TAttrib<Attrib::StringKey> &result) const;
    bool BASE_MATERIAL(Attrib::StringKey &result) const;
    const Attrib::StringKey &BASE_MATERIAL() const;
    bool SET_BASE_MATERIAL(const Attrib::StringKey &input);

    bool CG(TAttrib<UMath::Vector4> &result) const;
    bool CG(UMath::Vector4 &result) const;
    const UMath::Vector4 &CG() const;
    bool SET_CG(const UMath::Vector4 &input);

    bool COLLISION_BOX_PAD(TAttrib<UMath::Vector4> &result) const;
    bool COLLISION_BOX_PAD(UMath::Vector4 &result) const;
    const UMath::Vector4 &COLLISION_BOX_PAD() const;
    bool SET_COLLISION_BOX_PAD(const UMath::Vector4 &input);

    bool DEFAULT_COL_BOX(TAttrib<Attrib::StringKey> &result) const;
    bool DEFAULT_COL_BOX(Attrib::StringKey &result) const;
    const Attrib::StringKey &DEFAULT_COL_BOX() const;
    bool SET_DEFAULT_COL_BOX(const Attrib::StringKey &input);

    bool DRAG(TAttrib<UMath::Vector4> &result) const;
    bool DRAG(UMath::Vector4 &result) const;
    const UMath::Vector4 &DRAG() const;
    bool SET_DRAG(const UMath::Vector4 &input);

    bool DRAG_ANGULAR(TAttrib<UMath::Vector4> &result) const;
    bool DRAG_ANGULAR(UMath::Vector4 &result) const;
    const UMath::Vector4 &DRAG_ANGULAR() const;
    bool SET_DRAG_ANGULAR(const UMath::Vector4 &input);

    bool GRAVITY(TAttrib<EA::Reflection::Float> &result) const;
    bool GRAVITY(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &GRAVITY() const;
    bool SET_GRAVITY(const EA::Reflection::Float &input);

    bool GROUND_ELASTICITY(TAttrib<UMath::Vector4> &result) const;
    bool GROUND_ELASTICITY(UMath::Vector4 &result) const;
    const UMath::Vector4 &GROUND_ELASTICITY() const;
    bool SET_GROUND_ELASTICITY(const UMath::Vector4 &input);

    bool GROUND_FRICTION(TAttrib<EA::Reflection::Float> &result) const;
    bool GROUND_FRICTION(EA::Reflection::Float &result, unsigned int index) const;
    const EA::Reflection::Float &GROUND_FRICTION(unsigned int index) const;
    unsigned int Num_GROUND_FRICTION() const;
    bool SET_GROUND_FRICTION(const EA::Reflection::Float &input, unsigned int index);

    bool GROUND_MOMENT_SCALE(TAttrib<UMath::Vector4> &result) const;
    bool GROUND_MOMENT_SCALE(UMath::Vector4 &result) const;
    const UMath::Vector4 &GROUND_MOMENT_SCALE() const;
    bool SET_GROUND_MOMENT_SCALE(const UMath::Vector4 &input);

    bool IMMOBILE_OBJECT_COLLISIONS(TAttrib<EA::Reflection::Bool> &result) const;
    bool IMMOBILE_OBJECT_COLLISIONS(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &IMMOBILE_OBJECT_COLLISIONS() const;
    bool SET_IMMOBILE_OBJECT_COLLISIONS(const EA::Reflection::Bool &input);

    bool INSTANCE_COLLISIONS_3D(TAttrib<EA::Reflection::Bool> &result) const;
    bool INSTANCE_COLLISIONS_3D(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &INSTANCE_COLLISIONS_3D() const;
    bool SET_INSTANCE_COLLISIONS_3D(const EA::Reflection::Bool &input);

    bool NATURAL_ANGULAR_DAMPING(TAttrib<EA::Reflection::Float> &result) const;
    bool NATURAL_ANGULAR_DAMPING(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &NATURAL_ANGULAR_DAMPING() const;
    bool SET_NATURAL_ANGULAR_DAMPING(const EA::Reflection::Float &input);

    bool NO_GROUND_COLLISIONS(TAttrib<EA::Reflection::Bool> &result) const;
    bool NO_GROUND_COLLISIONS(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &NO_GROUND_COLLISIONS() const;
    bool SET_NO_GROUND_COLLISIONS(const EA::Reflection::Bool &input);

    bool NO_OBJ_COLLISIONS(TAttrib<EA::Reflection::Bool> &result) const;
    bool NO_OBJ_COLLISIONS(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &NO_OBJ_COLLISIONS() const;
    bool SET_NO_OBJ_COLLISIONS(const EA::Reflection::Bool &input);

    bool NO_WORLD_COLLISIONS(TAttrib<EA::Reflection::Bool> &result) const;
    bool NO_WORLD_COLLISIONS(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &NO_WORLD_COLLISIONS() const;
    bool SET_NO_WORLD_COLLISIONS(const EA::Reflection::Bool &input);

    bool OBJ_ELASTICITY(TAttrib<UMath::Vector4> &result) const;
    bool OBJ_ELASTICITY(UMath::Vector4 &result) const;
    const UMath::Vector4 &OBJ_ELASTICITY() const;
    bool SET_OBJ_ELASTICITY(const UMath::Vector4 &input);

    bool OBJ_FRICTION(TAttrib<EA::Reflection::Float> &result) const;
    bool OBJ_FRICTION(EA::Reflection::Float &result, unsigned int index) const;
    const EA::Reflection::Float &OBJ_FRICTION(unsigned int index) const;
    unsigned int Num_OBJ_FRICTION() const;
    bool SET_OBJ_FRICTION(const EA::Reflection::Float &input, unsigned int index);

    bool OBJ_MOMENT_SCALE(TAttrib<UMath::Vector4> &result) const;
    bool OBJ_MOMENT_SCALE(UMath::Vector4 &result) const;
    const UMath::Vector4 &OBJ_MOMENT_SCALE() const;
    bool SET_OBJ_MOMENT_SCALE(const UMath::Vector4 &input);

    bool SLEEP_VELOCITY(TAttrib<EA::Reflection::Float> &result) const;
    bool SLEEP_VELOCITY(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &SLEEP_VELOCITY() const;
    bool SET_SLEEP_VELOCITY(const EA::Reflection::Float &input);

    bool WALL_ELASTICITY(TAttrib<UMath::Vector4> &result) const;
    bool WALL_ELASTICITY(UMath::Vector4 &result) const;
    const UMath::Vector4 &WALL_ELASTICITY() const;
    bool SET_WALL_ELASTICITY(const UMath::Vector4 &input);

    bool WALL_FRICTION(TAttrib<EA::Reflection::Float> &result) const;
    bool WALL_FRICTION(EA::Reflection::Float &result, unsigned int index) const;
    const EA::Reflection::Float &WALL_FRICTION(unsigned int index) const;
    unsigned int Num_WALL_FRICTION() const;
    bool SET_WALL_FRICTION(const EA::Reflection::Float &input, unsigned int index);

    bool WORLD_MOMENT_SCALE(TAttrib<UMath::Vector4> &result) const;
    bool WORLD_MOMENT_SCALE(UMath::Vector4 &result) const;
    const UMath::Vector4 &WORLD_MOMENT_SCALE() const;
    bool SET_WORLD_MOMENT_SCALE(const UMath::Vector4 &input);

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

inline bool Gen::rigidbodyspecs::BASE_MATERIAL(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x4164f5f9);
}
inline const Attrib::StringKey &Gen::rigidbodyspecs::BASE_MATERIAL() const {
    ATTRIB_CODEGEN_GETLAYOUT(BASE_MATERIAL);
}
inline bool Gen::rigidbodyspecs::BASE_MATERIAL(Attrib::StringKey &result) const {
    result = BASE_MATERIAL();
    return true;
}
inline bool Gen::rigidbodyspecs::SET_BASE_MATERIAL(const Attrib::StringKey &input) {
    ATTRIB_CODEGEN_SETLAYOUT(BASE_MATERIAL, input);
}

inline bool Gen::rigidbodyspecs::CG(TAttrib<UMath::Vector4> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x91eb5d15);
}
inline const UMath::Vector4 &Gen::rigidbodyspecs::CG() const {
    ATTRIB_CODEGEN_GETLAYOUT(CG);
}
inline bool Gen::rigidbodyspecs::CG(UMath::Vector4 &result) const {
    result = CG();
    return true;
}
inline bool Gen::rigidbodyspecs::SET_CG(const UMath::Vector4 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(CG, input);
}

inline bool Gen::rigidbodyspecs::COLLISION_BOX_PAD(TAttrib<UMath::Vector4> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x897ccf01);
}
inline const UMath::Vector4 &Gen::rigidbodyspecs::COLLISION_BOX_PAD() const {
    ATTRIB_CODEGEN_GETLAYOUT(COLLISION_BOX_PAD);
}
inline bool Gen::rigidbodyspecs::COLLISION_BOX_PAD(UMath::Vector4 &result) const {
    result = COLLISION_BOX_PAD();
    return true;
}
inline bool Gen::rigidbodyspecs::SET_COLLISION_BOX_PAD(const UMath::Vector4 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(COLLISION_BOX_PAD, input);
}

inline bool Gen::rigidbodyspecs::DEFAULT_COL_BOX(TAttrib<Attrib::StringKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::StringKey, 0x52a236e0);
}
inline const Attrib::StringKey &Gen::rigidbodyspecs::DEFAULT_COL_BOX() const {
    ATTRIB_CODEGEN_GETLAYOUT(DEFAULT_COL_BOX);
}
inline bool Gen::rigidbodyspecs::DEFAULT_COL_BOX(Attrib::StringKey &result) const {
    result = DEFAULT_COL_BOX();
    return true;
}
inline bool Gen::rigidbodyspecs::SET_DEFAULT_COL_BOX(const Attrib::StringKey &input) {
    ATTRIB_CODEGEN_SETLAYOUT(DEFAULT_COL_BOX, input);
}

inline bool Gen::rigidbodyspecs::DRAG(TAttrib<UMath::Vector4> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0xf9f0f071);
}
inline const UMath::Vector4 &Gen::rigidbodyspecs::DRAG() const {
    ATTRIB_CODEGEN_GETLAYOUT(DRAG);
}
inline bool Gen::rigidbodyspecs::DRAG(UMath::Vector4 &result) const {
    result = DRAG();
    return true;
}
inline bool Gen::rigidbodyspecs::SET_DRAG(const UMath::Vector4 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(DRAG, input);
}

inline bool Gen::rigidbodyspecs::DRAG_ANGULAR(TAttrib<UMath::Vector4> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x1ad92dda);
}
inline const UMath::Vector4 &Gen::rigidbodyspecs::DRAG_ANGULAR() const {
    ATTRIB_CODEGEN_GETLAYOUT(DRAG_ANGULAR);
}
inline bool Gen::rigidbodyspecs::DRAG_ANGULAR(UMath::Vector4 &result) const {
    result = DRAG_ANGULAR();
    return true;
}
inline bool Gen::rigidbodyspecs::SET_DRAG_ANGULAR(const UMath::Vector4 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(DRAG_ANGULAR, input);
}

inline bool Gen::rigidbodyspecs::GRAVITY(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x07a7e841);
}
inline const EA::Reflection::Float &Gen::rigidbodyspecs::GRAVITY() const {
    ATTRIB_CODEGEN_GETLAYOUT(GRAVITY);
}
inline bool Gen::rigidbodyspecs::GRAVITY(EA::Reflection::Float &result) const {
    result = GRAVITY();
    return true;
}
inline bool Gen::rigidbodyspecs::SET_GRAVITY(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(GRAVITY, input);
}

inline bool Gen::rigidbodyspecs::GROUND_ELASTICITY(TAttrib<UMath::Vector4> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x97ed84dd);
}
inline const UMath::Vector4 &Gen::rigidbodyspecs::GROUND_ELASTICITY() const {
    ATTRIB_CODEGEN_GETLAYOUT(GROUND_ELASTICITY);
}
inline bool Gen::rigidbodyspecs::GROUND_ELASTICITY(UMath::Vector4 &result) const {
    result = GROUND_ELASTICITY();
    return true;
}
inline bool Gen::rigidbodyspecs::SET_GROUND_ELASTICITY(const UMath::Vector4 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(GROUND_ELASTICITY, input);
}

inline bool Gen::rigidbodyspecs::GROUND_FRICTION(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x3fb1b342);
}
inline const EA::Reflection::Float &Gen::rigidbodyspecs::GROUND_FRICTION(unsigned int index) const {
    ATTRIB_CODEGEN_GETLAYOUTINDEXED(EA::Reflection::Float, GROUND_FRICTION, index);
}
inline bool Gen::rigidbodyspecs::GROUND_FRICTION(EA::Reflection::Float &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(GROUND_FRICTION, result, index);
}
inline unsigned int Gen::rigidbodyspecs::Num_GROUND_FRICTION() const {
    ATTRIB_CODEGEN_GETLAYOUTLENGTH(GROUND_FRICTION);
}
inline bool Gen::rigidbodyspecs::SET_GROUND_FRICTION(const EA::Reflection::Float &input, unsigned int index) {
    ATTRIB_CODEGEN_SETLAYOUTINDEXED(GROUND_FRICTION, input, index);
}

inline bool Gen::rigidbodyspecs::GROUND_MOMENT_SCALE(TAttrib<UMath::Vector4> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0xb7dff090);
}
inline const UMath::Vector4 &Gen::rigidbodyspecs::GROUND_MOMENT_SCALE() const {
    ATTRIB_CODEGEN_GETLAYOUT(GROUND_MOMENT_SCALE);
}
inline bool Gen::rigidbodyspecs::GROUND_MOMENT_SCALE(UMath::Vector4 &result) const {
    result = GROUND_MOMENT_SCALE();
    return true;
}
inline bool Gen::rigidbodyspecs::SET_GROUND_MOMENT_SCALE(const UMath::Vector4 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(GROUND_MOMENT_SCALE, input);
}

inline bool Gen::rigidbodyspecs::IMMOBILE_OBJECT_COLLISIONS(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x4793843b);
}
inline const EA::Reflection::Bool &Gen::rigidbodyspecs::IMMOBILE_OBJECT_COLLISIONS() const {
    ATTRIB_CODEGEN_GETLAYOUT(IMMOBILE_OBJECT_COLLISIONS);
}
inline bool Gen::rigidbodyspecs::IMMOBILE_OBJECT_COLLISIONS(EA::Reflection::Bool &result) const {
    result = IMMOBILE_OBJECT_COLLISIONS();
    return true;
}
inline bool Gen::rigidbodyspecs::SET_IMMOBILE_OBJECT_COLLISIONS(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETLAYOUT(IMMOBILE_OBJECT_COLLISIONS, input);
}

inline bool Gen::rigidbodyspecs::INSTANCE_COLLISIONS_3D(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x22c5d3f9);
}
inline const EA::Reflection::Bool &Gen::rigidbodyspecs::INSTANCE_COLLISIONS_3D() const {
    ATTRIB_CODEGEN_GETLAYOUT(INSTANCE_COLLISIONS_3D);
}
inline bool Gen::rigidbodyspecs::INSTANCE_COLLISIONS_3D(EA::Reflection::Bool &result) const {
    result = INSTANCE_COLLISIONS_3D();
    return true;
}
inline bool Gen::rigidbodyspecs::SET_INSTANCE_COLLISIONS_3D(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETLAYOUT(INSTANCE_COLLISIONS_3D, input);
}

inline bool Gen::rigidbodyspecs::NATURAL_ANGULAR_DAMPING(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xae9322de);
}
inline const EA::Reflection::Float &Gen::rigidbodyspecs::NATURAL_ANGULAR_DAMPING() const {
    ATTRIB_CODEGEN_GETLAYOUT(NATURAL_ANGULAR_DAMPING);
}
inline bool Gen::rigidbodyspecs::NATURAL_ANGULAR_DAMPING(EA::Reflection::Float &result) const {
    result = NATURAL_ANGULAR_DAMPING();
    return true;
}
inline bool Gen::rigidbodyspecs::SET_NATURAL_ANGULAR_DAMPING(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(NATURAL_ANGULAR_DAMPING, input);
}

inline bool Gen::rigidbodyspecs::NO_GROUND_COLLISIONS(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x43a505ee);
}
inline const EA::Reflection::Bool &Gen::rigidbodyspecs::NO_GROUND_COLLISIONS() const {
    ATTRIB_CODEGEN_GETLAYOUT(NO_GROUND_COLLISIONS);
}
inline bool Gen::rigidbodyspecs::NO_GROUND_COLLISIONS(EA::Reflection::Bool &result) const {
    result = NO_GROUND_COLLISIONS();
    return true;
}
inline bool Gen::rigidbodyspecs::SET_NO_GROUND_COLLISIONS(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETLAYOUT(NO_GROUND_COLLISIONS, input);
}

inline bool Gen::rigidbodyspecs::NO_OBJ_COLLISIONS(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x45b11648);
}
inline const EA::Reflection::Bool &Gen::rigidbodyspecs::NO_OBJ_COLLISIONS() const {
    ATTRIB_CODEGEN_GETLAYOUT(NO_OBJ_COLLISIONS);
}
inline bool Gen::rigidbodyspecs::NO_OBJ_COLLISIONS(EA::Reflection::Bool &result) const {
    result = NO_OBJ_COLLISIONS();
    return true;
}
inline bool Gen::rigidbodyspecs::SET_NO_OBJ_COLLISIONS(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETLAYOUT(NO_OBJ_COLLISIONS, input);
}

inline bool Gen::rigidbodyspecs::NO_WORLD_COLLISIONS(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x1b5ea2c7);
}
inline const EA::Reflection::Bool &Gen::rigidbodyspecs::NO_WORLD_COLLISIONS() const {
    ATTRIB_CODEGEN_GETLAYOUT(NO_WORLD_COLLISIONS);
}
inline bool Gen::rigidbodyspecs::NO_WORLD_COLLISIONS(EA::Reflection::Bool &result) const {
    result = NO_WORLD_COLLISIONS();
    return true;
}
inline bool Gen::rigidbodyspecs::SET_NO_WORLD_COLLISIONS(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETLAYOUT(NO_WORLD_COLLISIONS, input);
}

inline bool Gen::rigidbodyspecs::OBJ_ELASTICITY(TAttrib<UMath::Vector4> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x99b357bb);
}
inline const UMath::Vector4 &Gen::rigidbodyspecs::OBJ_ELASTICITY() const {
    ATTRIB_CODEGEN_GETLAYOUT(OBJ_ELASTICITY);
}
inline bool Gen::rigidbodyspecs::OBJ_ELASTICITY(UMath::Vector4 &result) const {
    result = OBJ_ELASTICITY();
    return true;
}
inline bool Gen::rigidbodyspecs::SET_OBJ_ELASTICITY(const UMath::Vector4 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(OBJ_ELASTICITY, input);
}

inline bool Gen::rigidbodyspecs::OBJ_FRICTION(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xe1a55d9b);
}
inline const EA::Reflection::Float &Gen::rigidbodyspecs::OBJ_FRICTION(unsigned int index) const {
    ATTRIB_CODEGEN_GETLAYOUTINDEXED(EA::Reflection::Float, OBJ_FRICTION, index);
}
inline bool Gen::rigidbodyspecs::OBJ_FRICTION(EA::Reflection::Float &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(OBJ_FRICTION, result, index);
}
inline unsigned int Gen::rigidbodyspecs::Num_OBJ_FRICTION() const {
    ATTRIB_CODEGEN_GETLAYOUTLENGTH(OBJ_FRICTION);
}
inline bool Gen::rigidbodyspecs::SET_OBJ_FRICTION(const EA::Reflection::Float &input, unsigned int index) {
    ATTRIB_CODEGEN_SETLAYOUTINDEXED(OBJ_FRICTION, input, index);
}

inline bool Gen::rigidbodyspecs::OBJ_MOMENT_SCALE(TAttrib<UMath::Vector4> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0xd4167df9);
}
inline const UMath::Vector4 &Gen::rigidbodyspecs::OBJ_MOMENT_SCALE() const {
    ATTRIB_CODEGEN_GETLAYOUT(OBJ_MOMENT_SCALE);
}
inline bool Gen::rigidbodyspecs::OBJ_MOMENT_SCALE(UMath::Vector4 &result) const {
    result = OBJ_MOMENT_SCALE();
    return true;
}
inline bool Gen::rigidbodyspecs::SET_OBJ_MOMENT_SCALE(const UMath::Vector4 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(OBJ_MOMENT_SCALE, input);
}

inline bool Gen::rigidbodyspecs::SLEEP_VELOCITY(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x9f4709c6);
}
inline const EA::Reflection::Float &Gen::rigidbodyspecs::SLEEP_VELOCITY() const {
    ATTRIB_CODEGEN_GETLAYOUT(SLEEP_VELOCITY);
}
inline bool Gen::rigidbodyspecs::SLEEP_VELOCITY(EA::Reflection::Float &result) const {
    result = SLEEP_VELOCITY();
    return true;
}
inline bool Gen::rigidbodyspecs::SET_SLEEP_VELOCITY(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(SLEEP_VELOCITY, input);
}

inline bool Gen::rigidbodyspecs::WALL_ELASTICITY(TAttrib<UMath::Vector4> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0xbe3faeb0);
}
inline const UMath::Vector4 &Gen::rigidbodyspecs::WALL_ELASTICITY() const {
    ATTRIB_CODEGEN_GETLAYOUT(WALL_ELASTICITY);
}
inline bool Gen::rigidbodyspecs::WALL_ELASTICITY(UMath::Vector4 &result) const {
    result = WALL_ELASTICITY();
    return true;
}
inline bool Gen::rigidbodyspecs::SET_WALL_ELASTICITY(const UMath::Vector4 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(WALL_ELASTICITY, input);
}

inline bool Gen::rigidbodyspecs::WALL_FRICTION(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x519c53b1);
}
inline const EA::Reflection::Float &Gen::rigidbodyspecs::WALL_FRICTION(unsigned int index) const {
    ATTRIB_CODEGEN_GETLAYOUTINDEXED(EA::Reflection::Float, WALL_FRICTION, index);
}
inline bool Gen::rigidbodyspecs::WALL_FRICTION(EA::Reflection::Float &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(WALL_FRICTION, result, index);
}
inline unsigned int Gen::rigidbodyspecs::Num_WALL_FRICTION() const {
    ATTRIB_CODEGEN_GETLAYOUTLENGTH(WALL_FRICTION);
}
inline bool Gen::rigidbodyspecs::SET_WALL_FRICTION(const EA::Reflection::Float &input, unsigned int index) {
    ATTRIB_CODEGEN_SETLAYOUTINDEXED(WALL_FRICTION, input, index);
}

inline bool Gen::rigidbodyspecs::WORLD_MOMENT_SCALE(TAttrib<UMath::Vector4> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector4, 0x048fa05d);
}
inline const UMath::Vector4 &Gen::rigidbodyspecs::WORLD_MOMENT_SCALE() const {
    ATTRIB_CODEGEN_GETLAYOUT(WORLD_MOMENT_SCALE);
}
inline bool Gen::rigidbodyspecs::WORLD_MOMENT_SCALE(UMath::Vector4 &result) const {
    result = WORLD_MOMENT_SCALE();
    return true;
}
inline bool Gen::rigidbodyspecs::SET_WORLD_MOMENT_SCALE(const UMath::Vector4 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(WORLD_MOMENT_SCALE, input);
}

}; // namespace Attrib

#endif
