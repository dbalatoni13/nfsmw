#include "Rumble.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/Misc/Replay.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bTypes.hpp"

static const int bShakeEnable = 1;                          // Decl: 20
int bShakeTest = 0;                                         // Decl: 21
static const int bShakeDebug = 0;                           // Decl: 22
static const float fShakeDuration = 0.6f;                   // Decl: 23
static const float fShakeFrequency = 11.74265f;             // Decl: 24
bVector3 vShakeTest(0.0f, 0.0f, 0.3f);                      // Decl: 25
static const bVector3 vShakeRotation(-1200.0f, 2000.0f, 0.0f); // Decl: 26
static const bVector3 vShakeAccelBias(0.7f, 0.7f, 1.0f);    // Decl: 27

static const float fShakeIntensityCoeffs[6] = {100.0f, 600.0f, 0.1f, 0.4f, 0.3f, 0.69999999f}; // Decl: 29

// total size: 0x8
// Decl: 41
struct sEnvelopePoint {
    float fTime;  // offset 0x0, size 0x4
    float fValue; // offset 0x4, size 0x4
};

static const int bRumbleDebug = 0; // Decl: 46

sEnvelopePoint SmallRumblePoints[4] = {{0.0f, 255.0f}, {0.05f, 200.0f}, {0.07f, 80.0f}, {0.8f, 0.0f}};

sEnvelopePoint BigRumblePoints[4] = {{0.0f, 255.0f}, {0.15f, 255.0f}, {0.2f, 120.0f}, {1.2f, 0.0f}};

sEnvelopePoint ShockSwayRumblePoints[3] = {{0.0f, 55.0f}, {0.05f, 15.0f}, {0.1f, 0.0f}};

sEnvelopePoint RoadNoisePoints0[3] = {{0.0f, 80.0f}, {0.05f, 15.0f}, {0.1f, 0.0f}};

sEnvelopePoint RoadNoisePoints1[3] = {{0.0f, 80.0f}, {0.05f, 15.0f}, {0.1f, 0.0f}};

sEnvelopePoint BinaryRumblePoints[2] = {{0.0f, 255.0f}, {0.15f, 255.0f}};

sEnvelopePoint SingleRoadBumpRumblePoints[2] = {{0.0f, 140.0f}, {0.07f, 0.0f}};

sEnvelopePoint DoubleRoadBumpRumblePoints[4] = {{0.0f, 255.0f}, {0.05f, 0.0f}, {0.07f, 255.0f}, {0.15f, 0.0f}};

sEnvelopePoint ShakeAmplitudePoints[5] = {
    {0.0f, 1.0f}, {0.1f, 0.7f}, {0.3f, 0.5f}, {0.5f, 0.4f}, {1.0f, 0.0f},
};

sEnvelopePoint GearGrindPoints[3] = {{0.0f, 255.0f}, {0.075f, 20.0f}, {0.2f, 0.0f}};

sEnvelopePoint EngineHeatPoints[2] = {{0.0f, 45.0f}, {0.25f, 35.0f}};

sEnvelopePoint EngineRevPoints[2] = {{0.0f, 45.0f}, {0.01f, 1.0f}};

sEnvelopePoint NOSPoints[2] = {{0.0f, 70.0f}, {1.0f, 1.0f}};

sEnvelopePoint DriftPoints[2] = {{0.0f, 45.0f}, {0.01f, 0.0f}};

sEnvelopePoint BurnoutPoints[3] = {{0.0f, 55.0f}, {0.01f, 15.0f}, {0.05f, 0.0f}};

static const float fShockSwayIntensityCoeffs[4] = {9.0f, 40.0f, 0.94999999f, 1.5f};

static const float fDriftIntensityCoeffs[4] = {5.0f, 15.0f, 2.0f, 2.5f};

static const float fRoadNoiseIntensityCoeffs[6] = {0.001f, 0.025f, 1.1f, 1.5f, 25.0f, 85.0f};

static const float fShockBottomIntensityCoeffs[4] = {1.0f, 3.0f, 1.0f, 2.0f};

static const float fImpactIntensityCoeffs[4] = {2.0f, 11.0f, 0.30000001f, 2.0f};

static const float fBlownTiresIntensityCoeffs[4] = {2.0f, 11.0f, 0.30000001f, 2.0f};

static const float fRoadBumpIntensityCoeffs[4] = {0.0f, 1.0f, 0.30000001f, 2.0f};

static const float fGearGrindIntensityCoeffs[4] = {0.0f, 1.0f, 0.30000001f, 3.0f};

static const float fEngineHeatIntensityCoeffs[4] = {0.0f, 1.0f, 1.5f, 3.0f};

static const float fEngineRevIntensityCoeffs[4] = {0.30000001f, 0.89999998f, 1.2f, 2.75f};

static const float fNOSIntensityCoeffs[4] = {0.0f, 1.0f, 1.0f, 1.9f};

static const float fBurnoutIntensityCoeffs[4] = {1.0f, 10.0f, 1.15f, 1.9f}; // Decl: 258

// total size: 0x8
// Decl: 268
class tEnvelope {
  public:
    tEnvelope(sEnvelopePoint *p, int n) {
        this->pPoints = p;
        this->nPoints = n;
    }

    ~tEnvelope() {}

    bool OutOfRange(float t);
    float GetValue(float t);
    float GetSlope(float t);
    float GetIntegral(float t0, float t1);

    sEnvelopePoint *GetPoint(int i) {
        return &this->pPoints[i];
    }

  private:
  private:
    float GetSlope(int nIndex); // Decl: 282

    int GetIndex(float t); // Decl: 283

    sEnvelopePoint *pPoints; // offset 0x0, size 0x4
    int nPoints;             // offset 0x4, size 0x4, Decl: 285
};

tEnvelope SmallRumbleEnvelope(SmallRumblePoints, NUM_ELEMENTS(SmallRumblePoints)); // Decl: 292
tEnvelope BigRumbleEnvelope(BigRumblePoints, NUM_ELEMENTS(BigRumblePoints));
tEnvelope ShockSwayRumbleEnvelope(ShockSwayRumblePoints, NUM_ELEMENTS(ShockSwayRumblePoints));
tEnvelope RoadNoiseRumbleEnvelope0(RoadNoisePoints0, NUM_ELEMENTS(RoadNoisePoints0));
tEnvelope RoadNoiseRumbleEnvelope1(RoadNoisePoints1, NUM_ELEMENTS(RoadNoisePoints1));
tEnvelope BinaryRumbleEnvelope(BinaryRumblePoints, NUM_ELEMENTS(BinaryRumblePoints));
tEnvelope SingleRoadBumpRumbleEnvelope(SingleRoadBumpRumblePoints, NUM_ELEMENTS(SingleRoadBumpRumblePoints));
tEnvelope DoubleRoadBumpRumbleEnvelope(DoubleRoadBumpRumblePoints, NUM_ELEMENTS(DoubleRoadBumpRumblePoints));
tEnvelope ShakeAmplitudeEnvelope(ShakeAmplitudePoints, NUM_ELEMENTS(ShakeAmplitudePoints));
tEnvelope GearGrindEnvelope(GearGrindPoints, NUM_ELEMENTS(GearGrindPoints));
tEnvelope EngineHeatEnvelope(EngineHeatPoints, NUM_ELEMENTS(EngineHeatPoints));
tEnvelope EngineRevEnvelope(EngineRevPoints, NUM_ELEMENTS(EngineRevPoints));
tEnvelope NOSEnvelope(NOSPoints, NUM_ELEMENTS(NOSPoints));
tEnvelope DriftEnvelope(DriftPoints, NUM_ELEMENTS(DriftPoints));
tEnvelope BurnoutEnvelope(BurnoutPoints, NUM_ELEMENTS(BurnoutPoints)); // Decl: 306

int tEnvelope::GetIndex(float t) {
    int i = 0;
    for (; i < this->nPoints; i++) {
        if (t < this->pPoints[i].fTime) {
            break;
        }
    }
    if (i > 0) {
        return i - 1;
    }
    return i;
}

bool tEnvelope::OutOfRange(float t) {
    return (t < this->pPoints[0].fTime) || (t >= this->pPoints[this->nPoints - 1].fTime);
}

float tEnvelope::GetSlope(int nIndex) {
    if ((nIndex < 0) || (nIndex > this->nPoints - 2)) {
        return 0.0f;
    }
    float fTemp = this->pPoints[nIndex + 1].fValue - this->pPoints[nIndex].fValue;
    return fTemp / (this->pPoints[nIndex + 1].fTime - this->pPoints[nIndex].fTime);
}

// STRIPPED
float tEnvelope::GetSlope(float t) {}

float tEnvelope::GetValue(float t) {
    if (this->OutOfRange(t)) {
        return 0.0f;
    }

    int i = this->GetIndex(t);
    float s = this->GetSlope(i);

    return this->pPoints[i].fValue + s * (t - this->pPoints[i].fTime);
}

// STRIPPED
float tEnvelope::GetIntegral(float t0, float t1) {}

static const int MAX_RUMBLE_ENVELOPES = 3; // Decl: 461

// total size: 0x30
// Decl: 463
class tRumbler {
  public:
    tRumbler() {} // Decl: 466

    ~tRumbler() {} // Decl: 476

    bool StartRumble(tEnvelope *envelope, float amplitude, int8 priority); // Decl: 478

    bool Update(float fElapsed); // Decl: 479

    int GetValue() {} // Decl: 481

    int GetOldValue() {} // Decl: 482

    bool NewValue() {} // Decl: 483

    bool IsRumbling() {} // Decl: 484

    int GetEnvelopeValue(int index); // Decl: 486

  private:
    struct tEnvelope *pEnvelope[3]; // offset 0x0, size 0xC, Decl: 490
    float fTime[3];                 // offset 0xC, size 0xC, Decl: 491
    float fAmplitude[3];            // offset 0x18, size 0xC, Decl: 492
    int8 iPriority[3];              // offset 0x24, size 0x3, Decl: 493
    int nValue;                     // offset 0x28, size 0x4, Decl: 495
    int nValuePrevious;             // offset 0x2C, size 0x4, Decl: 496
};

// STRIPPED
int tRumbler::GetEnvelopeValue(int index) {}

// STRIPPED
bool tRumbler::StartRumble(tEnvelope *envelope, float amplitude, int8 priority) {}

// STRIPPED
bool tRumbler::Update(float fElapsed) {}

// total size: 0x20
// Decl: 734
class tShaker {
  public:
    void DoSnapshot(ReplaySnapshot *pSnapshot);
    float GetAmplitude();
    void StartShaking(bVector3 *pV, float fDur, float fFreq);
    void Update(float fDeltaTime);
    void GetValue(bVector3 *pV);

    tShaker() {
        this->Reset();
    }

    ~tShaker() {}

    void Reset() {
        bFill(&this->vShake, 0.0f, 0.0f, 0.0f);
        this->pAmplitude = nullptr;
        this->fTime = 0.0f;
        this->fDuration = 1.0f;
        this->fFrequency = 11.74265f;
    }

    // bool IsShaking() {}

  private:
    bVector3 vShake;       // offset 0x0, size 0x10
    tEnvelope *pAmplitude; // offset 0x10, size 0x4
    float fTime;           // offset 0x14, size 0x4
    float fDuration;       // offset 0x18, size 0x4
    float fFrequency;      // offset 0x1C, size 0x4
};

// STRIPPED
void tShaker::DoSnapshot(ReplaySnapshot *pSnapshot) {}

float tShaker::GetAmplitude() {
    if (this->pAmplitude != nullptr) {
        return this->pAmplitude->GetValue(this->fTime);
    }
    return 0.0f;
}

void tShaker::StartShaking(bVector3 *pV, float fDur, float fFreq) {
    bScale(&this->vShake, pV, -1.0f);
    this->pAmplitude = &ShakeAmplitudeEnvelope;
    this->fTime = 0.0f;
    this->fDuration = fDur;
    this->fFrequency = fFreq;
}

void tShaker::Update(float fDeltaTime) {
    if (this->pAmplitude != nullptr) {
        this->fTime += fDeltaTime / this->fDuration;
        if (this->pAmplitude->OutOfRange(this->fTime)) {
            this->pAmplitude = nullptr;
        }
    }
}

void tShaker::GetValue(bVector3 *pV) {
    float fShake = this->fTime * this->fFrequency;
    int nShake = static_cast<int>(fShake);
    bool bShake = ((nShake ^ 1) & 1) != 0;
    float fAmplitude;

    if (bShake && (this->pAmplitude != nullptr)) {
        fAmplitude = this->pAmplitude->GetValue(this->fTime);
    } else {
        fAmplitude = 0.0f;
    }
    bScale(pV, &vShake, fAmplitude);
}

// Decl: 805
tShaker CameraShakers[2];

void UpdateCameraShakers(float dT) {
    for (int i = 0; i < NUM_ELEMENTS(CameraShakers); i++) {
        CameraShakers[i].Update(dT);
    }
}

void ResetCameraShakers() {
    for (int i = 0; i < NUM_ELEMENTS(CameraShakers); i++) {
        CameraShakers[i].Reset();
    }
}

// STRIPPED
void SnapshotCameraShakers(ReplaySnapshot *pSnapshot) {}

void GetShake(int nPlayer, bVector3 *pV) {
    CameraShakers[nPlayer].GetValue(pV);
}

// STRIPPED
bool IsShakeActive(int nPlayer) {}

void ApplyCameraShake(int nViewID, bMatrix4 *pMatrix) {
    if (Sim::GetSpeed() < 1.0f) {
        return;
    }
    int nPlayer;
    switch (nViewID) {
        case 1:
        case 3:
            nPlayer = 0;
            break;
        case 2:
            nPlayer = 1;
            break;
        default:
            return;
    }
    bVector4 vShake;
    GetShake(nPlayer, reinterpret_cast<bVector3 *>(&vShake));
    vShake.w = 0.0f;

    bMatrix4 m;
    bIdentity(&m);
    eMulVector(&m.v3, pMatrix, &vShake);
    m.v3.w = 1.0f;
    eRotateX(&m, &m, static_cast<bAngle>(vShakeRotation.x * (m.v3.z + m.v3.y)));
    eRotateY(&m, &m, static_cast<bAngle>(vShakeRotation.y * m.v3.x));
    eRotateZ(&m, &m, static_cast<bAngle>(vShakeRotation.z * (m.v3.x + m.v3.y)));
    eMulMatrix(pMatrix, pMatrix, &m);
}

void ForceCameraShake(int nPlayer, bVector3 *pShake) {
    CameraShakers[nPlayer].StartShaking(pShake, fShakeDuration, fShakeFrequency);
}

void MaybeCameraShake(int nPlayer, bVector3 *pAccel) {
    bVector3 vAccel(vShakeAccelBias.x * pAccel->x, vShakeAccelBias.y * pAccel->y, vShakeAccelBias.z * pAccel->z);
    float fRatio = bLength(&vAccel);
    if (fRatio < fShakeIntensityCoeffs[0]) {
        return;
    }
    if (fRatio > fShakeIntensityCoeffs[1]) {
        fRatio = fShakeIntensityCoeffs[1];
    }
    fRatio -= fShakeIntensityCoeffs[0];
    fRatio /= fShakeIntensityCoeffs[1] - fShakeIntensityCoeffs[0];

    float fMagnitude = fRatio * (fShakeIntensityCoeffs[3] - fShakeIntensityCoeffs[2]) + fShakeIntensityCoeffs[2];
    if (fMagnitude > CameraShakers[nPlayer].GetAmplitude()) {
        bVector3 vShake;
        float fDuration = fRatio * (fShakeIntensityCoeffs[5] - fShakeIntensityCoeffs[4]) + fShakeIntensityCoeffs[4];
        bNormalize(&vShake, &vAccel, fMagnitude);
        CameraShakers[nPlayer].StartShaking(&vShake, fDuration, fShakeFrequency);
    }
}
