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

static const char kPlayForceError[] = "ERROR: trying to play a force on channel %d but no wheel opened.
";

static inline Wheels *LGWheelsGetWheels(LGWheels *self) {
    return reinterpret_cast<Wheels *>(reinterpret_cast<char *>(self) + 0x828);
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

static inline int &LGWheelsGetWasPlayingBeforeAirborne(LGWheels *self, int channel, int forceType) {
    return reinterpret_cast<int *>(reinterpret_cast<char *>(self) + 0x15CC + channel * 0x28)[forceType];
}

static inline int &LGWheelsGetIsAirborne(LGWheels *self, int channel) {
    return reinterpret_cast<int *>(reinterpret_cast<char *>(self) + 0x166C)[channel];
}

static inline unsigned char &LGWheelsGetOverallGain(LGWheels *self) {
    return *reinterpret_cast<unsigned char *>(reinterpret_cast<char *>(self) + 0x15A8);
}

static inline int &LGWheelsGetPlaying(Force *self, int channel, int forceNumber) {
    return *(reinterpret_cast<int *>(self) + forceNumber + channel * 8);
}

static inline unsigned long &LGWheelsGetEffectID(Force *self, int channel, int forceNumber) {
    return *(reinterpret_cast<unsigned long *>(reinterpret_cast<char *>(self) + 0x80) + forceNumber + channel * 8);
}

static inline SpringForceParams *LGWheelsGetSpringForceParams(LGWheels *self) {
    return reinterpret_cast<SpringForceParams *>(reinterpret_cast<char *>(self) + 0x167C);
}

static inline ConstantForceParams *LGWheelsGetConstantForceParams(LGWheels *self) {
    return reinterpret_cast<ConstantForceParams *>(reinterpret_cast<char *>(self) + 0x168C);
}

static inline DamperForceParams *LGWheelsGetDamperForceParams(LGWheels *self) {
    return reinterpret_cast<DamperForceParams *>(reinterpret_cast<char *>(self) + 0x169C);
}

static inline RoadEffectParams *LGWheelsGetFrontalCollisionParams(LGWheels *self) {
    return reinterpret_cast<RoadEffectParams *>(reinterpret_cast<char *>(self) + 0x16B4);
}

static inline RoadEffectParams *LGWheelsGetDirtRoadParams(LGWheels *self) {
    return reinterpret_cast<RoadEffectParams *>(reinterpret_cast<char *>(self) + 0x16BC);
}

static inline RoadEffectParams *LGWheelsGetBumpyRoadParams(LGWheels *self) {
    return reinterpret_cast<RoadEffectParams *>(reinterpret_cast<char *>(self) + 0x16C4);
}

static inline RoadEffectParams *LGWheelsGetSlipperyRoadParams(LGWheels *self) {
    return reinterpret_cast<RoadEffectParams *>(reinterpret_cast<char *>(self) + 0x16CC);
}

static inline SurfaceEffectParams *LGWheelsGetSurfaceEffectParams(LGWheels *self) {
    return reinterpret_cast<SurfaceEffectParams *>(reinterpret_cast<char *>(self) + 0x16D4);
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

void LGWheels::InitVars(long channel) {
    int ii;
    int channelOffset = channel * 4;
    int forceOffset = channel * 0x20;
    char *isAirborne = reinterpret_cast<char *>(this) + 0x166C;
    char *damperWasPlaying = reinterpret_cast<char *>(this) + 0x15AC;
    char *springWasPlaying = reinterpret_cast<char *>(this) + 0x15BC;
    char *conditionEffectID = reinterpret_cast<char *>(this) + 0x1228;
    char *conditionPlaying = reinterpret_cast<char *>(this) + 0x11A8;
    char *constantEffectID = reinterpret_cast<char *>(this) + 0x1328;
    char *constantPlaying = reinterpret_cast<char *>(this) + 0x12A8;
    char *periodicEffectID = reinterpret_cast<char *>(this) + 0x1428;
    char *periodicPlaying = reinterpret_cast<char *>(this) + 0x13A8;
    char *rampEffectID = reinterpret_cast<char *>(this) + 0x1528;
    char *rampPlaying = reinterpret_cast<char *>(this) + 0x14A8;

    *reinterpret_cast<int *>(isAirborne + channelOffset) = 0;
    *reinterpret_cast<int *>(damperWasPlaying + channelOffset) = 0;
    *reinterpret_cast<int *>(springWasPlaying + channelOffset) = 0;

    for (ii = 0; ii < 8; ii++) {
        int offset = ii * 4 + forceOffset;

        *reinterpret_cast<unsigned long *>(conditionEffectID + offset) = static_cast<unsigned long>(-1);
        *reinterpret_cast<int *>(conditionPlaying + offset) = 0;
        *reinterpret_cast<unsigned long *>(constantEffectID + offset) = static_cast<unsigned long>(-1);
        *reinterpret_cast<int *>(constantPlaying + offset) = 0;
        *reinterpret_cast<unsigned long *>(periodicEffectID + offset) = static_cast<unsigned long>(-1);
        *reinterpret_cast<int *>(periodicPlaying + offset) = 0;
        *reinterpret_cast<unsigned long *>(rampEffectID + offset) = static_cast<unsigned long>(-1);
        *reinterpret_cast<int *>(rampPlaying + offset) = 0;
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

void LGWheels::StopForce(long channel, long forceType) {
    switch (forceType) {
    case 0:
        if (IsPlaying(channel, 0)) {
            LGWheelsGetCondition(this)->Stop(channel, 0);
        }
        break;
    case 1:
        if (IsPlaying(channel, 1)) {
            LGWheelsGetConstant(this)->Stop(channel, 0);
        }
        break;
    case 2:
        if (IsPlaying(channel, 2)) {
            LGWheelsGetCondition(this)->Stop(channel, 1);
        }
        break;
    case 3:
        if (IsPlaying(channel, 3)) {
            LGWheelsGetConstant(this)->Stop(channel, 1);
        }
        break;
    case 4:
        if (IsPlaying(channel, 4)) {
            LGWheelsGetPeriodic(this)->Stop(channel, 0);
        }
        break;
    case 5:
        if (IsPlaying(channel, 5)) {
            LGWheelsGetPeriodic(this)->Stop(channel, 1);
        }
        break;
    case 6:
        if (IsPlaying(channel, 6)) {
            LGWheelsGetPeriodic(this)->Stop(channel, 2);
        }
        break;
    case 7:
        if (IsPlaying(channel, 7)) {
            LGWheelsGetCondition(this)->Stop(channel, 2);
        }
        if (LGWheelsGetDamperWasPlaying(this, channel)) {
            PlayDamperForce(channel, LGWheelsGetDamperForceParams(this)[channel].coefficient);
            LGWheelsGetPlaying(LGWheelsGetCondition(this), channel, 1) = 1;
            LGWheelsGetDamperWasPlaying(this, channel) = 0;
        }
        if (LGWheelsGetSpringWasPlaying(this, channel)) {
            PlaySpringForce(channel, LGWheelsGetSpringForceParams(this)[channel].offset, LGWheelsGetSpringForceParams(this)[channel].saturation, LGWheelsGetSpringForceParams(this)[channel].coefficient);
            LGWheelsGetPlaying(LGWheelsGetCondition(this), channel, 0) = 1;
            LGWheelsGetSpringWasPlaying(this, channel) = 0;
        }
        break;
    case 8:
        if (IsPlaying(channel, 8)) {
            LGWheelsGetPeriodic(this)->Stop(channel, 3);
        }
        break;
    case 9:
        if (IsPlaying(channel, 9)) {
            LGWheelsGetIsAirborne(this, channel) = 0;
            if (LGWheelsGetWasPlayingBeforeAirborne(this, channel, 0) == 1) {
                PlaySpringForce(channel, LGWheelsGetSpringForceParams(this)[channel].offset, LGWheelsGetSpringForceParams(this)[channel].saturation, LGWheelsGetSpringForceParams(this)[channel].coefficient);
            }
            if (LGWheelsGetWasPlayingBeforeAirborne(this, channel, 1) == 1) {
                PlayConstantForce(channel, LGWheelsGetConstantForceParams(this)[channel].magnitude, LGWheelsGetConstantForceParams(this)[channel].direction);
            }
            if (LGWheelsGetWasPlayingBeforeAirborne(this, channel, 2) == 1) {
                PlayDamperForce(channel, LGWheelsGetDamperForceParams(this)[channel].coefficient);
            }
            if (LGWheelsGetWasPlayingBeforeAirborne(this, channel, 5) == 1) {
                PlayDirtRoadEffect(channel, static_cast<unsigned char>(LGWheelsGetDirtRoadParams(this)[channel].magnitude));
            }
            if (LGWheelsGetWasPlayingBeforeAirborne(this, channel, 6) == 1) {
                PlayBumpyRoadEffect(channel, static_cast<unsigned char>(LGWheelsGetBumpyRoadParams(this)[channel].magnitude));
            }
            if (LGWheelsGetWasPlayingBeforeAirborne(this, channel, 7) == 1) {
                PlaySlipperyRoadEffect(channel, LGWheelsGetSlipperyRoadParams(this)[channel].magnitude);
            }
            if (LGWheelsGetWasPlayingBeforeAirborne(this, channel, 8) == 1) {
                PlaySurfaceEffect(channel, LGWheelsGetSurfaceEffectParams(this)[channel].type, LGWheelsGetSurfaceEffectParams(this)[channel].magnitude, LGWheelsGetSurfaceEffectParams(this)[channel].period);
            }
            {
                int jj;

                for (jj = 0; jj < 10; jj++) {
                    LGWheelsGetWasPlayingBeforeAirborne(this, channel, jj) = 0;
                }
            }
        }
        break;
    }
}

bool LGWheels::IsConnected(long channel) {
    return LGWheelsGetWheels(this)->IsConnected(channel);
}

bool LGWheels::IsPlaying(long channel, long forceType) {
    switch (forceType) {
    case 0:
        return LGWheelsGetPlaying(LGWheelsGetCondition(this), channel, 0) != 0;
    case 1:
        return LGWheelsGetPlaying(LGWheelsGetConstant(this), channel, 0) != 0;
    case 2:
        return LGWheelsGetPlaying(LGWheelsGetCondition(this), channel, 1) != 0;
    case 3:
        return LGWheelsGetPlaying(LGWheelsGetConstant(this), channel, 1) != 0;
    case 4:
        return LGWheelsGetPlaying(LGWheelsGetPeriodic(this), channel, 0) != 0;
    case 5:
        return LGWheelsGetPlaying(LGWheelsGetPeriodic(this), channel, 1) != 0;
    case 6:
        return LGWheelsGetPlaying(LGWheelsGetPeriodic(this), channel, 2) != 0;
    case 7:
        return LGWheelsGetPlaying(LGWheelsGetCondition(this), channel, 2) != 0;
    case 8:
        return LGWheelsGetPlaying(LGWheelsGetPeriodic(this), channel, 3) != 0;
    case 9:
        return LGWheelsGetIsAirborne(this, channel) == 1;
    }

    return false;
}

bool LGWheels::ButtonIsPressed(long channel, unsigned long buttonMask) {
    return LGWheelsGetWheels(this)->ButtonIsPressed(channel, buttonMask);
}

bool LGWheels::PedalsConnected(long channel) {
    return LGWheelsGetWheels(this)->PedalsConnected(channel);
}

void LGWheels::PlayAutoCalibAndSpringForce(long channel) {
    if (LGWheelsGetWheels(this)->IsConnected(channel) && *reinterpret_cast<int *>(reinterpret_cast<char *>(this) + channel * 4 + 0x166C) == 0) {
        if (*reinterpret_cast<unsigned long *>(reinterpret_cast<char *>(this) + channel * 0x20 + 0x1438) == static_cast<unsigned long>(-1)) {
            LGWheelsGetPeriodic(this)->DownloadForce(channel, 4, LGWheelsGetWheelHandle(this, channel), 3, 2200, 0, 180, 90, 2200, 0, 0, 0, 0, 0, 0);
            LGWheelsGetPeriodic(this)->Start(channel, 4);
        }

        if (*reinterpret_cast<unsigned long *>(reinterpret_cast<char *>(this) + channel * 0x20 + 0x1228) == static_cast<unsigned long>(-1)) {
            LGWheelsGetCondition(this)->DownloadForce(channel, 0, LGWheelsGetWheelHandle(this, channel), 7, static_cast<unsigned long>(-1), 2200, 0, 0, 180, 180, 180, 180);
            LGWheelsGetCondition(this)->Start(channel, 0);
        }
    }
}

void LGWheels::PlaySpringForce(long channel, signed char offset, unsigned char saturation, short coefficient) {
    int ret;

    if (*reinterpret_cast<int *>(reinterpret_cast<char *>(this) + channel * 0x20 + 0x11B0) != 0) {
        return;
    }

    if (reinterpret_cast<Wheels *>(reinterpret_cast<char *>(this) + 0x828)->IsConnected(channel)) {
        if (*reinterpret_cast<int *>(reinterpret_cast<char *>(this) + channel * 4 + 0x166C) != 0) {
            return;
        }

        if (reinterpret_cast<Condition *>(reinterpret_cast<char *>(this) + 0x11A8)->Playing[channel][0] != 0) {
            if (SameSpringForceParams(channel, offset, saturation, coefficient)) {
                return;
            }

            ret = reinterpret_cast<Condition *>(reinterpret_cast<char *>(this) + 0x11A8)->UpdateForce(channel, 0, 7, static_cast<unsigned long>(-1), 0, offset, 0, saturation, saturation, coefficient, coefficient);
            if (ret < 0) {
                return;
            }

            reinterpret_cast<SpringForceParams *>(reinterpret_cast<char *>(this) + 0x167C)[channel].offset = offset;
            reinterpret_cast<SpringForceParams *>(reinterpret_cast<char *>(this) + 0x167C)[channel].saturation = saturation;
            reinterpret_cast<SpringForceParams *>(reinterpret_cast<char *>(this) + 0x167C)[channel].coefficient = coefficient;
            return;
        }

        if (reinterpret_cast<Condition *>(reinterpret_cast<char *>(this) + 0x11A8)->EffectID[channel][0] == static_cast<unsigned long>(-1)) {
            ret = reinterpret_cast<Condition *>(reinterpret_cast<char *>(this) + 0x11A8)
                      ->DownloadForce(channel, 0, reinterpret_cast<unsigned long *>(reinterpret_cast<char *>(this) + 0x1050)[channel], 7, static_cast<unsigned long>(-1), 0, offset, 0, saturation, saturation, coefficient, coefficient);
        } else if (SameSpringForceParams(channel, offset, saturation, coefficient)) {
            reinterpret_cast<Condition *>(reinterpret_cast<char *>(this) + 0x11A8)->Start(channel, 0);
            return;
        } else {
            ret = reinterpret_cast<Condition *>(reinterpret_cast<char *>(this) + 0x11A8)->UpdateForce(channel, 0, 7, static_cast<unsigned long>(-1), 0, offset, 0, saturation, saturation, coefficient, coefficient);
        }

        if (ret >= 0) {
            reinterpret_cast<SpringForceParams *>(reinterpret_cast<char *>(this) + 0x167C)[channel].offset = offset;
            reinterpret_cast<SpringForceParams *>(reinterpret_cast<char *>(this) + 0x167C)[channel].saturation = saturation;
            reinterpret_cast<SpringForceParams *>(reinterpret_cast<char *>(this) + 0x167C)[channel].coefficient = coefficient;
        }

        reinterpret_cast<Condition *>(reinterpret_cast<char *>(this) + 0x11A8)->Start(channel, 0);
    } else {
        OSReport(kPlayForceError, channel);
    }
}

void LGWheels::StopSpringForce(long channel) {
    this->StopForce(channel, 0);
}

bool LGWheels::SameSpringForceParams(long channel, signed char offset, unsigned char saturation, short coefficient) {
    const char *channelParams = reinterpret_cast<const char *>(this) + channel * 4;
    return *reinterpret_cast<const signed char *>(channelParams + 0x167C) == offset &&
           *reinterpret_cast<const unsigned char *>(channelParams + 0x167D) == saturation &&
           *reinterpret_cast<const short *>(channelParams + 0x167E) == coefficient;
}

void LGWheels::PlayConstantForce(long channel, short magnitude, unsigned short direction) {
    int ret;

    if (LGWheelsGetWheels(this)->IsConnected(channel)) {
        if (LGWheelsGetIsAirborne(this, channel)) {
            return;
        }

        if (LGWheelsGetPlaying(LGWheelsGetConstant(this), channel, 0) != 0) {
            if (SameConstantForceParams(channel, magnitude, direction)) {
                return;
            }

            ret = LGWheelsGetConstant(this)->UpdateForce(channel, 0, static_cast<unsigned long>(-1), 0, magnitude, direction, 0, 0, 0, 0);
            if (ret < 0) {
                return;
            }

            LGWheelsGetConstantForceParams(this)[channel].magnitude = magnitude;
            LGWheelsGetConstantForceParams(this)[channel].direction = direction;
            return;
        }

        if (LGWheelsGetEffectID(LGWheelsGetConstant(this), channel, 0) == static_cast<unsigned long>(-1)) {
            ret = LGWheelsGetConstant(this)->DownloadForce(channel, 0, LGWheelsGetWheelHandle(this, channel), static_cast<unsigned long>(-1), 0, magnitude, direction, 0, 0, 0, 0);
        } else if (SameConstantForceParams(channel, magnitude, direction)) {
            LGWheelsGetConstant(this)->Start(channel, 0);
            return;
        } else {
            ret = LGWheelsGetConstant(this)->UpdateForce(channel, 0, static_cast<unsigned long>(-1), 0, magnitude, direction, 0, 0, 0, 0);
        }

        if (ret >= 0) {
            LGWheelsGetConstantForceParams(this)[channel].magnitude = magnitude;
            LGWheelsGetConstantForceParams(this)[channel].direction = direction;
        }

        LGWheelsGetConstant(this)->Start(channel, 0);
    } else {
        OSReport(kPlayForceError, channel);
    }
}

void LGWheels::StopConstantForce(long channel) {
    this->StopForce(channel, 1);
}

bool LGWheels::SameConstantForceParams(long channel, short magnitude, unsigned short direction) {
    const ConstantForceParams &params = LGWheelsGetConstantForceParams(this)[channel];
    return params.magnitude == magnitude && params.direction == direction;
}

void LGWheels::PlayDamperForce(long channel, short coefficient) {
    int ret;

    if (*reinterpret_cast<int *>(reinterpret_cast<char *>(this) + channel * 0x20 + 0x11B0) != 0) {
        return;
    }

    if (reinterpret_cast<Wheels *>(reinterpret_cast<char *>(this) + 0x828)->IsConnected(channel)) {
        if (*reinterpret_cast<int *>(reinterpret_cast<char *>(this) + channel * 4 + 0x166C) != 0) {
            return;
        }

        if (*reinterpret_cast<int *>(reinterpret_cast<char *>(this) + channel * 0x20 + 0x11AC) != 0) {
            if (SameDamperForceParams(channel, coefficient)) {
                return;
            }

            ret = reinterpret_cast<Condition *>(reinterpret_cast<char *>(this) + 0x11A8)->UpdateForce(channel, 1, 8, static_cast<unsigned long>(-1), 0, 0, 0xFF, 0xFF, 0xFF, coefficient, coefficient);
            if (ret < 0) {
                return;
            }

            *reinterpret_cast<short *>(reinterpret_cast<char *>(this) + channel * 2 + 0x169C) = coefficient;
            return;
        }

        if (*reinterpret_cast<unsigned long *>(reinterpret_cast<char *>(this) + channel * 0x20 + 0x122C) == static_cast<unsigned long>(-1)) {
            ret = reinterpret_cast<Condition *>(reinterpret_cast<char *>(this) + 0x11A8)
                      ->DownloadForce(channel, 1, *reinterpret_cast<unsigned long *>(reinterpret_cast<char *>(this) + channel * 4 + 0x1050), 8, static_cast<unsigned long>(-1), 0, 0, 0xFF, 0xFF, 0xFF, coefficient, coefficient);
        } else if (SameDamperForceParams(channel, coefficient)) {
            reinterpret_cast<Condition *>(reinterpret_cast<char *>(this) + 0x11A8)->Start(channel, 1);
            return;
        } else {
            ret = reinterpret_cast<Condition *>(reinterpret_cast<char *>(this) + 0x11A8)->UpdateForce(channel, 1, 8, static_cast<unsigned long>(-1), 0, 0, 0xFF, 0xFF, 0xFF, coefficient, coefficient);
        }

        if (ret >= 0) {
            *reinterpret_cast<short *>(reinterpret_cast<char *>(this) + channel * 2 + 0x169C) = coefficient;
        }

        reinterpret_cast<Condition *>(reinterpret_cast<char *>(this) + 0x11A8)->Start(channel, 1);
    } else {
        OSReport(kPlayForceError, channel);
    }
}

void LGWheels::StopDamperForce(long channel) {
    this->StopForce(channel, 2);
}

bool LGWheels::SameDamperForceParams(long channel, short coefficient) {
    const short *channelCoefficient = &LGWheelsGetDamperForceParams(this)[channel].coefficient;
    return *channelCoefficient == coefficient;
}

void LGWheels::PlayFrontalCollisionForce(long channel, unsigned char magnitude) {
    int ret;

    if (LGWheelsGetWheels(this)->IsConnected(channel)) {
        if (LGWheelsGetPlaying(LGWheelsGetPeriodic(this), channel, 0) != 0) {
            if (!SameFrontalCollisionForceParams(channel, magnitude)) {
                ret = LGWheelsGetPeriodic(this)->UpdateForce(channel, 0, 3, 150, 0, magnitude, 90, 75, 0, 0, 20, 0, 0, 0);
                if (ret >= 0) {
                    LGWheelsGetFrontalCollisionParams(this)[channel].magnitude = magnitude;
                }
            }
            LGWheelsGetPeriodic(this)->Start(channel, 0);
            return;
        }

        if (LGWheelsGetEffectID(LGWheelsGetPeriodic(this), channel, 0) == static_cast<unsigned long>(-1)) {
            ret = LGWheelsGetPeriodic(this)->DownloadForce(channel, 0, LGWheelsGetWheelHandle(this, channel), 3, 150, 0, magnitude, 90, 75, 0, 0, 20, 0, 0, 0);
        } else if (SameFrontalCollisionForceParams(channel, magnitude)) {
            ret = 0;
        } else {
            ret = LGWheelsGetPeriodic(this)->UpdateForce(channel, 0, 3, 150, 0, magnitude, 90, 75, 0, 0, 20, 0, 0, 0);
        }

        if (ret >= 0) {
            LGWheelsGetFrontalCollisionParams(this)[channel].magnitude = magnitude;
        }

        LGWheelsGetPeriodic(this)->Start(channel, 0);
    } else {
        OSReport(kPlayForceError, channel);
    }
}

bool LGWheels::SameFrontalCollisionForceParams(long channel, short magnitude) {
    const short *channelMagnitude = &LGWheelsGetFrontalCollisionParams(this)[channel].magnitude;
    return *channelMagnitude == magnitude;
}

void LGWheels::PlayDirtRoadEffect(long channel, unsigned char magnitude) {
    int ret;
    Periodic *periodic = LGWheelsGetPeriodic(this);

    if (LGWheelsGetWheels(this)->IsConnected(channel)) {
        if (LGWheelsGetIsAirborne(this, channel)) {
            return;
        }

        if (LGWheelsGetPlaying(periodic, channel, 1) != 0) {
            if (SameDirtRoadEffectParams(channel, magnitude)) {
                return;
            }

            ret = periodic->UpdateForce(channel, 1, 2, static_cast<unsigned long>(-1), 0, magnitude, 90, 65, 0, 0, 0, 0, 0, 0);
            if (ret < 0) {
                return;
            }

            LGWheelsGetDirtRoadParams(this)[channel].magnitude = magnitude;
            return;
        }

        if (LGWheelsGetEffectID(periodic, channel, 1) == static_cast<unsigned long>(-1)) {
            ret = periodic->DownloadForce(
                channel, 1, LGWheelsGetWheelHandle(this, channel), 2, static_cast<unsigned long>(-1), 0, magnitude, 90,
                65, 0, 0, 0, 0, 0, 0);
        } else if (SameDirtRoadEffectParams(channel, magnitude)) {
            periodic->Start(channel, 1);
            return;
        } else {
            ret = periodic->UpdateForce(channel, 1, 2, static_cast<unsigned long>(-1), 0, magnitude, 90, 65, 0, 0, 0, 0, 0, 0);
        }

        if (ret >= 0) {
            LGWheelsGetDirtRoadParams(this)[channel].magnitude = magnitude;
        }

        periodic->Start(channel, 1);
    } else {
        OSReport(kPlayForceError, channel);
    }
}

void LGWheels::StopDirtRoadEffect(long channel) {
    this->StopForce(channel, 5);
}

bool LGWheels::SameDirtRoadEffectParams(long channel, short magnitude) {
    const short *channelMagnitude = &LGWheelsGetDirtRoadParams(this)[channel].magnitude;
    return *channelMagnitude == magnitude;
}

void LGWheels::PlayBumpyRoadEffect(long channel, unsigned char magnitude) {
    int ret;
    Periodic *periodic = LGWheelsGetPeriodic(this);

    if (LGWheelsGetWheels(this)->IsConnected(channel)) {
        if (LGWheelsGetIsAirborne(this, channel)) {
            return;
        }

        if (LGWheelsGetPlaying(periodic, channel, 2) != 0) {
            if (SameBumpyRoadEffectParams(channel, magnitude)) {
                return;
            }

            ret = periodic->UpdateForce(channel, 2, 3, static_cast<unsigned long>(-1), 0, magnitude, 90, 100, 0, 0, 0, 0, 0, 0);
            if (ret < 0) {
                return;
            }

            LGWheelsGetBumpyRoadParams(this)[channel].magnitude = magnitude;
            return;
        }

        if (LGWheelsGetEffectID(periodic, channel, 2) == static_cast<unsigned long>(-1)) {
            ret = periodic->DownloadForce(
                channel, 2, LGWheelsGetWheelHandle(this, channel), 3, static_cast<unsigned long>(-1), 0, magnitude,
                90, 100, 0, 0, 0, 0, 0, 0);
        } else if (SameBumpyRoadEffectParams(channel, magnitude)) {
            periodic->Start(channel, 2);
            return;
        } else {
            ret = periodic->UpdateForce(channel, 2, 3, static_cast<unsigned long>(-1), 0, magnitude, 90, 100, 0, 0, 0, 0, 0, 0);
        }

        if (ret >= 0) {
            LGWheelsGetBumpyRoadParams(this)[channel].magnitude = magnitude;
        }

        periodic->Start(channel, 2);
    } else {
        OSReport(kPlayForceError, channel);
    }
}

void LGWheels::StopBumpyRoadEffect(long channel) {
    this->StopForce(channel, 6);
}

bool LGWheels::SameBumpyRoadEffectParams(long channel, short magnitude) {
    const short *channelMagnitude = &LGWheelsGetBumpyRoadParams(this)[channel].magnitude;
    return *channelMagnitude == magnitude;
}

void LGWheels::PlaySlipperyRoadEffect(long channel, short magnitude) {
    int ret;

    if (IsPlaying(channel, 2)) {
        StopDamperForce(channel);
        *reinterpret_cast<int *>(reinterpret_cast<char *>(this) + channel * 0x20 + 0x11AC) = 0;
        *reinterpret_cast<int *>(reinterpret_cast<char *>(this) + channel * 4 + 0x15AC) = 1;
    }

    if (IsPlaying(channel, 0)) {
        StopSpringForce(channel);
        *reinterpret_cast<int *>(reinterpret_cast<char *>(this) + channel * 0x20 + 0x11A8) = 0;
        *reinterpret_cast<int *>(reinterpret_cast<char *>(this) + channel * 4 + 0x15BC) = 1;
    }

    if (reinterpret_cast<Wheels *>(reinterpret_cast<char *>(this) + 0x828)->IsConnected(channel)) {
        if (*reinterpret_cast<int *>(reinterpret_cast<char *>(this) + channel * 4 + 0x166C) != 0) {
            return;
        }

        if (*reinterpret_cast<int *>(reinterpret_cast<char *>(this) + channel * 0x20 + 0x11B0) != 0) {
            if (SameSlipperyRoadEffectParams(channel, magnitude)) {
                return;
            }

            ret = reinterpret_cast<Condition *>(reinterpret_cast<char *>(this) + 0x11A8)->UpdateForce(channel, 2, 8, static_cast<unsigned long>(-1), 0, 0, 0, 0xFF, 0xFF, -magnitude, -magnitude);
            if (ret < 0) {
                return;
            }

            *reinterpret_cast<short *>(reinterpret_cast<char *>(this) + channel * 2 + 0x16CC) = magnitude;
            return;
        }

        if (*reinterpret_cast<unsigned long *>(reinterpret_cast<char *>(this) + channel * 0x20 + 0x1230) == static_cast<unsigned long>(-1)) {
            ret = reinterpret_cast<Condition *>(reinterpret_cast<char *>(this) + 0x11A8)
                      ->DownloadForce(channel, 2, *reinterpret_cast<unsigned long *>(reinterpret_cast<char *>(this) + channel * 4 + 0x1050), 8, static_cast<unsigned long>(-1), 0, 0, 0, 0xFF, 0xFF, -magnitude, -magnitude);
        } else if (SameSlipperyRoadEffectParams(channel, magnitude)) {
            reinterpret_cast<Condition *>(reinterpret_cast<char *>(this) + 0x11A8)->Start(channel, 2);
            return;
        } else {
            ret = reinterpret_cast<Condition *>(reinterpret_cast<char *>(this) + 0x11A8)->UpdateForce(channel, 2, 8, static_cast<unsigned long>(-1), 0, 0, 0, 0xFF, 0xFF, -magnitude, -magnitude);
        }

        if (ret >= 0) {
            *reinterpret_cast<short *>(reinterpret_cast<char *>(this) + channel * 2 + 0x16CC) = magnitude;
        }

        reinterpret_cast<Condition *>(reinterpret_cast<char *>(this) + 0x11A8)->Start(channel, 2);
    } else {
        OSReport(kPlayForceError, channel);
    }
}

void LGWheels::StopSlipperyRoadEffect(long channel) {
    this->StopForce(channel, 7);
}

bool LGWheels::SameSlipperyRoadEffectParams(long channel, short magnitude) {
    const short *channelMagnitude = &LGWheelsGetSlipperyRoadParams(this)[channel].magnitude;
    return *channelMagnitude == magnitude;
}

void LGWheels::PlaySurfaceEffect(long channel, unsigned char type, unsigned char magnitude, unsigned short period) {
    int ret = 0;

    if (reinterpret_cast<Wheels *>(reinterpret_cast<char *>(this) + 0x828)->IsConnected(channel)) {
        if (*reinterpret_cast<int *>(reinterpret_cast<char *>(this) + channel * 4 + 0x166C) != 0) {
            return;
        }

        if (*reinterpret_cast<int *>(reinterpret_cast<char *>(this) + channel * 0x20 + 0x13B4) != 0) {
            if (SameSurfaceEffectParams(channel, type, magnitude, period)) {
                return;
            }

            if (type != reinterpret_cast<SurfaceEffectParams *>(reinterpret_cast<char *>(this) + 0x16D4)[channel].type) {
                reinterpret_cast<Periodic *>(reinterpret_cast<char *>(this) + 0x13A8)->Destroy(channel, 3);
                ret = reinterpret_cast<Periodic *>(reinterpret_cast<char *>(this) + 0x13A8)
                          ->DownloadForce(channel, 3, reinterpret_cast<unsigned long *>(reinterpret_cast<char *>(this) + 0x1050)[channel], type, static_cast<unsigned long>(-1), 0, magnitude, 90, period, 0, 0, 0, 0, 0, 0);
                reinterpret_cast<Periodic *>(reinterpret_cast<char *>(this) + 0x13A8)->Start(channel, 3);
            } else {
                ret = reinterpret_cast<Periodic *>(reinterpret_cast<char *>(this) + 0x13A8)
                          ->UpdateForce(channel, 3, type, static_cast<unsigned long>(-1), 0, magnitude, 90, period, 0, 0, 0, 0, 0, 0);
            }

            if (ret >= 0) {
                reinterpret_cast<SurfaceEffectParams *>(reinterpret_cast<char *>(this) + 0x16D4)[channel].magnitude = magnitude;
                reinterpret_cast<SurfaceEffectParams *>(reinterpret_cast<char *>(this) + 0x16D4)[channel].type = type;
                reinterpret_cast<SurfaceEffectParams *>(reinterpret_cast<char *>(this) + 0x16D4)[channel].period = period;
            }
            return;
        }

        if (*reinterpret_cast<unsigned long *>(reinterpret_cast<char *>(this) + channel * 0x20 + 0x1434) == static_cast<unsigned long>(-1)) {
            ret = reinterpret_cast<Periodic *>(reinterpret_cast<char *>(this) + 0x13A8)
                      ->DownloadForce(channel, 3, reinterpret_cast<unsigned long *>(reinterpret_cast<char *>(this) + 0x1050)[channel], type, static_cast<unsigned long>(-1), 0, magnitude, 90, period, 0, 0, 0, 0, 0, 0);
            if (ret >= 0) {
                reinterpret_cast<SurfaceEffectParams *>(reinterpret_cast<char *>(this) + 0x16D4)[channel].magnitude = magnitude;
                reinterpret_cast<SurfaceEffectParams *>(reinterpret_cast<char *>(this) + 0x16D4)[channel].type = type;
                reinterpret_cast<SurfaceEffectParams *>(reinterpret_cast<char *>(this) + 0x16D4)[channel].period = period;
            }
            reinterpret_cast<Periodic *>(reinterpret_cast<char *>(this) + 0x13A8)->Start(channel, 3);
            return;
        }

        if (SameSurfaceEffectParams(channel, type, magnitude, period)) {
            reinterpret_cast<Periodic *>(reinterpret_cast<char *>(this) + 0x13A8)->Start(channel, 3);
            return;
        }

        if (type != reinterpret_cast<SurfaceEffectParams *>(reinterpret_cast<char *>(this) + 0x16D4)[channel].type) {
            reinterpret_cast<Periodic *>(reinterpret_cast<char *>(this) + 0x13A8)->Destroy(channel, 3);
            ret = reinterpret_cast<Periodic *>(reinterpret_cast<char *>(this) + 0x13A8)
                      ->DownloadForce(channel, 3, reinterpret_cast<unsigned long *>(reinterpret_cast<char *>(this) + 0x1050)[channel], type, static_cast<unsigned long>(-1), 0, magnitude, 90, period, 0, 0, 0, 0, 0, 0);
        } else {
            ret = reinterpret_cast<Periodic *>(reinterpret_cast<char *>(this) + 0x13A8)
                      ->UpdateForce(channel, 3, type, static_cast<unsigned long>(-1), 0, magnitude, 90, period, 0, 0, 0, 0, 0, 0);
        }

        if (ret >= 0) {
            reinterpret_cast<SurfaceEffectParams *>(reinterpret_cast<char *>(this) + 0x16D4)[channel].magnitude = magnitude;
            reinterpret_cast<SurfaceEffectParams *>(reinterpret_cast<char *>(this) + 0x16D4)[channel].type = type;
            reinterpret_cast<SurfaceEffectParams *>(reinterpret_cast<char *>(this) + 0x16D4)[channel].period = period;
        }

        reinterpret_cast<Periodic *>(reinterpret_cast<char *>(this) + 0x13A8)->Start(channel, 3);
    } else {
        OSReport(kPlayForceError, channel);
    }
}

void LGWheels::StopSurfaceEffect(long channel) {
    this->StopForce(channel, 8);
}

bool LGWheels::SameSurfaceEffectParams(long channel, unsigned char type, unsigned char magnitude, unsigned short period) {
    const char *channelParams = reinterpret_cast<const char *>(this) + channel * 4;
    return *reinterpret_cast<const unsigned char *>(channelParams + 0x16D4) == type &&
           *reinterpret_cast<const unsigned char *>(channelParams + 0x16D5) == magnitude &&
           *reinterpret_cast<const unsigned short *>(channelParams + 0x16D6) == period;
}

void LGWheels::PlayCarAirborne(long channel) {
    if (LGWheelsGetWheels(this)->IsConnected(channel)) {
        LGWheelsGetIsAirborne(this, channel) = 1;
        if (IsPlaying(channel, 0)) {
            StopSpringForce(channel);
            LGWheelsGetWasPlayingBeforeAirborne(this, channel, 0) = 1;
        }
        if (IsPlaying(channel, 1)) {
            StopConstantForce(channel);
            LGWheelsGetWasPlayingBeforeAirborne(this, channel, 1) = 1;
        }
        if (IsPlaying(channel, 2)) {
            StopDamperForce(channel);
            LGWheelsGetWasPlayingBeforeAirborne(this, channel, 2) = 1;
        }
        if (IsPlaying(channel, 5)) {
            StopDirtRoadEffect(channel);
            LGWheelsGetWasPlayingBeforeAirborne(this, channel, 5) = 1;
        }
        if (IsPlaying(channel, 6)) {
            StopBumpyRoadEffect(channel);
            LGWheelsGetWasPlayingBeforeAirborne(this, channel, 6) = 1;
        }
        if (IsPlaying(channel, 7)) {
            StopSlipperyRoadEffect(channel);
            LGWheelsGetWasPlayingBeforeAirborne(this, channel, 7) = 1;
        }
        if (IsPlaying(channel, 8)) {
            StopSurfaceEffect(channel);
            LGWheelsGetWasPlayingBeforeAirborne(this, channel, 8) = 1;
        }
    } else {
        OSReport(kPlayForceError, channel);
    }
}

void LGWheels::StopCarAirborne(long channel) {
    this->StopForce(channel, 9);
}
