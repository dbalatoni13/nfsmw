#include "Speed/Indep/Src/EAXSound/EAXTunerCar.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"

static const float T_CRUISE_MIX_DELAY = 6.0f; // Decl: 79

static const int MEM_LEAK_DISABLE_NITROUS = 0;  // Decl: 82
static const int MEM_LEAK_DISABLE_SHIFTING = 0; // Decl: 83
static const int MEM_LEAK_DISABLE_TURBO = 0;    // Decl: 84

static const int SND_PlayerCreateFX = 1; // Decl: 89

Slope BreakingPitchVsSpeed(3000.0f, 5000.0f, 30.0f, 100.0f); // Decl: 102

static const float STEADY_STATE_TORQUE = 7.0f;                // Decl: 108
static const int TIME_TO_REACH_STEADY_STATE_TORQUE_MS = 4000; // Decl: 109

#define FL 0 // Decl: 118
#define FR 1 // Decl: 119
#define RR 2 // Decl: 120
#define RL 3 // Decl: 121

STATETYPE_IMPLEMENT(0x20000, EAXTunerCar, EAXCar);

EAXTunerCar::EAXTunerCar() {
    this->bFirstUpdate = true;
    this->TrunkBounceInstensity = 0.0f;
    this->BottomOutPlay = false;
    this->TrunkBouncePlay = false;
    this->PlayBackFire = false;
}

EAXTunerCar::~EAXTunerCar() {}

int ntesthackbullshit = 0;                        // Decl: 161
static const float gfTCarWindRadius = 6.0f;       // Decl: 162
static const int gnMinWindSeparationAngle = 2730; // Decl: 163
static const int gnMaxWindSeparationAngle = 8191; // Decl: 164

void EAXTunerCar::PreLoadAssets() {}

void EAXTunerCar::ProcessSoundSphere(uint32 unamehash, int nparamid, bVector3 *pv3pos, float fradius) {}

// TODO unsolved, scheduling
int EAXTunerCar::SFXMessage(eSFXMessageType SFXMessageType, uint32 param1, uint32 param2) {
    switch (SFXMessageType) {
        case SFX_NONE:
            break;
        case SFX_SHIFT_UP:
        case SFX_SHIFT_DOWN:
        case SFX_NITROUS:
            break;
        case SFX_BOTTOMOUT:
            this->BottomOutPlay = true;
            this->BottomOutIntensity = param1 >> 8;
            break;
        case SFX_TRUNKBOUNCE:
            this->TrunkBouncePlay = true;
            this->TrunkBounceInstensity = *reinterpret_cast<float *>(&param1);
            break;
        case SFX_CHANGEGEAR:
            return 0;
        default:
            break;
    }
    return EAXCar::SFXMessage(SFXMessageType, param1, param2);
}

void DebugPrintSkidBar(int Horz, int Vert, char *Str, int Value) {}

void DebugPrintColumn(int x, int y, char *label, unsigned int label_color, float val, unsigned int bar_color) {}

// UNSOLVED
int EAXTunerCar::UpdateRotation() {
    this->m_Rotation = bClamp(this->m_Rotation, 0, 1024);
    return this->m_Rotation;
}

void EAXTunerCar::UpdatePov() {
    CameraMover *cm = eGetView(1, false)->GetCameraMover();

    if (cm != nullptr) {
        CameraAnchor *anchor = cm->GetAnchor();

        this->m_IsDriveCamera = static_cast<int>(cm->IsDriveCamera());

        if (anchor != nullptr) {
            this->m_PovType = static_cast<int>(anchor->GetPOVType());
        } else {
            this->m_PovType = 7;
        }
    } else {
        this->m_IsDriveCamera = 0;
    }
}

void EAXTunerCar::FirstUpdate(float t) {
    this->bFirstUpdate = false;
}

int gnTestSoundSphereCrap = 0; // Decl: 450

bool gbTunerCarUpdate = false; // Decl: 452

void EAXTunerCar::UpdateParams(float t) {
    EAXCar::UpdateParams(t);
    if (this->m_pCar != nullptr) {
        if (this->bFirstUpdate) {
            this->FirstUpdate(t);
        }
        this->UpdatePov();
        this->UpdateRotation();
    }
}
