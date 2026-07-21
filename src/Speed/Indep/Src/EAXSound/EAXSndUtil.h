//
//
#ifndef EAX_SND_UTIL_HPP
#define EAX_SND_UTIL_HPP

#define MAX_NUM_STAGES 6 // Decl: 6

#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_EnumAttributes.hpp"

// total size: 0x8C

class cPathLine {
  public:
    cPathLine();
    ~cPathLine();

    void Initialize(float _Start, float _Finish, int _Length);

    void ClearStages();
    int AddStage(float _Start, float _Finish, int _Length, eCURVETYPE _Curve);
    int AddLinkedStage(float _Finish, int _Length, eCURVETYPE _Curve);

    void Reset();

    float GetValue() {}
    int iGetValue() {}
    void Update(float delta_time);
    void Update(float delta_time, float _new_Finish);

    bool IsFinished() {}
    int GetStageNumber() {}

  private:
    float ElapsedTime; // offset 0x0, size 0x4

    float Length[6];          // offset 0x4, size 0x18
    float Start[6];           // offset 0x1C, size 0x18
    float Finish[6];          // offset 0x34, size 0x18
    bool IsLinked[6];         // offset 0x4C, size 0x6
    eCURVETYPE CurveTypes[6]; // offset 0x64, size 0x18

    int num_stages; // offset 0x7C, size 0x4
    int cur_stage;  // offset 0x80, size 0x4

    float CurValue; // offset 0x84, size 0x4

    bool bComplete; // offset 0x88, size 0x1
};

// total size: 0x1C

class cInterpLine {
  public:
    cInterpLine();
    ~cInterpLine();

    void Initialize(float _Start, float _Finish, int _Length, eCURVETYPE _Curve);

    void Reset();

    float GetValue() {
        return this->CurValue;
    }
    int iGetValue() {}
    void Update(float delta_time);
    void Update(float delta_time, float _new_Finish);

    bool IsFinished() {}

  private:
    float ElapsedTime;     // offset 0x0, size 0x4
    float Length;          // offset 0x4, size 0x4
    float Start;           // offset 0x8, size 0x4
    float Finish;          // offset 0xC, size 0x4
    eCURVETYPE CurveTypes; // offset 0x10, size 0x4

    float CurValue; // offset 0x14, size 0x4

    bool bComplete; // offset 0x18, size 0x1
};

// total size: 0x1C

struct RotatingCircle {
    RotatingCircle() {}

    float Theta;       // offset 0x0, size 0x4
    float Vol;         // offset 0x4, size 0x4
    float Az;          // offset 0x8, size 0x4
    bVector2 Position; // offset 0xC, size 0x8
    bVector2 StartPos; // offset 0x14, size 0x8
};

// total size: 0x68

class Oscillator {
  public:
    Oscillator();
    Oscillator(float _seperation, float _RollOff);
    ~Oscillator();

    void Reset();

    void Update(float t);

    float RotSpeed; // offset 0x0, size 0x4
    int Seperation; // offset 0x4, size 0x4
    float Rolloff;  // offset 0x8, size 0x4
    float Radius;   // offset 0xC, size 0x4
    float Distance; // offset 0x10, size 0x4

    RotatingCircle Circle[3]; // offset 0x14, size 0x54
};

enum eFADE_TYPE {
    FADE_NONE = 0,
    FADE_UP = 1,
    FADE_DOWN = 2,
};

// total size: 0x18

struct PanSoundObj {
    PanSoundObj() {}

    int Vol;              // offset 0x0, size 0x4
    int Az;               // offset 0x4, size 0x4
    int Pitch;            // offset 0x8, size 0x4
    eFADE_TYPE FADE_TYPE; // offset 0xC, size 0x4
    int iFadeVol;         // offset 0x10, size 0x4

    bool IsPanRight; // offset 0x14, size 0x1
};

// total size: 0x24

struct sCreatePanEffect {
    sCreatePanEffect() {}

    int MaxSeperation; // offset 0x0, size 0x4
    int MinSeperation; // offset 0x4, size 0x4

    int MaxPitch; // offset 0x8, size 0x4
    int MinPitch; // offset 0xC, size 0x4

    float Vel_VolBegin; // offset 0x10, size 0x4
    float Vel_VolEnd;   // offset 0x14, size 0x4

    float Vel_SepBegin; // offset 0x18, size 0x4
    float Vel_SepEnd;   // offset 0x1C, size 0x4

    float tOscillation; // offset 0x20, size 0x4
};

// total size: 0x78

class PanEffect {
  public:
    PanEffect();
    ~PanEffect();

    void Initialize(sCreatePanEffect _CreatePanStruct);
    void ReInit(sCreatePanEffect _CreatePanStruct);
    void Update(float t, float Velocity);
    int GetAz(int item) {}
    int GetVol(int item) {}
    int GetPitch(int item) {}
    void FlipSamples();

    int MaxSeperation; // offset 0x0, size 0x4
    int MinSeperation; // offset 0x4, size 0x4

    int MaxPitch; // offset 0x8, size 0x4
    int MinPitch; // offset 0xC, size 0x4

    float Vel_VolBegin; // offset 0x10, size 0x4
    float Vel_VolEnd;   // offset 0x14, size 0x4

    float Vel_SepBegin; // offset 0x18, size 0x4
    float Vel_SepEnd;   // offset 0x1C, size 0x4

    bool IsInitialized;    // offset 0x20, size 0x1
    float tOscillation;    // offset 0x24, size 0x4
    float tCurOscillation; // offset 0x28, size 0x4

    int Seperation; // offset 0x2C, size 0x4

    PanSoundObj SndObject[3]; // offset 0x30, size 0x48
};

// total size: 0x1C

class Slope {
  public:
    Slope();

    Slope(float _Min, float _Max, float _Start, float _Finish);

    ~Slope();

    float GetValue();

    float GetValue(float input);

    float GetInputScale(float input);

    void Update(float input);

    void Regenerate();

    void Initialize(float _Min, float _Max, float _Start, float _Finish);

    float Min;             // offset 0x0, size 0x4
    float Max;             // offset 0x4, size 0x4
    float Start;           // offset 0x8, size 0x4
    float Finish;          // offset 0xC, size 0x4
    float LastInput;       // offset 0x10, size 0x4
    float LastOutput;      // offset 0x14, size 0x4
    bool bNeedsRegenerate; // offset 0x18, size 0x1
};

#define BVEC_TO_UMATH(bvector3, vector4)                                                                                                             \
    vector4.z = bvector3->x;                                                                                                                         \
    vector4.x = -bvector3->y;                                                                                                                        \
    vector4.y = bvector3->z;

#define UMATH_TO_BVEC(bvector3, vector4)                                                                                                             \
    bvector3->x = vector4.z;                                                                                                                         \
    bvector3->y = -vector4.x;                                                                                                                        \
    bvector3->z = vector4.y;

#endif
