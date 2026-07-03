#ifndef EAXSOUND_EAXSNDUTIL_H
#define EAXSOUND_EAXSNDUTIL_H

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

    inline float GetValue() {
        return CurValue;
    }

    inline int iGetValue() {}

    void Update(float delta_time, float _new_Finish);

    void Update(float delta_time);

    inline bool IsFinished() {
        return bComplete;
    }

    inline int GetStageNumber() {
        return cur_stage;
    }

  private:
    float ElapsedTime;        // offset 0x0, size 0x4
    float Length[6];          // offset 0x4, size 0x18
    float Start[6];           // offset 0x1C, size 0x18
    float Finish[6];          // offset 0x34, size 0x18
    bool IsLinked[6];         // offset 0x4C, size 0x6
    eCURVETYPE CurveTypes[6]; // offset 0x64, size 0x18
    int num_stages;           // offset 0x7C, size 0x4
    int cur_stage;            // offset 0x80, size 0x4
    float CurValue;           // offset 0x84, size 0x4
    bool bComplete;           // offset 0x88, size 0x1
};

// total size: 0x1C
class cInterpLine {
  public:
    cInterpLine();

    ~cInterpLine();

    void Initialize(float _Start, float _Finish, int _Length, eCURVETYPE _Curve);

    void Reset();

    inline float GetValue() {
        return CurValue;
    }

    inline int iGetValue() {}

    void Update(float delta_time, float _new_Finish);

    void Update(float delta_time);

    bool IsFinished() {
        return bComplete;
    }

  private:
    float ElapsedTime;     // offset 0x0, size 0x4
    float Length;          // offset 0x4, size 0x4
    float Start;           // offset 0x8, size 0x4
    float Finish;          // offset 0xC, size 0x4
    eCURVETYPE CurveTypes; // offset 0x10, size 0x4
    float CurValue;        // offset 0x14, size 0x4
    bool bComplete;        // offset 0x18, size 0x1
};

#endif
