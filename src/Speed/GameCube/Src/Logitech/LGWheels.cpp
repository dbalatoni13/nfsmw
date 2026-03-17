#include "LGWheels.hpp"

#include <string.h>

extern "C" {
void LGInit();
}

struct SpringForceParams {
    char offset;
    unsigned char saturation;
    short coefficient;
};

struct ConstantForceParams {
    short magnitude;
    unsigned short direction;
};

struct DamperForceParams {
    short coefficient;
};

struct RoadEffectParams {
    short magnitude;
};

struct SurfaceEffectParams {
    unsigned char type;
    unsigned char magnitude;
    unsigned short period;
};

extern void Wheels_Ctor(Wheels *self) asm("__6Wheels");
extern void Force_Ctor(Force *self) asm("__5Force");
extern void Condition_Ctor(Condition *self) asm("__9Condition");
extern void Constant_Ctor(Constant *self) asm("__8Constant");
extern void Periodic_Ctor(Periodic *self) asm("__8Periodic");
extern void Ramp_Ctor(Ramp *self) asm("__4Ramp");

static inline Wheels *LGWheelsGetWheels(LGWheels *self) {
    return reinterpret_cast<Wheels *>(reinterpret_cast<char *>(self) + 0x828);
}

static inline const Wheels *LGWheelsGetWheels(const LGWheels *self) {
    return reinterpret_cast<const Wheels *>(reinterpret_cast<const char *>(self) + 0x828);
}

static inline Force *LGWheelsGetForce(LGWheels *self) {
    return reinterpret_cast<Force *>(reinterpret_cast<char *>(self) + 0x10A8);
}

static inline Condition *LGWheelsGetCondition(LGWheels *self) {
    return reinterpret_cast<Condition *>(reinterpret_cast<char *>(self) + 0x11A8);
}

static inline Constant *LGWheelsGetConstant(LGWheels *self) {
    return reinterpret_cast<Constant *>(reinterpret_cast<char *>(self) + 0x12A8);
}

static inline Periodic *LGWheelsGetPeriodic(LGWheels *self) {
    return reinterpret_cast<Periodic *>(reinterpret_cast<char *>(self) + 0x13A8);
}

static inline Ramp *LGWheelsGetRamp(LGWheels *self) {
    return reinterpret_cast<Ramp *>(reinterpret_cast<char *>(self) + 0x14A8);
}

static inline unsigned long &LGWheelsGetWheelHandle(LGWheels *self, int channel) {
    return reinterpret_cast<unsigned long *>(reinterpret_cast<char *>(self) + 0x1050)[channel];
}

static inline int &LGWheelsGetDamperWasPlaying(LGWheels *self, int channel) {
    return reinterpret_cast<int *>(reinterpret_cast<char *>(self) + 0x15AC)[channel];
}

static inline int &LGWheelsGetSpringWasPlaying(LGWheels *self, int channel) {
    return reinterpret_cast<int *>(reinterpret_cast<char *>(self) + 0x15BC)[channel];
}

static inline int &LGWheelsGetIsAirborne(LGWheels *self, int channel) {
    return reinterpret_cast<int *>(reinterpret_cast<char *>(self) + 0x166C)[channel];
}

static inline unsigned char &LGWheelsGetOverallGain(LGWheels *self) {
    return *reinterpret_cast<unsigned char *>(reinterpret_cast<char *>(self) + 0x15A8);
}

static inline int &LGWheelsGetPlaying(Force *self, int channel, int forceNumber) {
    return reinterpret_cast<int *>(self)[channel * 8 + forceNumber];
}

static inline unsigned long &LGWheelsGetEffectID(Force *self, int channel, int forceNumber) {
    return reinterpret_cast<unsigned long *>(reinterpret_cast<char *>(self) + 0x80)[channel * 8 + forceNumber];
}

static inline SpringForceParams *LGWheelsGetSpringForceParams(LGWheels *self) {
    return reinterpret_cast<SpringForceParams *>(reinterpret_cast<char *>(self) + 0x167C);
}

static inline const SpringForceParams *LGWheelsGetSpringForceParams(const LGWheels *self) {
    return reinterpret_cast<const SpringForceParams *>(reinterpret_cast<const char *>(self) + 0x167C);
}

static inline ConstantForceParams *LGWheelsGetConstantForceParams(LGWheels *self) {
    return reinterpret_cast<ConstantForceParams *>(reinterpret_cast<char *>(self) + 0x168C);
}

static inline const ConstantForceParams *LGWheelsGetConstantForceParams(const LGWheels *self) {
    return reinterpret_cast<const ConstantForceParams *>(reinterpret_cast<const char *>(self) + 0x168C);
}

static inline DamperForceParams *LGWheelsGetDamperForceParams(LGWheels *self) {
    return reinterpret_cast<DamperForceParams *>(reinterpret_cast<char *>(self) + 0x169C);
}

static inline const DamperForceParams *LGWheelsGetDamperForceParams(const LGWheels *self) {
    return reinterpret_cast<const DamperForceParams *>(reinterpret_cast<const char *>(self) + 0x169C);
}

static inline RoadEffectParams *LGWheelsGetDirtRoadParams(LGWheels *self) {
    return reinterpret_cast<RoadEffectParams *>(reinterpret_cast<char *>(self) + 0x16BC);
}

static inline const RoadEffectParams *LGWheelsGetDirtRoadParams(const LGWheels *self) {
    return reinterpret_cast<const RoadEffectParams *>(reinterpret_cast<const char *>(self) + 0x16BC);
}

static inline RoadEffectParams *LGWheelsGetBumpyRoadParams(LGWheels *self) {
    return reinterpret_cast<RoadEffectParams *>(reinterpret_cast<char *>(self) + 0x16C4);
}

static inline const RoadEffectParams *LGWheelsGetBumpyRoadParams(const LGWheels *self) {
    return reinterpret_cast<const RoadEffectParams *>(reinterpret_cast<const char *>(self) + 0x16C4);
}

static inline RoadEffectParams *LGWheelsGetSlipperyRoadParams(LGWheels *self) {
    return reinterpret_cast<RoadEffectParams *>(reinterpret_cast<char *>(self) + 0x16CC);
}

static inline const RoadEffectParams *LGWheelsGetSlipperyRoadParams(const LGWheels *self) {
    return reinterpret_cast<const RoadEffectParams *>(reinterpret_cast<const char *>(self) + 0x16CC);
}

static inline SurfaceEffectParams *LGWheelsGetSurfaceEffectParams(LGWheels *self) {
    return reinterpret_cast<SurfaceEffectParams *>(reinterpret_cast<char *>(self) + 0x16D4);
}

static inline const SurfaceEffectParams *LGWheelsGetSurfaceEffectParams(const LGWheels *self) {
    return reinterpret_cast<const SurfaceEffectParams *>(reinterpret_cast<const char *>(self) + 0x16D4);
}

LGWheels::LGWheels() {
    int ii;

    Wheels_Ctor(LGWheelsGetWheels(this));
    Force_Ctor(LGWheelsGetForce(this));
    Condition_Ctor(LGWheelsGetCondition(this));
    Constant_Ctor(LGWheelsGetConstant(this));
    Periodic_Ctor(LGWheelsGetPeriodic(this));
    Ramp_Ctor(LGWheelsGetRamp(this));
    LGInit();
    LGWheelsGetOverallGain(this) = 0xFF;

    for (ii = 0; ii < 4; ii++) {
        InitVars(ii);
    }
}

void LGWheels::InitVars(int channel) {
    int ii;

    LGWheelsGetIsAirborne(this, channel) = 0;
    LGWheelsGetDamperWasPlaying(this, channel) = 0;
    LGWheelsGetSpringWasPlaying(this, channel) = 0;

    for (ii = 0; ii < 8; ii++) {
        LGWheelsGetEffectID(LGWheelsGetCondition(this), channel, ii) = static_cast<unsigned long>(-1);
        LGWheelsGetPlaying(LGWheelsGetCondition(this), channel, ii) = 0;
        LGWheelsGetEffectID(LGWheelsGetConstant(this), channel, ii) = static_cast<unsigned long>(-1);
        LGWheelsGetPlaying(LGWheelsGetConstant(this), channel, ii) = 0;
        LGWheelsGetEffectID(LGWheelsGetPeriodic(this), channel, ii) = static_cast<unsigned long>(-1);
        LGWheelsGetPlaying(LGWheelsGetPeriodic(this), channel, ii) = 0;
        LGWheelsGetEffectID(LGWheelsGetRamp(this), channel, ii) = static_cast<unsigned long>(-1);
        LGWheelsGetPlaying(LGWheelsGetRamp(this), channel, ii) = 0;
    }
}

void LGWheels::ReadAll() {
    short wheelUnplugged;

    wheelUnplugged = LGWheelsGetWheels(this)->ReadAll();
    memcpy(this, LGWheelsGetWheels(this), sizeof(LGPosition) * 4);
    if (wheelUnplugged != -1) {
        InitVars(wheelUnplugged);
    }
}

bool LGWheels::IsConnected(int channel) {
    return LGWheelsGetWheels(this)->IsConnected(channel);
}

bool LGWheels::ButtonIsPressed(int channel, unsigned long buttonMask) {
    return LGWheelsGetWheels(this)->ButtonIsPressed(channel, buttonMask);
}

bool LGWheels::PedalsConnected(int channel) {
    return LGWheelsGetWheels(this)->PedalsConnected(channel);
}

void LGWheels::PlayAutoCalibAndSpringForce(int channel) {
    if (LGWheelsGetWheels(this)->IsConnected(channel) && !LGWheelsGetIsAirborne(this, channel)) {
        if (LGWheelsGetEffectID(LGWheelsGetPeriodic(this), channel, 4) == static_cast<unsigned long>(-1)) {
            LGWheelsGetPeriodic(this)->DownloadForce(channel, 4, LGWheelsGetWheelHandle(this, channel), 3, 2200, 0, 180, 90, 2200, 0, 0, 0, 0, 0, 0);
            LGWheelsGetPeriodic(this)->Start(channel, 4);
        }

        if (LGWheelsGetEffectID(LGWheelsGetCondition(this), channel, 0) == static_cast<unsigned long>(-1)) {
            LGWheelsGetCondition(this)->DownloadForce(channel, 0, LGWheelsGetWheelHandle(this, channel), 7, static_cast<unsigned long>(-1), 2200, 0, 0, 180, 180, 180, 180);
            LGWheelsGetCondition(this)->Start(channel, 0);
        }
    }
}

void LGWheels::StopSpringForce(int channel) {
    this->StopForce(channel, 0);
}

bool LGWheels::SameSpringForceParams(int channel, char offset, unsigned char saturation, short coefficient) {
    const SpringForceParams &params = LGWheelsGetSpringForceParams(this)[channel];
    return params.offset == offset && params.saturation == saturation && params.coefficient == coefficient;
}

void LGWheels::StopConstantForce(int channel) {
    this->StopForce(channel, 1);
}

bool LGWheels::SameConstantForceParams(int channel, short magnitude, unsigned short direction) {
    const ConstantForceParams &params = LGWheelsGetConstantForceParams(this)[channel];
    return params.magnitude == magnitude && params.direction == direction;
}

void LGWheels::StopDamperForce(int channel) {
    this->StopForce(channel, 2);
}

bool LGWheels::SameDamperForceParams(int channel, short coefficient) {
    return LGWheelsGetDamperForceParams(this)[channel].coefficient == coefficient;
}

void LGWheels::StopDirtRoadEffect(int channel) {
    this->StopForce(channel, 5);
}

bool LGWheels::SameDirtRoadEffectParams(int channel, short magnitude) {
    return LGWheelsGetDirtRoadParams(this)[channel].magnitude == magnitude;
}

void LGWheels::StopBumpyRoadEffect(int channel) {
    this->StopForce(channel, 6);
}

bool LGWheels::SameBumpyRoadEffectParams(int channel, short magnitude) {
    return LGWheelsGetBumpyRoadParams(this)[channel].magnitude == magnitude;
}

void LGWheels::StopSlipperyRoadEffect(int channel) {
    this->StopForce(channel, 7);
}

bool LGWheels::SameSlipperyRoadEffectParams(int channel, short magnitude) {
    return LGWheelsGetSlipperyRoadParams(this)[channel].magnitude == magnitude;
}

void LGWheels::StopSurfaceEffect(int channel) {
    this->StopForce(channel, 8);
}

bool LGWheels::SameSurfaceEffectParams(int channel, unsigned char type, unsigned char magnitude, unsigned short period) {
    const SurfaceEffectParams &params = LGWheelsGetSurfaceEffectParams(this)[channel];
    return params.type == type && params.magnitude == magnitude && params.period == period;
}

void LGWheels::StopCarAirborne(int channel) {
    this->StopForce(channel, 9);
}
