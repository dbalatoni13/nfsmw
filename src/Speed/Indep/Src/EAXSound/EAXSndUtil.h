#ifndef EAXSOUND_EAXSNDUTIL_H
#define EAXSOUND_EAXSNDUTIL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

enum eCURVETYPE {
    LINEAR = 0,
    PARABOLIC = 1,
    INV_PARABOLIC = 2,
    SINUSOID = 3,
    POWER = 4,
    EQ_PWR_SQ = 5
};

enum eMIXTABLEID {
    SHAPE_DWN_EQPWR = 0,
    SHAPE_UP_EQPWR = 1,
    SHAPE_DWN_EQPWR_SQ = 2,
    SHAPE_UP_EQPWR_SQ = 3,
    SHAPE_DWN_ONE_MIN_EQPWR = 4,
    SHAPE_UP_ONE_MIN_EQPWR = 5,
    SHAPE_DWN_ONE_MIN_EQPWR_SQ = 6,
    SHAPE_UP_ONE_MIN_EQPWR_SQ = 7,
    SHAPE_DWN_LINEAR = 8,
    SHAPE_UP_LINEAR = 9,
    MAX_SHAPE_TYPES = 10,
};

class NFSMixShape {
  public:
    typedef ::eMIXTABLEID eMIXTABLEID;
    enum {
        SHAPE_DWN_EQPWR = ::SHAPE_DWN_EQPWR,
        SHAPE_UP_EQPWR = ::SHAPE_UP_EQPWR,
        SHAPE_DWN_EQPWR_SQ = ::SHAPE_DWN_EQPWR_SQ,
        SHAPE_UP_EQPWR_SQ = ::SHAPE_UP_EQPWR_SQ,
        SHAPE_DWN_ONE_MIN_EQPWR = ::SHAPE_DWN_ONE_MIN_EQPWR,
        SHAPE_UP_ONE_MIN_EQPWR = ::SHAPE_UP_ONE_MIN_EQPWR,
        SHAPE_DWN_ONE_MIN_EQPWR_SQ = ::SHAPE_DWN_ONE_MIN_EQPWR_SQ,
        SHAPE_UP_ONE_MIN_EQPWR_SQ = ::SHAPE_UP_ONE_MIN_EQPWR_SQ,
        SHAPE_DWN_LINEAR = ::SHAPE_DWN_LINEAR,
        SHAPE_UP_LINEAR = ::SHAPE_UP_LINEAR,
        MAX_SHAPE_TYPES = ::MAX_SHAPE_TYPES,
    };

    static int GetCurveOutput(::eMIXTABLEID id, int nQ15Ratio, bool invert);
    static int GetAzimShapeOutput(::eMIXTABLEID etable1, ::eMIXTABLEID etable2, int *pdistances, int nmixratio);
    static int GetQ15FromHundredthsdB(int ndB);
    static int GetdBFromQ15(int nQ15);
    static float GetFloatFromHundredthsdB(int ndB);
    static int GetCentsFromPitchMult(float ratio);
    static float GetPitchMultFromCents(int cents);
    static int GetIntPitchMultFromCents(int cents);
};

struct cInterpLine {
    float ElapsedTime;  // offset 0x0, size 0x4
    float Length;        // offset 0x4, size 0x4
    float Start;         // offset 0x8, size 0x4
    float Finish;        // offset 0xC, size 0x4
    eCURVETYPE CurveTypes; // offset 0x10, size 0x4
    float CurValue;      // offset 0x14, size 0x4
    bool bComplete;      // offset 0x18, size 0x1

    cInterpLine();
    ~cInterpLine();
    void Initialize(float _Start, float _Finish, int _Length, eCURVETYPE _Curve);
    void Update(float dt, float target);
    void Update(float dt);
    float GetValue() { return CurValue; }
    bool IsFinished() { return bComplete; }
};

struct cPathLine {
    float ElapsedTime;       // offset 0x0, size 0x4
    float Length[6];         // offset 0x4, size 0x18
    float Start[6];          // offset 0x1C, size 0x18
    float Finish[6];         // offset 0x34, size 0x18
    bool IsLinked[6];        // offset 0x4C, size 0x6
    eCURVETYPE CurveTypes[6]; // offset 0x64, size 0x18
    int num_stages;          // offset 0x7C, size 0x4
    int cur_stage;           // offset 0x80, size 0x4
    float CurValue;          // offset 0x84, size 0x4
    bool bComplete;          // offset 0x88, size 0x1

    cPathLine();
    ~cPathLine();
    void Initialize(float _Start, float _Finish, int _Length);
    void ClearStages();
    int AddStage(float len, float finish, int numstages, eCURVETYPE curve);
    int AddLinkedStage(float _Finish, int _Length, eCURVETYPE _Curve);
    void Update(float dt);
};

struct Slope {
    float Min;            // offset 0x0, size 0x4
    float Max;            // offset 0x4, size 0x4
    float Start;          // offset 0x8, size 0x4
    float Finish;         // offset 0xC, size 0x4
    float LastInput;      // offset 0x10, size 0x4
    float LastOutput;     // offset 0x14, size 0x4
    bool bNeedsRegenerate; // offset 0x18, size 0x1

    Slope(float _Min, float _Max, float _Start, float _Finish);
    ~Slope();
    void Initialize(float _Min, float _Max, float _Start, float _Finish);
    float GetValue(float input);
    void Regenerate();
};

struct EAX_CarState;
struct bVector3;

EAX_CarState *GetClosestPlayerCar(const bVector3 *vPosition);
EAX_CarState *GetClosestPlayerCar(const bVector3 *vPosition, bool CameraRelative, int &CarID);

#endif
