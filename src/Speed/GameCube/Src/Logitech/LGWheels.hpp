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
    // total size: 0x880
    Wheels();
    ~Wheels();

    void InitLGDevLibrary();
    short ReadAll();
    int FirstConnectedPort();
    bool ButtonIsPressed(long channel, unsigned long buttonMask);
    bool ButtonTriggered(long channel, unsigned long buttonMask);
    bool ButtonReleased(long channel, unsigned long buttonMask);
    bool IsConnected(long channel);
    bool PedalsConnected(long channel);
    bool PowerConnected(long channel);
    void GenerateNonLinValues(long channel, unsigned char nonLinCoeff);
    float CalculateNonLinValue(int inputValue, unsigned char nonLinearCoeff, short nonLinMinOutput, short nonLinMaxOutput);

    LGPosition Position[4];            // offset 0x0, size 0x28
    short NonLinearWheel[256][4];      // offset 0x28, size 0x800
    unsigned long WheelHandles[4];     // offset 0x828, size 0x10
    unsigned long type[4];             // offset 0x838, size 0x10
    unsigned long WheelHandle[4];      // offset 0x848, size 0x10
    LGPosition PositionLast[4];        // offset 0x858, size 0x28
};

struct Force {
    // total size: 0x100
    Force();
    void InitVars();
    int Start(long channel, long forceNumber);
    int Stop(long channel, long forceNumber);
    int Destroy(long channel, long forceNumber);
    void SetOverallForceGain(unsigned long &handle, int value);
    int GetOverallForceGain(unsigned long &handle);

    bool Playing[8][4];                // offset 0x0, size 0x20
    unsigned long EffectID[8][4];      // offset 0x20, size 0x80
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
    // total size: 0x100
    Ramp();
};

class LGWheels {
    // total size: 0x16E4
  public:
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

  private:
    LGPosition Position[4];                    // offset 0x0, size 0x28
    short NonLinearWheel[256][4];             // offset 0x28, size 0x800
    unsigned char wheels[0x880];              // offset 0x828, size 0x880
    unsigned char force[0x100];               // offset 0x10A8, size 0x100
    unsigned char condition[0x100];           // offset 0x11A8, size 0x100
    unsigned char constant[0x100];            // offset 0x12A8, size 0x100
    unsigned char periodic[0x100];            // offset 0x13A8, size 0x100
    unsigned char ramp[0x100];                // offset 0x14A8, size 0x100
    unsigned char OverallGain;                // offset 0x15A8, size 0x1
    bool damperWasPlaying[4];                 // offset 0x15AC, size 0x4
    bool springWasPlaying[4];                 // offset 0x15BC, size 0x4
    bool wasPlayingBeforeAirborne[10][4];     // offset 0x15CC, size 0x28
    bool IsAirborne[4];                       // offset 0x166C, size 0x4
    struct {
        char offset;                          // offset 0x0, size 0x1
        unsigned char saturation;             // offset 0x1, size 0x1
        short coefficient;                    // offset 0x2, size 0x2
    } SpringForceParams[4];                   // offset 0x167C, size 0x10
    struct {
        short magnitude;                      // offset 0x0, size 0x2
        unsigned short direction;             // offset 0x2, size 0x2
    } ConstantForceParams[4];                 // offset 0x168C, size 0x10
    struct {
        short coefficient;                    // offset 0x0, size 0x2
    } DamperForceParams[4];                   // offset 0x169C, size 0x8
    struct {
        short magnitude;                      // offset 0x0, size 0x2
        unsigned short direction;             // offset 0x2, size 0x2
    } SideCollisionParams[4];                 // offset 0x16A4, size 0x10
    struct {
        short magnitude;                      // offset 0x0, size 0x2
    } FrontalCollisionParams[4];              // offset 0x16B4, size 0x8
    struct {
        short magnitude;                      // offset 0x0, size 0x2
    } DirtRoadParams[4];                      // offset 0x16BC, size 0x8
    struct {
        short magnitude;                      // offset 0x0, size 0x2
    } BumpyRoadParams[4];                     // offset 0x16C4, size 0x8
    struct {
        short magnitude;                      // offset 0x0, size 0x2
    } SlipperyRoadParams[4];                  // offset 0x16CC, size 0x8
    struct {
        unsigned char type;                   // offset 0x0, size 0x1
        unsigned char magnitude;              // offset 0x1, size 0x1
        unsigned short period;                // offset 0x2, size 0x2
    } SurfaceEffectParams[4];                 // offset 0x16D4, size 0x10
};

extern LGWheels *plat_lgwheels;

#endif
