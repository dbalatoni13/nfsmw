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

struct Wheels {
    Wheels();
    ~Wheels();

    bool ButtonIsPressed(int channel, unsigned long buttonMask);
    bool IsConnected(int channel);
    bool PedalsConnected(int channel);
};

struct Force {
    bool Playing[8][4];
    unsigned long EffectID[8][4];
};

struct Condition : public Force {};
struct Constant : public Force {};
struct Periodic : public Force {};
struct Ramp : public Force {};

struct LGWheels {
    LGWheels();
    ~LGWheels();

    void StopForce(int channel, int forceType);
    bool IsConnected(int channel);
    bool ButtonIsPressed(int channel, unsigned long buttonMask);
    bool PedalsConnected(int channel);
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
