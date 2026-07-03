#ifndef SFXOBJ_ENUMATTRIBUTES_HPP
#define SFXOBJ_ENUMATTRIBUTES_HPP

#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

enum eDRIVE_BY_TYPE {
    DRIVE_BY_UNKNOWN = 0,
    DRIVE_BY_TREE = 1,
    DRIVE_BY_LAMPPOST = 2,
    DRIVE_BY_SMOKABLE = 3,
    DRIVE_BY_TUNNEL_IN = 4,
    DRIVE_BY_TUNNEL_OUT = 5,
    DRIVE_BY_OVERPASS_IN = 6,
    DRIVE_BY_OVERPASS_OUT = 7,
    DRIVE_BY_AI_CAR = 8,
    DRIVE_BY_TRAFFIC = 9,
    DRIVE_BY_BRIDGE = 10,
    DRIVE_BY_PRE_COL = 11,
    DRIVE_BY_CAMERA_BY = 12,
    MAX_DRIVE_BY_TYPES = 13,
};

typedef eDRIVE_BY_TYPE reflection_typedef_eDRIVE_BY_TYPE;

// total size: 0x10

struct CollisionStream {
    Attrib::RefSpec StreamMoment;    // offset 0x0, size 0xC
    EA::Reflection::UInt8 Threshold; // offset 0xC, size 0x1

    CollisionStream &operator=(const CollisionStream &_ctor_arg) {}

    CollisionStream(const CollisionStream &_ctor_arg) {}
};

// total size: 0x8

struct StitchCollisionVol {
    EA::Reflection::Int16 Vol1; // offset 0x0, size 0x2
    EA::Reflection::Int16 Vol2; // offset 0x2, size 0x2
    EA::Reflection::Int16 Vol3; // offset 0x4, size 0x2
    EA::Reflection::Int16 Vol4; // offset 0x6, size 0x2
};

// total size: 0x10

struct UpgradeSpecs {
    Attrib::RefSpec Item;        // offset 0x0, size 0xC
    EA::Reflection::UInt8 Level; // offset 0xC, size 0x1

    UpgradeSpecs &operator=(const UpgradeSpecs &_ctor_arg) {}

    UpgradeSpecs(const UpgradeSpecs &_ctor_arg) {}
};

enum eCURVETYPE {
    LINEAR = 0,
    PARABOLIC = 1,
    INV_PARABOLIC = 2,
    SINUSOID = 3,
    POWER = 4,
    EQ_PWR_SQ = 5,
};

enum eENGINE_GROUP {
    eENGINE_V4 = 0,
    eENGINE_V6 = 1,
    eENGINE_V8 = 2,
};

// total size: 0x4

struct stShiftPair {
    EA::Reflection::Int16 RPM;  // offset 0x0, size 0x2
    EA::Reflection::Int16 Time; // offset 0x2, size 0x2
};

#endif
