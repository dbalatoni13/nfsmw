#ifndef GAMECUBE_LOGITECH_LGWHEELS_H
#define GAMECUBE_LOGITECH_LGWHEELS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

struct LGPosition {
    unsigned short button;
    unsigned char misc;
    char wheel;
    unsigned char accelerator;
    unsigned char brake;
    char combined;
    unsigned char triggerLeft;
    unsigned char triggerRight;
    char err;
};

struct LGEnvelopeParams {
    unsigned long attackTime;
    unsigned long fadeTime;
    unsigned char attackLevel;
    unsigned char fadeLevel;
    unsigned char pad[2];
};

struct LGConstantForceParams {
    short magnitude;
    unsigned short direction;
    LGEnvelopeParams envelope;
};

struct LGRampForceParams {
    short magnitudeStart;
    short magnitudeEnd;
    unsigned short direction;
};

struct LGPeriodicForceParams {
    unsigned char magnitude;
    unsigned char pad;
    unsigned short direction;
    unsigned short period;
    unsigned short phase;
    short offset;
    unsigned char pad2[2];
    LGEnvelopeParams envelope;
};

struct LGConditionForceParams {
    char offset;
    unsigned char deadband;
    unsigned char saturationNeg;
    unsigned char saturationPos;
    short coefficientNeg;
    short coefficientPos;
};

struct LGForceEffect {
    unsigned char type;
    unsigned char pad[3];
    unsigned long duration;
    unsigned long startDelay;
    union {
        LGConstantForceParams constant;
        LGRampForceParams ramp;
        LGPeriodicForceParams periodic;
        LGConditionForceParams condition[2];
    } p;
};

struct Wheels {
    Wheels();
    ~Wheels();

    short ReadAll();
    bool ButtonIsPressed(long channel, unsigned long buttonMask);
    bool IsConnected(long channel);
    bool PedalsConnected(long channel);
};

struct Force {
    Force();
    void InitVars();
    int Start(long channel, long forceNumber);
    int Stop(long channel, long forceNumber);
    int Destroy(long channel, long forceNumber);

    bool Playing[8][4];
    unsigned long EffectID[8][4];
};

struct Condition : public Force {
    Condition();
    int DownloadForce(long channel, long forceNumber, unsigned long & handle, unsigned char type, unsigned long duration, unsigned long startDelay, signed char offset, unsigned char deadband, unsigned char satNeg, unsigned char satPos, short coeffNeg, short coeffPos);
    int UpdateForce(long channel, long forceNumber, unsigned char type, unsigned long duration, unsigned long startDelay, signed char offset, unsigned char deadband, unsigned char satNeg, unsigned char satPos, short coeffNeg, short coeffPos);
};

struct Constant : public Force {
    Constant();
    int DownloadForce(long channel, long forceNumber, unsigned long & handle, unsigned long duration, unsigned long startDelay, short magnitude, unsigned short direction, unsigned long attackTime, unsigned long fadeTime, unsigned char attackLevel, unsigned char fadeLevel);
    int UpdateForce(long channel, long forceNumber, unsigned long duration, unsigned long startDelay, short magnitude, unsigned short direction, unsigned long attackTime, unsigned long fadeTime, unsigned char attackLevel, unsigned char fadeLevel);
};

struct Periodic : public Force {
    Periodic();
    int DownloadForce(long channel, long forceNumber, unsigned long & handle, unsigned char type, unsigned long duration, unsigned long startDelay, unsigned char magnitude, unsigned short direction, unsigned short period, unsigned short phase, short offset, unsigned long attackTime, unsigned long fadeTime, unsigned char attackLevel, unsigned char fadeLevel);
    int UpdateForce(long channel, long forceNumber, unsigned char type, unsigned long duration, unsigned long startDelay, unsigned char magnitude, unsigned short direction, unsigned short period, unsigned short phase, short offset, unsigned long attackTime, unsigned long fadeTime, unsigned char attackLevel, unsigned char fadeLevel);
};

struct Ramp : public Force {
    Ramp();
};

struct LGWheels {
    LGWheels();
    ~LGWheels();

    void InitVars(long channel);
    void ReadAll();
    void StopForce(long channel, long forceType);
    bool IsConnected(long channel);
    bool IsPlaying(long channel, long forceType);
    bool ButtonIsPressed(long channel, unsigned long buttonMask);
    bool PedalsConnected(long channel);
    void PlayAutoCalibAndSpringForce(long channel);
    void PlaySpringForce(long channel, signed char offset, unsigned char saturation, short coefficient);
    void StopSpringForce(long channel);
    bool SameSpringForceParams(long channel, signed char offset, unsigned char saturation, short coefficient);
    void PlayConstantForce(long channel, short magnitude, unsigned short direction);
    void StopConstantForce(long channel);
    bool SameConstantForceParams(long channel, short magnitude, unsigned short direction);
    void PlayDamperForce(long channel, short coefficient);
    void StopDamperForce(long channel);
    bool SameDamperForceParams(long channel, short coefficient);
    void PlayFrontalCollisionForce(long channel, unsigned char magnitude);
    bool SameFrontalCollisionForceParams(long channel, short magnitude);
    void PlayDirtRoadEffect(long channel, unsigned char magnitude);
    void StopDirtRoadEffect(long channel);
    bool SameDirtRoadEffectParams(long channel, short magnitude);
    void PlayBumpyRoadEffect(long channel, unsigned char magnitude);
    void StopBumpyRoadEffect(long channel);
    bool SameBumpyRoadEffectParams(long channel, short magnitude);
    void PlaySlipperyRoadEffect(long channel, short magnitude);
    void StopSlipperyRoadEffect(long channel);
    bool SameSlipperyRoadEffectParams(long channel, short magnitude);
    void PlaySurfaceEffect(long channel, unsigned char type, unsigned char magnitude, unsigned short period);
    void StopSurfaceEffect(long channel);
    bool SameSurfaceEffectParams(long channel, unsigned char type, unsigned char magnitude, unsigned short period);
    void PlayCarAirborne(long channel);
    void StopCarAirborne(long channel);
};

extern LGWheels *plat_lgwheels;

#endif
