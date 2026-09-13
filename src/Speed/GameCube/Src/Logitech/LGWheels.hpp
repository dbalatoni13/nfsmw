// TODO is this file real?

#ifndef _LGWHEELS_H
#define _LGWHEELS_H

struct lgDevPosition { // 0x18
    /* 0x00 */ short unsigned int Status;
    /* 0x02 */ short int X1;
    /* 0x04 */ short int Y1;
    /* 0x06 */ short int X2;
    /* 0x08 */ short int Y2;
    /* 0x0a */ short unsigned int Throttle;
    /* 0x0c */ short int Twist;
    /* 0x0e */ short unsigned int Accelerator;
    /* 0x10 */ short unsigned int Brake;
    /* 0x12 */ unsigned char POV0;
    /* 0x13 */ unsigned char POV1;
    /* 0x14 */ unsigned int Buttons;
};

struct lgDevDeviceDesc { // 0x18
    /* 0x00 */ unsigned int DevClass;
    /* 0x04 */ unsigned int AdditionalControls;
    /* 0x08 */ unsigned int Flags;
    /* 0x0c */ unsigned char Topology[8];
    /* 0x14 */ int ExistingHandle;
};

struct lgDevEnvelopeParams { // 0x10
    /* 0x0 */ unsigned int AttackTime;
    /* 0x4 */ unsigned int AttackLevel;
    /* 0x8 */ unsigned int FadeTime;
    /* 0xc */ unsigned int FadeLevel;
};

struct lgDevConstantForceParams { // 0x18
    /* 0x00 */ int Magnitude;
    /* 0x04 */ unsigned int Direction;
    /* 0x08 */ lgDevEnvelopeParams envelope;
};

struct lgDevRampForceParams { // 0xc
    /* 0x0 */ int MagnitudeStart;
    /* 0x4 */ int MagnitudeEnd;
    /* 0x8 */ unsigned int Direction;
};

struct lgDevPeriodicForceParams { // 0x24
    /* 0x00 */ unsigned int Magnitude;
    /* 0x04 */ unsigned int Direction;
    /* 0x08 */ unsigned int Period;
    /* 0x0c */ unsigned int Phase;
    /* 0x10 */ int Offset;
    /* 0x14 */ lgDevEnvelopeParams envelope;
};

struct lgDevConditionForceParams { // 0x18
    /* 0x00 */ int Offset;
    /* 0x04 */ unsigned int Deadband;
    /* 0x08 */ unsigned int SaturationNeg;
    /* 0x0c */ unsigned int SaturationPos;
    /* 0x10 */ int CoefficientNeg;
    /* 0x14 */ int CoefficientPos;
};

struct lgDevForceEffect { // 0x3c
    /* 0x00 */ unsigned char Type;
    /* 0x04 */ unsigned int Duration;
    /* 0x08 */ unsigned int StartDelay;
    /* 0x0c */ union { // 0x30
        /* 0x0c */ lgDevConstantForceParams constant;
        /* 0x0c */ lgDevRampForceParams ramp;
        /* 0x0c */ lgDevPeriodicForceParams periodic;
        /* 0x0c */ lgDevConditionForceParams condition[2];
    } p;
};

typedef enum { LG_LOCAL_DPAD_UP = 0, LG_LOCAL_DPAD_RIGHT = 1, LG_LOCAL_DPAD_DOWN = 2, LG_LOCAL_DPAD_LEFT = 3 } DPadDirection;

typedef enum { LG_MENU_UP = 0, LG_MENU_DOWN = 1, LG_MENU_RIGHT = 2, LG_MENU_LEFT = 3 } MenuDirection;

struct LogiWheels { // 0x900
    /* 0x000 */ lgDevPosition Position[2];
    /* 0x030 */ int NonLinearWheel[2][256];
    /* 0x830 */ int WheelHandles[2];
    /* 0x838 */ bool validController[2];
    /* 0x840 */ int wheelClass[2];

  private:
    /* 0x880 */ lgDevPosition PositionLast[2];
    /* 0x8b0 */ bool multiturnCapable[2];
    /* 0x8b8 */ int numDevices;
    /* 0x8bc */ int enumhint;
    /* 0x8c0 */ bool rebootedIOP;

  public:
    LogiWheels();
    ~LogiWheels();
    bool EnumIsNeeded();
    void EnumerateDevices();
    void ReadPort();
    bool ButtonIsPressed();
    bool ButtonTriggered();
    bool ButtonReleased();
    bool WheelAMenuIsPressed();
    bool WheelAMenuTriggered();
    bool IsConnected();
    bool IsWheelClassA();
    bool IsWheelClassB();
    bool IsMultiturnCapable();
    bool IsMultiturnEnabled();
    void SetMultiturn();
    void DestroyDevices();
    void GenerateNonLinValues();
    void PrepareForIOPReboot();
    void InitAfterIOPReboot();

  private:
    void InitVars();
    void InitlgDevLibrary();
    unsigned int GetDeviceNumber();
    bool DPadIsPressed();
    bool DPadTriggered();
    bool DPadReleased();
    float CalculateNonLinValue();
};

struct LogiForce { // 0x80
    /* 0x00 */ bool Playing[2][8];
    /* 0x40 */ int EffectID[2][8];

    LogiForce();
    ~LogiForce();
    int Start();
    int Stop();
    int Destroy();
    void SetOverallForceGain();
    int GetOverallForceGain();

  private:
    void InitVars();
};

struct LogiCondition : /* 0x00 */ LogiForce { // 0x80

    LogiCondition();
    ~LogiCondition();
    int DownloadForce();
    int UpdateForce();
};

struct LogiConstant : /* 0x00 */ LogiForce { // 0x80

    LogiConstant();
    ~LogiConstant();
    void Initialize();
    int DownloadForce();
    int UpdateForce();
};

struct LogiPeriodic : /* 0x00 */ LogiForce { // 0x80

    LogiPeriodic();
    ~LogiPeriodic();
    int DownloadForce();
    int UpdateForce();
};

struct LogiRamp : /* 0x00 */ LogiForce { // 0x80

    LogiRamp();
    ~LogiRamp();
    int DownloadForce();
    int UpdateForce();
};

typedef enum {
    LG_SPRING = 0,
    LG_CONSTANT = 1,
    LG_DAMPER = 2,
    LG_SIDE_COLLISION = 3,
    LG_FRONTAL_COLLISION = 4,
    LG_DIRT_ROAD = 5,
    LG_BUMPY_ROAD = 6,
    LG_SLIPPERY_ROAD = 7,
    LG_SURFACE_EFFECT = 8,
    LG_CAR_AIRBORNE = 9,
    LG_NUMBER_FORCE_EFFECTS = 10,
    LG_SOFTSTOP = 11
} ForceType;

typedef enum {
    LG_CONDITION_0 = 0,
    LG_CONDITION_1 = 1,
    LG_CONDITION_2 = 2,
    LG_CONDITION_3 = 3,
    LG_CONDITION_4 = 4,
    LG_CONDITION_5 = 5,
    LG_CONDITION_6 = 6,
    LG_CONDITION_7 = 7
} ConditionForceNumber;

typedef enum {
    LG_PERIODIC_0 = 0,
    LG_PERIODIC_1 = 1,
    LG_PERIODIC_2 = 2,
    LG_PERIODIC_3 = 3,
    LG_PERIODIC_4 = 4,
    LG_PERIODIC_5 = 5,
    LG_PERIODIC_6 = 6,
    LG_PERIODIC_7 = 7
} PeriodicForceNumber;

typedef enum {
    LG_CONSTANT_0 = 0,
    LG_CONSTANT_1 = 1,
    LG_CONSTANT_2 = 2,
    LG_CONSTANT_3 = 3,
    LG_CONSTANT_4 = 4,
    LG_CONSTANT_5 = 5,
    LG_CONSTANT_6 = 6,
    LG_CONSTANT_7 = 7
} ConstantForceNumber;

typedef enum {
    LG_RAMP_0 = 0,
    LG_RAMP_1 = 1,
    LG_RAMP_2 = 2,
    LG_RAMP_3 = 3,
    LG_RAMP_4 = 4,
    LG_RAMP_5 = 5,
    LG_RAMP_6 = 6,
    LG_RAMP_7 = 7
} RampForceNumber;

// total size: 0xA
struct LGPosition {
    // Members
    unsigned short button;      // offset 0x0, size 0x2
    unsigned char misc;         // offset 0x2, size 0x1
    char wheel;                 // offset 0x3, size 0x1
    unsigned char accelerator;  // offset 0x4, size 0x1
    unsigned char brake;        // offset 0x5, size 0x1
    char combined;              // offset 0x6, size 0x1
    unsigned char triggerLeft;  // offset 0x7, size 0x1
    unsigned char triggerRight; // offset 0x8, size 0x1
    char err;                   // offset 0x9, size 0x1
};

// total size: 0x880
struct Wheels {
    // Functions
    Wheels();

    ~Wheels();

    void InitLGDevLibrary();

    short ReadAll();

    int FirstConnectedPort();

    bool ButtonIsPressed(int channel, unsigned long buttonMask);

    bool ButtonTriggered(int channel, unsigned long buttonMask);

    bool ButtonReleased(int channel, unsigned long buttonMask);

    bool IsConnected(int channel);

    bool PedalsConnected(int channel);

    bool PowerConnected(int channel);

    void GenerateNonLinValues(int channel, unsigned char nonLinCoeff);

    float CalculateNonLinValue(int inputValue, unsigned char nonLinearCoeff, short nonLinMinOutput, short nonLinMaxOutput);

    // Members
    LGPosition Position[4];        // offset 0x0, size 0x28
    short NonLinearWheel[256][4];  // offset 0x28, size 0x800
    unsigned long WheelHandles[4]; // offset 0x828, size 0x10
    unsigned long type[4];         // offset 0x838, size 0x10
    unsigned long WheelHandle[4];  // offset 0x848, size 0x10
    LGPosition PositionLast[4];    // offset 0x858, size 0x28
};

// total size: 0x100
struct Force {
    // Members
    bool Playing[8][4];           // offset 0x0, size 0x20
    unsigned long EffectID[8][4]; // offset 0x80, size 0x80
};

// total size: 0x100
struct Condition : public Force {
    // Functions
    Condition();

    ~Condition();

    int DownloadForce(int channel, int forceNumber, unsigned long &handle, unsigned char type, unsigned long duration, unsigned long startDelay,
                      char offset, unsigned char deadband, unsigned char satNeg, unsigned char satPos, short coeffNeg, short coeffPos);

    int UpdateForce(int channel, int forceNumber, unsigned char type, unsigned long duration, unsigned long startDelay, char offset,
                    unsigned char deadband, unsigned char satNeg, unsigned char satPos, short coeffNeg, short coeffPos);
};

// total size: 0x100
struct Constant : public Force {
    // Functions
    Constant();

    ~Constant();

    int DownloadForce(int channel, int forceNumber, unsigned long &handle, unsigned long duration, unsigned long startDelay, short magnitude,
                      unsigned short direction, unsigned long attackTime, unsigned long fadeTime, unsigned char attackLevel, unsigned char fadeLevel);

    int UpdateForce(int channel, int forceNumber, unsigned long duration, unsigned long startDelay, short magnitude, unsigned short direction,
                    unsigned long attackTime, unsigned long fadeTime, unsigned char attackLevel, unsigned char fadeLevel);
};

// total size: 0x100
struct Periodic : public Force {
    // Functions
    Periodic();

    ~Periodic();

    int DownloadForce(int channel, int forceNumber, unsigned long &handle, unsigned char type, unsigned long duration, unsigned long startDelay,
                      unsigned char magnitude, unsigned short direction, unsigned short period, unsigned short phase, short offset,
                      unsigned long attackTime, unsigned long fadeTime, unsigned char attackLevel, unsigned char fadeLevel);

    int UpdateForce(int channel, int forceNumber, unsigned char type, unsigned long duration, unsigned long startDelay, unsigned char magnitude,
                    unsigned short direction, unsigned short period, unsigned short phase, short offset, unsigned long attackTime,
                    unsigned long fadeTime, unsigned char attackLevel, unsigned char fadeLevel);
};

// total size: 0x100
struct Ramp : public Force {};

// total size: 0x16E4
struct LGWheels {
    // Functions
    LGWheels();

    ~LGWheels();

    void InitVars(int channel);

    void ReadAll();

    void StopForce(int channel, int forceType);

    bool IsConnected(int channel);

    bool IsPlaying(int channel, int forceType);

    bool ButtonTriggered(int channel, unsigned long buttonMask);

    bool ButtonReleased(int channel, unsigned long buttonMask);

    bool ButtonIsPressed(int channel, unsigned long buttonMask);

    bool PedalsConnected(int channel);

    bool PowerConnected(int channel);

    void SetOverallForceGain(int channel, int value);

    int GetOverallForceGain(int channel);

    void GenerateNonLinValues(int channel, unsigned char nonLinCoeff);

    void PlayAutoCalibAndSpringForce(int channel);

    void PlaySpringForce(int channel, char offset, unsigned char saturation, short coefficient);

    void StopSpringForce(int channel);

    bool SameSpringForceParams(int channel, char offset, unsigned char saturation, short coefficient);

    void PlayConstantForce(int channel, short magnitude, unsigned short direction);

    void StopConstantForce(int channel);

    bool SameConstantForceParams(int channel, short magnitude, unsigned short direction);

    void PlayDamperForce(int channel, short coefficient);

    void StopDamperForce(int channel);

    bool SameDamperForceParams(int channel, short coefficient);

    void PlaySideCollisionForce(int channel, short magnitude, unsigned short direction);

    bool SameSideCollisionForceParams(int channel, short magnitude, unsigned short direction);

    void PlayFrontalCollisionForce(int channel, unsigned char magnitude);

    bool SameFrontalCollisionForceParams(int channel, short magnitude);

    void PlayDirtRoadEffect(int channel, unsigned char magnitude);

    void StopDirtRoadEffect(int channel);

    bool SameDirtRoadEffectParams(int channel, short magnitude);

    void PlayBumpyRoadEffect(int channel, unsigned char magnitude);

    void StopBumpyRoadEffect(int channel);

    bool SameBumpyRoadEffectParams(int channel, short magnitude);

    void PlaySlipperyRoadEffect(int channel, short magnitude);

    void StopSlipperyRoadEffect(int channel);

    bool SameSlipperyRoadEffectParams(int channel, short magnitude);

    void PlaySurfaceEffect(int channel, unsigned char type, unsigned char magnitude, unsigned short period);

    void StopSurfaceEffect(int channel);

    bool SameSurfaceEffectParams(int channel, unsigned char type, unsigned char magnitude, unsigned short period);

    void PlayCarAirborne(int channel);

    void StopCarAirborne(int channel);

    // Members
    LGPosition Position[4];               // offset 0x0, size 0x28
    short NonLinearWheel[256][4];         // offset 0x28, size 0x800
    Wheels wheels;                        // offset 0x828, size 0x880
    Force force;                          // offset 0x10A8, size 0x100
    Condition condition;                  // offset 0x11A8, size 0x100
    Constant constant;                    // offset 0x12A8, size 0x100
    Periodic periodic;                    // offset 0x13A8, size 0x100
    Ramp ramp;                            // offset 0x14A8, size 0x100
    unsigned char OverallGain;            // offset 0x15A8, size 0x1
    bool damperWasPlaying[4];             // offset 0x15AC, size 0x4
    bool springWasPlaying[4];             // offset 0x15BC, size 0x4
    bool wasPlayingBeforeAirborne[10][4]; // offset 0x15CC, size 0x28
    bool IsAirborne[4];                   // offset 0x166C, size 0x4
    // total size: 0x4
    struct {
        // Members
        char offset;              // offset 0x0, size 0x1
        unsigned char saturation; // offset 0x1, size 0x1
        short coefficient;        // offset 0x2, size 0x2
    } SpringForceParams[4];       // offset 0x167C, size 0x10
    // total size: 0x4
    struct {
        // Members
        short magnitude;          // offset 0x0, size 0x2
        unsigned short direction; // offset 0x2, size 0x2
    } ConstantForceParams[4];     // offset 0x168C, size 0x10
    // total size: 0x2
    struct {
        // Members
        short coefficient;  // offset 0x0, size 0x2
    } DamperForceParams[4]; // offset 0x169C, size 0x8
    // total size: 0x4
    struct {
        // Members
        short magnitude;          // offset 0x0, size 0x2
        unsigned short direction; // offset 0x2, size 0x2
    } SideCollisionParams[4];     // offset 0x16A4, size 0x10
    // total size: 0x2
    struct {
        // Members
        short magnitude;         // offset 0x0, size 0x2
    } FrontalCollisionParams[4]; // offset 0x16B4, size 0x8
    // total size: 0x2
    struct {
        // Members
        short magnitude; // offset 0x0, size 0x2
    } DirtRoadParams[4]; // offset 0x16BC, size 0x8
    // total size: 0x2
    struct {
        // Members
        short magnitude;  // offset 0x0, size 0x2
    } BumpyRoadParams[4]; // offset 0x16C4, size 0x8
    // total size: 0x2
    struct {
        // Members
        short magnitude;     // offset 0x0, size 0x2
    } SlipperyRoadParams[4]; // offset 0x16CC, size 0x8
    // total size: 0x4
    struct {
        // Members
        unsigned char type;      // offset 0x0, size 0x1
        unsigned char magnitude; // offset 0x1, size 0x1
        unsigned short period;   // offset 0x2, size 0x2
    } SurfaceEffectParams[4];    // offset 0x16D4, size 0x10
};

#endif
