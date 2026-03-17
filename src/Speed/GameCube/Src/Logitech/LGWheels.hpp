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
    bool ButtonIsPressed(int channel, unsigned long buttonMask);
    bool IsConnected(int channel);
    bool PedalsConnected(int channel);
};

struct Force {
    Force();
    void InitVars();
    int Start(int channel, int forceNumber);
    int Stop(int channel, int forceNumber);
    int Destroy(int channel, int forceNumber);

    bool Playing[8][4];
    unsigned long EffectID[8][4];
};

struct Condition : public Force {
    Condition();
    int DownloadForce(int channel, int forceNumber, unsigned long & handle, unsigned char type, unsigned long duration, unsigned long startDelay, char offset, unsigned char deadband, unsigned char satNeg, unsigned char satPos, short coeffNeg, short coeffPos);
    int UpdateForce(int channel, int forceNumber, unsigned char type, unsigned long duration, unsigned long startDelay, char offset, unsigned char deadband, unsigned char satNeg, unsigned char satPos, short coeffNeg, short coeffPos);
};

struct Constant : public Force {
    Constant();
    int DownloadForce(int channel, int forceNumber, unsigned long & handle, unsigned long duration, unsigned long startDelay, short magnitude, unsigned short direction, unsigned long attackTime, unsigned long fadeTime, unsigned char attackLevel, unsigned char fadeLevel);
    int UpdateForce(int channel, int forceNumber, unsigned long duration, unsigned long startDelay, short magnitude, unsigned short direction, unsigned long attackTime, unsigned long fadeTime, unsigned char attackLevel, unsigned char fadeLevel);
};

struct Periodic : public Force {
    Periodic();
    int DownloadForce(int channel, int forceNumber, unsigned long & handle, unsigned char type, unsigned long duration, unsigned long startDelay, unsigned char magnitude, unsigned short direction, unsigned short period, unsigned short phase, short offset, unsigned long attackTime, unsigned long fadeTime, unsigned char attackLevel, unsigned char fadeLevel);
    int UpdateForce(int channel, int forceNumber, unsigned char type, unsigned long duration, unsigned long startDelay, unsigned char magnitude, unsigned short direction, unsigned short period, unsigned short phase, short offset, unsigned long attackTime, unsigned long fadeTime, unsigned char attackLevel, unsigned char fadeLevel);
};

struct Ramp : public Force {
    Ramp();
};

struct LGWheels {
    LGWheels();
    ~LGWheels();

    void InitVars(int channel);
    void ReadAll();
    void StopForce(int channel, int forceType);
    bool IsConnected(int channel);
    bool ButtonIsPressed(int channel, unsigned long buttonMask);
    bool PedalsConnected(int channel);
    void PlayAutoCalibAndSpringForce(int channel);
    void StopSpringForce(int channel);
    bool SameSpringForceParams(int channel, char offset, unsigned char saturation, short coefficient);
    void StopConstantForce(int channel);
    bool SameConstantForceParams(int channel, short magnitude, unsigned short direction);
    void StopDamperForce(int channel);
    bool SameDamperForceParams(int channel, short coefficient);
    void StopDirtRoadEffect(int channel);
    bool SameDirtRoadEffectParams(int channel, short magnitude);
    void StopBumpyRoadEffect(int channel);
    bool SameBumpyRoadEffectParams(int channel, short magnitude);
    void StopSlipperyRoadEffect(int channel);
    bool SameSlipperyRoadEffectParams(int channel, short magnitude);
    void StopSurfaceEffect(int channel);
    bool SameSurfaceEffectParams(int channel, unsigned char type, unsigned char magnitude, unsigned short period);
    void StopCarAirborne(int channel);
};

extern LGWheels *plat_lgwheels;

#endif
