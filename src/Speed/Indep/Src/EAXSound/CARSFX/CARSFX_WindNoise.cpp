#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_WindNoise.hpp"

#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"

namespace Csis {
extern ClassHandle gFX_WINDHandle;
extern InterfaceId FX_WINDId;
} // namespace Csis

extern int IsSoundEnabled;

struct FX_WINDStruct {
    int pitch;             // offset 0x0, size 0x4
    int volume_left;       // offset 0x4, size 0x4
    int volume_right;      // offset 0x8, size 0x4
    int azimuth_left;      // offset 0xC, size 0x4
    int azimuth_right;     // offset 0x10, size 0x4
    int intensity;         // offset 0x14, size 0x4
    int camera;            // offset 0x18, size 0x4
    int override_Trigger;  // offset 0x1C, size 0x4
    int rumble_Volume;     // offset 0x20, size 0x4
    int lowPass;           // offset 0x24, size 0x4
    int hiPass;            // offset 0x28, size 0x4
    int fX_Dry;            // offset 0x2C, size 0x4
    int fX_wet;            // offset 0x30, size 0x4
    int panning_Depth;     // offset 0x34, size 0x4
};

struct FX_WIND {
    static void *operator new(unsigned int size) {
        return Csis::System::Alloc(size);
    }

    static void operator delete(void *ptr) {
        Csis::System::Free(ptr);
    }

    FX_WIND(int pitch, int volume_left, int volume_right, int azimuth_left, int azimuth_right, int intensity,
            int camera, int override_Trigger, int rumble_Volume, int lowPass, int hiPass, int fX_Dry, int fX_wet,
            int panning_Depth)
        : mpClass(nullptr) {
        Csis::System::Result result;

        SetPitch(pitch);
        SetVolume_left(volume_left);
        SetVolume_right(volume_right);
        SetAzimuth_left(azimuth_left);
        SetAzimuth_right(azimuth_right);
        SetIntensity(intensity);
        SetCamera(camera);
        SetOverride_Trigger(override_Trigger);
        SetRumble_Volume(rumble_Volume);
        SetLowPass(lowPass);
        SetHiPass(hiPass);
        SetFX_Dry(fX_Dry);
        SetFX_wet(fX_wet);
        SetPanning_Depth(panning_Depth);

        result = static_cast<Csis::System::Result>(Csis::Class::CreateInstance(&Csis::gFX_WINDHandle, &mData, &mpClass));
        if (result < Csis::System::kResult_Ok) {
            Csis::gFX_WINDHandle.Set(&Csis::FX_WINDId);
            Csis::Class::CreateInstance(&Csis::gFX_WINDHandle, &mData, &mpClass);
        }
    }

    ~FX_WIND() {
        if (mpClass) {
            mpClass->Release();
        }
    }

    void CommitMemberData() {
        if (mpClass) {
            mpClass->SetMemberData(&mData);
        }
    }

    int GetRefCount() {
        int refCount;

        refCount = 0;
        if (mpClass) {
            mpClass->GetRefCount(&refCount);
        }
        return refCount;
    }

    void SetPitch(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x4000) {
            x = 0x4000;
        }
        mData.pitch = x;
    }

    void SetVolume_left(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        mData.volume_left = x;
    }

    void SetVolume_right(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        mData.volume_right = x;
    }

    void SetAzimuth_left(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0xFFFF) {
            x = 0xFFFF;
        }
        mData.azimuth_left = x;
    }

    void SetAzimuth_right(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0xFFFF) {
            x = 0xFFFF;
        }
        mData.azimuth_right = x;
    }

    void SetIntensity(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x410) {
            x = 0x410;
        }
        mData.intensity = x;
    }

    void SetCamera(int x) {
        mData.camera = x;
    }

    void SetOverride_Trigger(int x) {
        mData.override_Trigger = x;
    }

    void SetRumble_Volume(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        mData.rumble_Volume = x;
    }

    void SetLowPass(int x) {
        mData.lowPass = x;
    }

    void SetHiPass(int x) {
        mData.hiPass = x;
    }

    void SetFX_Dry(int x) {
        mData.fX_Dry = x;
    }

    void SetFX_wet(int x) {
        mData.fX_wet = x;
    }

    void SetPanning_Depth(int x) {
        mData.panning_Depth = x;
    }

    Csis::Class *mpClass;  // offset 0x0, size 0x4
    FX_WINDStruct mData;   // offset 0x4, size 0x38
};

SndBase::TypeInfo SFXCTL_3DLeftWindPos::s_TypeInfo = {
    0x000200D0, "SFXCTL_3DLeftWindPos", &SFXCTL_3DObjPos::s_TypeInfo, SFXCTL_3DLeftWindPos::CreateObject};
SndBase::TypeInfo SFXCTL_3DRightWindPos::s_TypeInfo = {
    0x000200E0, "SFXCTL_3DRightWindPos", &SFXCTL_3DObjPos::s_TypeInfo, SFXCTL_3DRightWindPos::CreateObject};
SndBase::TypeInfo CARSFX_WindNoise::s_TypeInfo = {
    0x00020090, "CARSFX_WindNoise", &SndBase::s_TypeInfo, CARSFX_WindNoise::CreateObject};

SndBase::TypeInfo *SFXCTL_3DLeftWindPos::GetTypeInfo() const {
    return &s_TypeInfo;
}

const char *SFXCTL_3DLeftWindPos::GetTypeName() const {
    return s_TypeInfo.typeName;
}

SndBase *SFXCTL_3DLeftWindPos::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (s_TypeInfo.typeName, false) SFXCTL_3DLeftWindPos();
    }
    return new (s_TypeInfo.typeName, true) SFXCTL_3DLeftWindPos();
}

SndBase::TypeInfo *SFXCTL_3DRightWindPos::GetTypeInfo() const {
    return &s_TypeInfo;
}

const char *SFXCTL_3DRightWindPos::GetTypeName() const {
    return s_TypeInfo.typeName;
}

SndBase *SFXCTL_3DRightWindPos::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (s_TypeInfo.typeName, false) SFXCTL_3DRightWindPos();
    }
    return new (s_TypeInfo.typeName, true) SFXCTL_3DRightWindPos();
}

SndBase::TypeInfo *CARSFX_WindNoise::GetTypeInfo() const {
    return &s_TypeInfo;
}

const char *CARSFX_WindNoise::GetTypeName() const {
    return s_TypeInfo.typeName;
}

SndBase *CARSFX_WindNoise::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (s_TypeInfo.typeName, false) CARSFX_WindNoise();
    }
    return new (s_TypeInfo.typeName, true) CARSFX_WindNoise();
}

CARSFX_WindNoise::CARSFX_WindNoise()
    : CARSFX() {
    IsInitialized = false;
    m_bIsInSpacialTranslation = false;
    m_pEAXCar = nullptr;
    m_v3CarBaseLeftPos = bVector3(0.0f, 0.0f, 0.0f);
    m_v3CarBaseRightPos = bVector3(0.0f, 0.0f, 0.0f);
    m_pCsisWind = nullptr;
}

CARSFX_WindNoise::~CARSFX_WindNoise() {
    Destroy();
}

void CARSFX_WindNoise::Destroy() {
    if (m_pCsisWind) {
        delete m_pCsisWind;
        m_pCsisWind = nullptr;
    }
}

int CARSFX_WindNoise::GetController(int Index) {
    if (Index == 0) {
        return 0xE;
    }
    if (Index == 1) {
        return 0xD;
    }
    return -1;
}

void CARSFX_WindNoise::AttachController(SFXCTL *psfxctl) {
    unsigned int objIndex;

    objIndex = psfxctl->GetObjectIndex();
    if (objIndex == 0xD) {
        m_p3DLeftWindPos = static_cast<SFXCTL_3DLeftWindPos *>(psfxctl);
    } else if (objIndex == 0xE) {
        m_p3DRightWindPos = static_cast<SFXCTL_3DRightWindPos *>(psfxctl);
    }
}

int CARSFX_WindNoise::UpdateMasterVolume() {
    return 0;
}

void CARSFX_WindNoise::SetupSFX(CSTATE_Base *_StateBase) {
    int i;

    SndBase::SetupSFX(_StateBase);
    for (i = 0; i < 2; i++) {
        m_stWindParams[i].nCurrentChannel = i & 1;
    }
}

void CARSFX_WindNoise::InitSFX() {
    if (IsSoundEnabled) {
        SndBase::InitSFX();
        IsInitialized = true;
        m_nLastTickUpdate = bGetTicker();
        m_bIsInSpacialTranslation = false;
        g_pEAXSound->SetCsisName(this);
        m_pCsisWind = new FX_WIND(0x1000, 0, 0, 0, 0, 0, 0, 0, 0, 25000, 0, 0x7FFF, 0, 0xF);
        m_p3DLeftWindPos->AssignPositionVector(&m_v3CarBaseLeftPos);
        m_p3DLeftWindPos->AssignDirectionVector(nullptr);
        m_p3DRightWindPos->AssignPositionVector(&m_v3CarBaseRightPos);
        m_p3DRightWindPos->AssignDirectionVector(nullptr);
        Enable();
    }
}

void CARSFX_WindNoise::ProcessUpdate() {
    if (IsEnabled() && m_pCsisWind) {
        UpdateCSISParams();
    }
}

void CARSFX_WindNoise::UpdateParams(float t) {
    Camera *pcam;
    EAX_CarState *pcar;
    float fcurvel;
    float fratio;
    int nAngleSep;
    const bVector3 *pv3CarDir;
    const bVector3 *pv3CarPos;
    bVector3 v3NewPosLeft;
    bVector3 v3NewPosRight;
    int i;

    if (!IsEnabled()) {
        return;
    }

    m_fmsTickDiff = bGetTickerDifference(m_nLastTickUpdate, bGetTicker());
    m_nLastTickUpdate = bGetTicker();
    pcam = eGetView(1, false)->GetCamera();
    if (pcam) {
        pcar = GetPhysCar();
        fcurvel = pcar->GetVelocityMagnitude();
        if (fcurvel < 2.0f) {
            fcurvel = 2.0f;
        } else if (fcurvel > 40.0f) {
            fcurvel = 39.95f;
        }

        fratio = fcurvel * 0.025f;
        m_stWindParams[0].nCrossFadeWeight = static_cast<int>(fratio * 1040.0f);
        nAngleSep = static_cast<short>(static_cast<int>(fratio * 12288.0f)) + 0x500;
        m_nVelocityWeightedVolume = static_cast<int>(fratio * 32767.0f);
        m_fcurwindradius = (1.0f - fratio) * 65.0f;
        if (m_fcurwindradius < m_pEAXCar->m_fSphereRadius) {
            m_fcurwindradius = m_pEAXCar->m_fSphereRadius * 1.01f;
        }

        pv3CarDir = pcar->GetForwardVector();
        pv3CarPos = pcar->GetPosition();
        v3NewPosLeft.x = m_fcurwindradius *
                         (bCos(static_cast<bAngle>(nAngleSep)) * pv3CarDir->x -
                          bSin(static_cast<bAngle>(nAngleSep)) * pv3CarDir->y);
        v3NewPosLeft.y = m_fcurwindradius *
                         (bSin(static_cast<bAngle>(nAngleSep)) * pv3CarDir->x +
                          bCos(static_cast<bAngle>(nAngleSep)) * pv3CarDir->y);
        v3NewPosLeft.z = pv3CarPos->z;

        nAngleSep = ~nAngleSep;
        v3NewPosRight.x = m_fcurwindradius *
                          (bCos(static_cast<bAngle>(nAngleSep)) * pv3CarDir->x -
                           bSin(static_cast<bAngle>(nAngleSep)) * pv3CarDir->y);
        v3NewPosRight.y = m_fcurwindradius *
                          (bSin(static_cast<bAngle>(nAngleSep)) * pv3CarDir->x +
                           bCos(static_cast<bAngle>(nAngleSep)) * pv3CarDir->y);
        v3NewPosRight.z = pv3CarPos->z;

        for (i = 0; i < 2; i++) {
            if (m_stWindParams[i].nCurrentChannel == 0) {
                m_v3CarBaseLeftPos.x = v3NewPosLeft.x + pv3CarPos->x;
                m_v3CarBaseLeftPos.y = v3NewPosLeft.y + pv3CarPos->y;
                m_v3CarBaseLeftPos.z = v3NewPosLeft.z;
            } else {
                m_v3CarBaseRightPos.x = v3NewPosRight.x + pv3CarPos->x;
                m_v3CarBaseRightPos.y = v3NewPosRight.y + pv3CarPos->y;
                m_v3CarBaseRightPos.z = v3NewPosRight.z;
            }
        }
    }
}

void CARSFX_WindNoise::UpdateCSISParams() {
    int nLeftVolume;
    int nRightVolume;
    int nRumbleVolume;
    int refcount;

    nLeftVolume = GetDMixOutput(2, DMX_VOL) * m_nVelocityWeightedVolume >> 15;
    nRightVolume = GetDMixOutput(3, DMX_VOL) * m_nVelocityWeightedVolume >> 15;
    nRumbleVolume = GetDMixOutput(4, DMX_VOL) * m_nVelocityWeightedVolume >> 15;
    refcount = m_pCsisWind->GetRefCount();
    m_pCsisWind->SetVolume_left(bClamp(nLeftVolume, 0, 0x7FFF));
    m_pCsisWind->SetVolume_right(bClamp(nRightVolume, 0, 0x7FFF));
    m_pCsisWind->SetRumble_Volume(bClamp(nRumbleVolume, 0, 0x7FFF));
    m_pCsisWind->SetPitch(bClamp(GetDMixOutput(5, DMX_PITCH), 0, 0x4000));
    m_pCsisWind->SetAzimuth_left(bClamp(GetDMixOutput(0, DMX_AZIM), 0, 0xFFFF));
    m_pCsisWind->SetAzimuth_right(bClamp(GetDMixOutput(1, DMX_AZIM), 0, 0xFFFF));
    m_pCsisWind->SetIntensity(bClamp(m_stWindParams[0].nCrossFadeWeight, 0, 0x410));
    m_pCsisWind->SetFX_Dry(0x7FFF);
    m_pCsisWind->CommitMemberData();
}
