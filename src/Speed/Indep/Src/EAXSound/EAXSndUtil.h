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

struct cInterpLine {
    float ElapsedTime;  // offset 0x0
    float Length;        // offset 0x4
    float Start;         // offset 0x8
    float Finish;        // offset 0xC
    eCURVETYPE CurveTypes; // offset 0x10
    float CurValue;      // offset 0x14
    bool bComplete;      // offset 0x18

    cInterpLine();
    ~cInterpLine();
    void Initialize(float len, float start, int finish, eCURVETYPE curve);
    void Update(float dt, float target);
    void Update(float dt);
    float GetValue() { return CurValue; }
};

struct cPathLine {
    float ElapsedTime;       // offset 0x0
    float Length[6];         // offset 0x4
    float Start[6];          // offset 0x1C
    float Finish[6];         // offset 0x34
    int IsLinked[6];         // offset 0x4C
    eCURVETYPE CurveTypes[6]; // offset 0x64
    int num_stages;          // offset 0x7C
    int cur_stage;           // offset 0x80
    float CurValue;          // offset 0x84
    bool bComplete;          // offset 0x88

    cPathLine();
    ~cPathLine();
    void Initialize(float len, float start, int finish);
    void ClearStages();
    int AddStage(float len, float finish, int numstages, eCURVETYPE curve);
    int AddLinkedStage(float len, int finish, eCURVETYPE curve);
    void Update(float dt);
};

struct Slope {
    float Min;            // offset 0x0
    float Max;            // offset 0x4
    float Start;          // offset 0x8
    float Finish;         // offset 0xC
    float LastInput;      // offset 0x10
    float LastOutput;     // offset 0x14
    bool bNeedsRegenerate; // offset 0x18

    Slope(float _Min, float _Max, float _Start, float _Finish);
    ~Slope();
    void Initialize(float _Min, float _Max, float _Start, float _Finish);
    float GetValue(float input);
    void Regenerate();
};

struct EAX_CarState;
struct bVector3;

EAX_CarState *GetClosestPlayerCar(const bVector3 *vPosition);

#endif
