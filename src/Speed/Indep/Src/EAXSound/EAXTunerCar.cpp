#include "Speed/Indep/Src/EAXSound/EAXTunerCar.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"

STATETYPE_IMPLEMENT(0x00020000, EAXTunerCar, EAXCar)

EAXTunerCar::EAXTunerCar() {
    this->bFirstUpdate = true;
    this->TrunkBounceInstensity = 0.0f;
    this->BottomOutPlay = false;
    this->TrunkBouncePlay = false;
    this->PlayBackFire = false;
}

EAXTunerCar::~EAXTunerCar() {
}

void EAXTunerCar::PreLoadAssets() {
}

void EAXTunerCar::ProcessSoundSphere(uint32 unamehash, int nparamid, bVector3 *pv3pos, float fradius) {
}

int EAXTunerCar::SFXMessage(eSFXMessageType SFXMessageType, uint32 param1, uint32 param2) {
    switch (SFXMessageType) {
        case SFX_NONE:
            break;
        case SFX_BOTTOMOUT:
            this->BottomOutPlay = true;
            this->BottomOutIntensity = param1 >> 8;
            break;
        case SFX_TRUNKBOUNCE:
            this->TrunkBouncePlay = true;
            this->TrunkBounceInstensity = *(float *)&param1;
            break;
        case SFX_SHIFT_UP:
        case SFX_SHIFT_DOWN:
        case SFX_NITROUS:
            return EAXCar::SFXMessage(SFXMessageType, param1, param2);
        case SFX_CHANGEGEAR:
            return 0;
        default:
            break;
    }

    return EAXCar::SFXMessage(SFXMessageType, param1, param2);
}

void DebugPrintSkidBar(int Horz, int Vert, char *Str, int Value) {
}

int EAXTunerCar::UpdateRotation() {
    int rot = 0;

    this->m_Rotation = rot;
    this->m_Rotation = bClamp(*(int *)&rot, 0, 0x400);

    return this->m_Rotation;
}

void EAXTunerCar::UpdatePov() {
    CameraMover *cm = eGetView(1, false)->GetCameraMover();

    if (cm != nullptr) {
        CameraAnchor *anchor = cm->GetAnchor();
        this->m_IsDriveCamera = cm->IsDriveCamera();

        if (anchor != nullptr) {
            this->m_PovType = anchor->GetPOVType();
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

void EAXTunerCar::UpdateParams(float t) {
    EAXCar::UpdateParams(t);

    if (this->GetPhysCar() != nullptr) {
        if (this->bFirstUpdate != false) {
            this->FirstUpdate(t);
        }

        this->UpdatePov();
        this->UpdateRotation();
    }
}
