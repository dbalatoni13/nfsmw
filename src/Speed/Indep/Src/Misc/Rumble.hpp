#ifndef MISC_RUMBLE_H
#define MISC_RUMBLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Replay.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

// total size: 0x8
struct sEnvelopePoint {
    float fTime;  // offset 0x0, size 0x4
    float fValue; // offset 0x4, size 0x4
};

// total size: 0x8
class tEnvelope {
  public:
    int GetIndex(float t);
    bool OutOfRange(float t);
    float GetSlope(int nIndex);
    float GetSlope(float t);
    float GetValue(float t);
    float GetIntegral(float t0, float t1);

    tEnvelope() {}

    tEnvelope(sEnvelopePoint *p, int n) {
        p = pPoints;
        nPoints = n;
    }

    ~tEnvelope() {}

    // sEnvelopePoint *GetPoint(int i) {}

  private:
    sEnvelopePoint *pPoints; // offset 0x0, size 0x4
    int nPoints;             // offset 0x4, size 0x4
};

// total size: 0x20
class tShaker {
  public:
    void DoSnapshot(ReplaySnapshot *pSnapshot);
    float GetAmplitude();
    void StartShaking(bVector3 *pV, float fDur, float fFreq);
    void Update(float fDeltaTime);
    void GetValue(bVector3 *pV);

    tShaker() {}

    ~tShaker() {}

    inline void Reset();

    // bool IsShaking() {}

  private:
    bVector3 vShake;       // offset 0x0, size 0x10
    tEnvelope *pAmplitude; // offset 0x10, size 0x4
    float fTime;           // offset 0x14, size 0x4
    float fDuration;       // offset 0x18, size 0x4
    float fFrequency;      // offset 0x1C, size 0x4
};

void ResetCameraShakers();

#endif
