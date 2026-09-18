#ifndef DYNAMICS_ARTICULATION_H
#define DYNAMICS_ARTICULATION_H

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Libs/Support/Utility/UVector.h"
#include "Speed/Indep/Src/Debug/Debugable.h"
#include "Speed/Indep/Src/Physics/Dynamics.h"
#include "Speed/Indep/bWare/Inc/bList.hpp"

namespace Dynamics {
namespace Articulation {

struct Quaternion {
    float x; // offset 0x0, size 0x4
    float y; // offset 0x4, size 0x4
    float z; // offset 0x8, size 0x4
    float w; // offset 0xC, size 0x4

    Quaternion() {}

    Quaternion(const UVector3 &vec, float angle) {
        float s;
        const float a = angle * 0.5f;
        s = UMath::Sina(a);
        w = UMath::Cosa(a);
        UMath::Scale(vec, s, *reinterpret_cast<UMath::Vector3 *>(this));
    }

    Quaternion(const UMath::Vector3 &normal1, const UMath::Vector3 &normal2) {
        const float angle = UMath::Dot(normal1, normal2);
        if (angle >= 1.0f) {
            z = 0.0f;
            y = 0.0f;
            x = 0.0f;
            w = 1.0f;
        } else {
            UMath::Vector3 axis;
            UMath::Cross(normal1, normal2, axis);
            if (angle < -0.99999f) {
                x = axis.x;
                y = axis.y;
                z = axis.z;
                w = 0.0f;
                UMath::Normalize(*reinterpret_cast<UMath::Vector4 *>(this));
            } else {
                const float s = UMath::Sqrt((angle + 1.0f) + (angle + 1.0f));
                const float invs = 1.0f / s;
                x = axis.x * invs;
                y = axis.y * invs;
                z = axis.z * invs;
                w = s * 0.5f;
            }
        }
    }
};

struct HJOINT__ {
    int unused;
};
typedef struct HJOINT__ *HJOINT;

enum eJointFlags {
    JF_NONE = 0,
    JF_IMMOBILE_MALE = 1,
    JF_IMMOBILE_FEMALE = 2,
};

enum eConstraint {
    PRISMATIC = 0,
    HYPERBOLIC = 1,
    CONICAL = 2,
};

// total size: 0x1C
class Lever : public Debugable {
  public:
    Lever(IEntity *ientity, const UMath::Vector3 &arm, bool immobile) : mArm(arm), mEntity(ientity), mImmoblie(immobile) {}

    const IEntity *GetEntity() const {
        return mEntity;
    }

    bool IsImmobile() const {
        return mImmoblie;
    }

    IEntity *GetEntity() {
        return mEntity;
    }

    const UMath::Vector3 &GetArm() const {
        return mArm;
    }

    void SetFulcrum(const UVector3 &fulcrum, bool rotate);

    virtual void OnDebugDraw();

    UVector3 mArm;    // offset 0x4, size 0xC
    IEntity *mEntity; // offset 0x10, size 0x4
    bool mImmoblie;   // offset 0x14, size 0x4
};

// total size: 0x6C
class Constraint : public Debugable {
  public:
    // total size: 0x8
    struct List : public UTL::Std::list<Constraint *, _type_list> {
        List() {}
        ~List() {}
    };

    inline void *operator new(unsigned int size) {
        return gFastMem.Alloc(size, nullptr);
    }

    inline void operator delete(void *mem, unsigned int size) {
        if (mem != nullptr) {
            gFastMem.Free(mem, size, nullptr);
        }
    }

    Constraint(const UMath::Matrix4 &orient, float minTheta, float maxTheta, Lever &female, Lever &male, const UMath::Vector3 &post, eConstraint type);

    void Resolve(const UVector3 &joint_fulcrum);
    void Rotate(Lever &lever, const Quaternion &joint_rotation, const Quaternion &joint_rotationInv, const UVector3 &joint_fulcrum, float angle);
    virtual void OnDebugDraw();
    bool React(const UVector3 &joint_collision_normal, const UVector3 &joint_collision_point, const UVector3 &joint_fulcrum, const float deltaA,
               const Quaternion &joint_rotation, const Quaternion &joint_rotationInv);

    UMath::Vector4 mOrientation; // offset 0x4, size 0x10
    UVector3 mInnerN;            // offset 0x14, size 0xC
    UVector3 mInnerR;            // offset 0x20, size 0xC
    UVector3 mOuterN;            // offset 0x2C, size 0xC
    UVector3 mOuterR;            // offset 0x38, size 0xC
    Lever &mFemale;              // offset 0x44, size 0x4
    Lever &mMale;                // offset 0x48, size 0x4
    UVector3 mPost;              // offset 0x4C, size 0xC
    float mInnerA;               // offset 0x58, size 0x4
    float mOuterA;               // offset 0x5C, size 0x4
    bool mEnabled;               // offset 0x60, size 0x4
    eConstraint mType;           // offset 0x64, size 0x4
};

// total size: 0x54
class Joint : public bTNode<Joint>, public Debugable {
  public:
    inline void *operator new(unsigned int size) {
        return gFastMem.Alloc(size, nullptr);
    }

    inline void operator delete(void *mem, unsigned int size) {
        if (mem != nullptr) {
            gFastMem.Free(mem, size, nullptr);
        }
    }

    Joint(IEntity *female, const UMath::Vector3 &female_arm, IEntity *male, const UMath::Vector3 &male_arm, eJointFlags flags);
    ~Joint();
    virtual void OnDebugDraw();
    bool Owns(const IEntity *rb) const;
    void Resolve();
    void AddConstraint(IEntity *female, const UMath::Matrix4 &orient, float minTheta, float maxTheta, const UMath::Vector3 &post, eConstraint type);

    static HJOINT mNextHandle;

    Lever mFemale;               // offset 0xC, size 0x1C
    Lever mMale;                 // offset 0x28, size 0x1C
    HJOINT mHandle;              // offset 0x44, size 0x4
    Constraint::List mConstraints; // offset 0x48, size 0x8
};

void Constrain(HJOINT hJoint, IEntity *female, const UMath::Matrix4 &orient, float minTheta, float maxTheta, const UMath::Vector3 &post, eConstraint type);
bool IsJoined(const IEntity *rb0, const IEntity *rb1);
bool IsJoined(const IEntity *rb);
void Resolve();
HJOINT Create(IEntity *female, const UMath::Vector3 &female_arm, IEntity *male, const UMath::Vector3 &male_arm, eJointFlags flags);
void Release(IEntity *rb0);

} // namespace Articulation
} // namespace Dynamics

#endif
