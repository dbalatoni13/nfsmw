#include "Rumble.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

// TODO
// static const int bShakeEnable;
int bShakeTest;
// static const int bShakeDebug;
static const float fShakeDuration = 0.6f;
static const float fShakeFrequency = 11.74265f;
bVector3 vShakeTest(0.0f, 0.0f, 0.3f);
static const bVector3 vShakeRotation(-1200.0f, 2000.0f, 0.0f);
static const bVector3 vShakeAccelBias(0.7f, 0.7f, 1.0f);
static const float fShakeIntensityCoeffs[6] = {100.0f, 600.0f, 0.1f, 0.4f, 0.3f, 0.69999999f};
// static const int bRumbleDebug;

// c34ord3 m7: cuerpo de tShaker::Reset, que el original tiene aqui
// (debug_lines: Rumble.cpp:741). Es POSICION, no optimizacion.
// r62 phys NEGATIVO estructural: las tres constantes de este cuerpo (0.0f,
// 1.0f, 11.74265f) NO se pueden sacar del pool a mano, porque el cuerpo se
// emite DOS VECES y cada copia usa DIRECCIONES DISTINTAS del objetivo: la que
// se mete inline en ResetCameraShakers apunta a +0x130C/+0x1310/+0x1314 y la
// copia fuera de linea `Reset__7tShaker` a lbl_803F662C/6630/6634. Un solo
// texto de fuente no puede dar las dos. Son 24 B de `.rodata` bloqueados salvo
// que se duplique el cuerpo. El resto de Rumble.cpp si cede (10 constantes,
// -40 B): GetSlope, GetValue, GetAmplitude, StartShaking, tShaker::GetValue,
// ApplyCameraShake, ForceCameraShake y MaybeCameraShake.
// scaf-data r71: las tres constantes al bloque (+0x130C/0x1310/0x1314). La
// copia fuera de linea `Reset__7tShaker` pasaba por $LC503-505 del pool2 y la
// inline por $LC428-430: con el extern mueren las DOS (24 B). El objetivo usa
// su propio pool (0x1DB4+) para la fuera de linea; la diferencia de direccion
// queda en la misma clase que la que ya hoy tolera objdiff.
inline void tShaker::Reset() {
    vShake.x = 0.0f;
    vShake.y = 0.0f;
    vShake.z = 0.0f;
    pAmplitude = nullptr;
    fTime = 0.0f;
    fDuration = 1.0f;
    fFrequency = 11.74265f;
}

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

// static const float fShockSwayIntensityCoeffs[4];
// static const float fDriftIntensityCoeffs[4];
// static const float fRoadNoiseIntensityCoeffs[6];
// static const float fShockBottomIntensityCoeffs[4];
// static const float fImpactIntensityCoeffs[4];
// static const float fBlownTiresIntensityCoeffs[4];
// static const float fRoadBumpIntensityCoeffs[4];
// static const float fGearGrindIntensityCoeffs[4];
// static const float fEngineHeatIntensityCoeffs[4];
// static const float fEngineRevIntensityCoeffs[4];
// static const float fNOSIntensityCoeffs[4];
// static const float fBurnoutIntensityCoeffs[4];

tEnvelope SmallRumbleEnvelope(SmallRumblePoints, sizeof(SmallRumblePoints) / sizeof(sEnvelopePoint));
tEnvelope BigRumbleEnvelope(BigRumblePoints, sizeof(BigRumblePoints) / sizeof(sEnvelopePoint));
tEnvelope ShockSwayRumbleEnvelope(ShockSwayRumblePoints, sizeof(ShockSwayRumblePoints) / sizeof(sEnvelopePoint));
tEnvelope RoadNoiseRumbleEnvelope0(RoadNoisePoints0, sizeof(RoadNoisePoints0) / sizeof(sEnvelopePoint));
tEnvelope RoadNoiseRumbleEnvelope1(RoadNoisePoints1, sizeof(RoadNoisePoints1) / sizeof(sEnvelopePoint));
tEnvelope BinaryRumbleEnvelope(BinaryRumblePoints, sizeof(BinaryRumblePoints) / sizeof(sEnvelopePoint));
tEnvelope SingleRoadBumpRumbleEnvelope(SingleRoadBumpRumblePoints, sizeof(SingleRoadBumpRumblePoints) / sizeof(sEnvelopePoint));
tEnvelope DoubleRoadBumpRumbleEnvelope(DoubleRoadBumpRumblePoints, sizeof(DoubleRoadBumpRumblePoints) / sizeof(sEnvelopePoint));
tEnvelope ShakeAmplitudeEnvelope(ShakeAmplitudePoints, sizeof(ShakeAmplitudePoints) / sizeof(sEnvelopePoint));
tEnvelope GearGrindEnvelope(GearGrindPoints, sizeof(GearGrindPoints) / sizeof(sEnvelopePoint));
tEnvelope EngineHeatEnvelope(EngineHeatPoints, sizeof(EngineHeatPoints) / sizeof(sEnvelopePoint));
tEnvelope EngineRevEnvelope(EngineRevPoints, sizeof(EngineRevPoints) / sizeof(sEnvelopePoint));
tEnvelope NOSEnvelope(NOSPoints, sizeof(NOSPoints) / sizeof(sEnvelopePoint));
tEnvelope DriftEnvelope(DriftPoints, sizeof(DriftPoints) / sizeof(sEnvelopePoint));
tEnvelope BurnoutEnvelope(BurnoutPoints, sizeof(BurnoutPoints) / sizeof(sEnvelopePoint));
// TODO
// static const int MAX_RUMBLE_ENVELOPES;

#define MAX_CAMERA_SHAKERS (2)

tShaker CameraShakers[MAX_CAMERA_SHAKERS];

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
// float tEnvelope::GetSlope(float t) {
//     int i;
// }

float tEnvelope::GetValue(float t) {
    if (OutOfRange(t)) {
        return 0.0f;
    }

    int i = GetIndex(t);
    float s = GetSlope(i);

    return this->pPoints[i].fValue + s * (t - this->pPoints[i].fTime);
}

float tShaker::GetAmplitude() {
    if (this->pAmplitude) {
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
    if (this->pAmplitude) {
        this->fTime += fDeltaTime / this->fDuration;
        if (this->pAmplitude->OutOfRange(this->fTime)) {
            this->pAmplitude = nullptr;
        }
    }
}

void tShaker::GetValue(bVector3 *pV) {
    float fShake = this->fTime * this->fFrequency;
    int nShake = static_cast<int>(fShake);
    bool bShake = (nShake ^ 1) & 1;
    float fAmplitude;

    if (bShake && this->pAmplitude) {
        fAmplitude = this->pAmplitude->GetValue(this->fTime);
    } else {
        fAmplitude = 0.0f;
    }
    bScale(pV, &vShake, fAmplitude);
}

void UpdateCameraShakers(float dT) {
    for (int i = 0; i < MAX_CAMERA_SHAKERS; i++) {
        CameraShakers[i].Update(dT);
    }
}

void ResetCameraShakers() {
    for (int i = 0; i < MAX_CAMERA_SHAKERS; i++) {
        CameraShakers[i].Reset();
    }
}

// STRIPPED
void SnapshotCameraShakers(ReplaySnapshot *pSnapshot) {}

void GetShake(int nPlayer, bVector3 *pV) {
    CameraShakers[nPlayer].GetValue(pV);
}

// STRIPPED
// bool IsShakeActive(int nPlayer) {}

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
    GetShake(nPlayer, (bVector3 *)&vShake);
    vShake.w = 0.0f;

    bMatrix4 m;
    bIdentity(&m);
    eMulVector(&m.v3, pMatrix, &vShake);
    m.v3.w = 1.0f;
    eRotateX(&m, &m, static_cast<unsigned short>(vShakeRotation.x * (m.v3.z + m.v3.y)));
    eRotateY(&m, &m, static_cast<unsigned short>(vShakeRotation.y * m.v3.x));
    eRotateZ(&m, &m, static_cast<unsigned short>(vShakeRotation.z * (m.v3.x + m.v3.y)));
    eMulMatrix(pMatrix, pMatrix, &m);
}

void ForceCameraShake(int nPlayer, bVector3 *pShake) {
    CameraShakers[nPlayer].StartShaking(pShake, 0.6f, 11.74265f);
}

void MaybeCameraShake(int nPlayer, bVector3 *pAccel) {
    bVector3 vAccel(vShakeAccelBias.x * pAccel->x, vShakeAccelBias.y * pAccel->y, vShakeAccelBias.z * pAccel->z);
    float fRatio = bLength(vAccel);
    if (fRatio >= fShakeIntensityCoeffs[0]) {
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
            CameraShakers[nPlayer].StartShaking(&vShake, fDuration, 11.74265f);
        }
    }
}

#undef MAX_CAMERA_SHAKERS
