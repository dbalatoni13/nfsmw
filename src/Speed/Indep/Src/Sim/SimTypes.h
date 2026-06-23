#ifndef SIMTYPES_H
#define SIMTYPES_H

#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"

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

// total size: 0x18
class SimCollisionMap {
  public:
    class IRigidBody *GetRB(int rbIndex) const;
    class IRigidBody *GetSRB(int srbIndex) const;
    class IRigidBody *GetOrderedBody(int index) const;

    void Clear() {
        for (unsigned int i = 0; i < 3; ++i) {
            fBitMap[i] = 0;
        }
    }

  private:
    unsigned long long fBitMap[3]; // offset 0x0, size 0x18
};

enum DriverClass {
    DRIVER_HUMAN = 0,
    DRIVER_TRAFFIC = 1,
    DRIVER_COP = 2,
    DRIVER_RACER = 3,
    DRIVER_NONE = 4,
    DRIVER_NIS = 5,
    DRIVER_REMOTE = 6,
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
