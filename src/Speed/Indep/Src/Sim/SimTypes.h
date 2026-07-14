#ifndef SIMTYPES_H
#define SIMTYPES_H

#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"
#include "Speed/Indep/bWare/Inc/bTypes.hpp"

#define DECLARE_SIMHANDLE(name)                                                                                                                      \
    struct name##__ {                                                                                                                                \
        int unused;                                                                                                                                  \
    };                                                                                                                                               \
    typedef struct name##__ *name;

DECLARE_SIMHANDLE(HSIMABLE);
DECLARE_SIMHANDLE(HACTIVITY);
DECLARE_SIMHANDLE(HSIMSERVICE);
DECLARE_SIMHANDLE(HSIMTASK);
DECLARE_SIMHANDLE(HMODEL);
DECLARE_SIMHANDLE(HCAUSE);

// TODO are these values correct for MW?
namespace Sim {

static const unsigned int MaxPlayers = 8;

static const unsigned int MaxServers = 5;
static const unsigned int MaxRigidBodies = 64;
static const unsigned int MaxSimpleBodies = 96;
static const unsigned int MaxAnyBodies = MaxRigidBodies + MaxSimpleBodies;
static const unsigned int MaxCollisionListeners = 160;

static const unsigned int MaxActivities = 40;
static const unsigned int MaxScenery = 2048;
static const unsigned int MaxVehicles = 30;
static const unsigned int MaxSceneryFragments = 96;
static const unsigned int MaxVehicleFragments = 180;
static const unsigned int MaxFragments = 276;
static const unsigned int MaxPlaceables = 12;
static const unsigned int MaxModels = 2366;
static const unsigned int MaxConnections = 2474;
static const unsigned int MaxTasks = 1000;

} // namespace Sim

enum SimableType {
    SIMABLE_INVALID = 0,
    SIMABLE_VEHICLE = 1,
    SIMABLE_SMACKABLE = 2,
    SIMABLE_EXPLOSION = 3,
    SIMABLE_HUMAN = 4,
    SIMABLE_WEAPON = 5,
    SIMABLE_NEWTON = 6,
    SIMABLE_SENTRY = 7,
    SIMABLE_FRAGMENT = 8,
};

namespace Sim {

namespace Collision {

// total size: 0x80
struct Info {
    enum CollisionType {
        NONE = 0,
        OBJECT = 1,
        WORLD = 2,
        GROUND = 3,
    };

    Info() {
        position = UMath::Vector3::kZero;
        objAsurface = nullptr;
        normal = UMath::Vector3::kZero;
        type = 0;
        objAImmobile = 0;
        objADetached = 0;
        objBImmobile = 0;
        objBDetached = 0;
        sliding = 0;
        closingVel = UMath::Vector3::kZero;
        force = 0.0f;
        armA = UMath::Vector3::kZero;
        objA = nullptr;
        armB = UMath::Vector3::kZero;
        objB = nullptr;
        objAVel = UMath::Vector3::kZero;
        impulseA = 0.0f;
        objBVel = UMath::Vector3::kZero;
        impulseB = 0.0f;
        slidingVel = UMath::Vector3::kZero;
        objBsurface = nullptr;
    }

    CollisionType Type() const {}

    UMath::Vector3 position;               // offset 0x0, size 0xC
    const Attrib::Collection *objAsurface; // offset 0xC, size 0x4
    UMath::Vector3 normal;                 // offset 0x10, size 0xC
    int type : 3;                          // offset 0x1C, size 0x4
    int objAImmobile : 1;                  // offset 0x1C, size 0x4
    int objADetached : 1;                  // offset 0x1C, size 0x4
    int objBImmobile : 1;                  // offset 0x1C, size 0x4
    int objBDetached : 1;                  // offset 0x1C, size 0x4
    int sliding : 1;                       // offset 0x1C, size 0x4
    int unused : 24;                       // offset 0x1C, size 0x4
    UMath::Vector3 closingVel;             // offset 0x20, size 0xC
    float force;                           // offset 0x2C, size 0x4
    UMath::Vector3 armA;                   // offset 0x30, size 0xC
    HSIMABLE objA;                         // offset 0x3C, size 0x4
    UMath::Vector3 armB;                   // offset 0x40, size 0xC
    HSIMABLE objB;                         // offset 0x4C, size 0x4
    UMath::Vector3 objAVel;                // offset 0x50, size 0xC
    float impulseA;                        // offset 0x5C, size 0x4
    UMath::Vector3 objBVel;                // offset 0x60, size 0xC
    float impulseB;                        // offset 0x6C, size 0x4
    UMath::Vector3 slidingVel;             // offset 0x70, size 0xC
    const Attrib::Collection *objBsurface; // offset 0x7C, size 0x4
};

}; // namespace Collision

}; // namespace Sim

typedef Sim::Collision::Info COLLISION_INFO;

class IRigidBody;

// total size: 0x18
class SimCollisionMap {
  public:
    void Clear() {
        for (unsigned int i = 0; i < NUM_ELEMENTS(fBitMap); ++i) {
            fBitMap[i] = 0;
        }
    }

    bool TestBit(unsigned int index) const {
        return ((this->fBitMap[index / 64] >> (index % 64)) & 1) != 0;
    }

    void SetBit(unsigned int index) {
        this->fBitMap[index / Sim::MaxRigidBodies] |= 1ULL << (index & (Sim::MaxRigidBodies - 1));
    }

    bool CollisionWithAny() const {
        for (int i = 0; i < 3; ++i) {
            if (this->fBitMap[i] != 0) {
                return true;
            }
        }
        return false;
    }

    bool CollisionWithOrderedBody(int obIndex) const {
        return this->TestBit(obIndex);
    }

    void SetCollisionWithRB(int rbIndex) {
        this->SetBit(rbIndex);
    }

    void SetCollisionWithSRB(int srbIndex) {
        this->SetBit(srbIndex + Sim::MaxRigidBodies);
    }

    IRigidBody *GetRB(int rbIndex) const;
    IRigidBody *GetSRB(int srbIndex) const;
    IRigidBody *GetOrderedBody(int index) const;

  private:
    uint64 fBitMap[3]; // offset 0x0, size 0x18
};

namespace Sim {

// total size: 0x10
class Param {
  public:
    Param() : mType(UCrc32()), mName(UCrc32()), mData(nullptr) {}

    Param(const Param &from) : mType(from.mType), mName(from.mName), mData(from.mData) {}

    template <typename T> Param(UCrc32 name, const T *addr) : mType(addr->TypeName()), mName(name), mData(addr) {}

    template <typename T> const Param *Find(UCrc32 name) const {
        UCrc32 type = T::TypeName();
        if (mName == name) {
            if (mType == type) {
                return this;
            } else {
                bBreak();
            }
        }
        return nullptr;
    }

    template <typename T> const T &Fetch(UCrc32 name) const {
        const Param *p = Find<T>(name);
        return *reinterpret_cast<const T *>(p->mData);
    }

  private:
    UCrc32 mType;      // offset 0x0, size 0x4
    UCrc32 mName;      // offset 0x4, size 0x4
    const void *mData; // offset 0x8, size 0x4
    unsigned int pad;  // offset 0xC, size 0x4
};

}; // namespace Sim

#define DECLARE_SIM_PARAM(_TYPE_)                                                                                                                    \
    static UCrc32 _TYPE_::TypeName() {                                                                                                               \
        static UCrc32 value(#_TYPE_);                                                                                                                \
        return value;                                                                                                                                \
    }

enum DriverClass {
    DRIVER_HUMAN = 0,
    DRIVER_TRAFFIC = 1,
    DRIVER_COP = 2,
    DRIVER_RACER = 3,
    DRIVER_NONE = 4,
    DRIVER_NIS = 5,
    DRIVER_REMOTE = 6,
};

enum DriverStyle {
    STYLE_RACING = 0,
    STYLE_DRAG = 1,
};

enum PhysicsMode {
    PHYSICS_MODE_INACTIVE = 0,
    PHYSICS_MODE_SIMULATED = 1,
    PHYSICS_MODE_EMULATED = 2,
};

// total size: 0x28
struct InputControls {
    InputControls() {
        fBanking = 0.0f;
        fSteering = 0.0f;
        fSteeringVertical = 0.0f;
        fStrafeVertical = 0.0f;
        fStrafeHorizontal = 0.0f;
        fGas = 0.0f;
        fBrake = 0.0f;
        fHandBrake = 0.0f;
        fActionButton = false;
        fNOS = false;
    }

    float fBanking;          // offset 0x0, size 0x4
    float fSteering;         // offset 0x4, size 0x4
    float fSteeringVertical; // offset 0x8, size 0x4
    float fStrafeVertical;   // offset 0xC, size 0x4
    float fStrafeHorizontal; // offset 0x10, size 0x4
    float fGas;              // offset 0x14, size 0x4
    float fBrake;            // offset 0x18, size 0x4
    float fHandBrake;        // offset 0x1C, size 0x4
    bool fActionButton;      // offset 0x20, size 0x1
    bool fNOS;               // offset 0x24, size 0x1
};

extern Attrib::StringKey BEHAVIOR_MECHANIC_AI;
extern Attrib::StringKey BEHAVIOR_MECHANIC_ENGINE;
extern Attrib::StringKey BEHAVIOR_MECHANIC_INPUT;
extern Attrib::StringKey BEHAVIOR_MECHANIC_RIGIDBODY;
extern Attrib::StringKey BEHAVIOR_MECHANIC_DRAW;
extern Attrib::StringKey BEHAVIOR_MECHANIC_DAMAGE;
extern Attrib::StringKey BEHAVIOR_MECHANIC_SUSPENSION;

#endif
