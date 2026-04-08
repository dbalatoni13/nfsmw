#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_Siren.hpp"

#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Misc/Config.h"

namespace Sound {
float DistanceToView(const bVector3 *position);
}

namespace Csis {
InterfaceId SIRENId = {"SIREN", 0x4683, 0x0362};
InterfaceId SIREN_BEDId = {"SIREN_BED", 0x4683, 0x4321};
ClassHandle gSIRENHandle;
ClassHandle gSIREN_BEDHandle;
} // namespace Csis

CARSFX_Siren::TypeInfo CARSFX_Siren::s_TypeInfo = {
    0x00040110,
    "CARSFX_Siren",
    &SndBase::s_TypeInfo,
    CARSFX_Siren::CreateObject,
};

CARSFX_Siren::TypeInfo *CARSFX_Siren::GetTypeInfo() const {
    return &s_TypeInfo;
}

const char *CARSFX_Siren::GetTypeName() const {
    return s_TypeInfo.typeName;
}

SndBase *CARSFX_Siren::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (s_TypeInfo.typeName, false) CARSFX_Siren();
    }
    return new (s_TypeInfo.typeName, true) CARSFX_Siren();
}

CARSFX_Siren::CARSFX_Siren()
    : CARSFX() {
    mSiren = nullptr;
    mT_death = Timer(0);
}

CARSFX_Siren::~CARSFX_Siren() {
    Destroy();
}

void CARSFX_Siren::Detach() {
    Destroy();
}

void CARSFX_Siren::Destroy() {
    if (mSiren) {
        if (mSiren->mpClass) {
            mSiren->mpClass->Release();
        }
        Csis::System::Free(mSiren);
    }
    mSiren = nullptr;
}

int CARSFX_Siren::GetController(int) {
    return -1;
}

void CARSFX_Siren::AttachController(SFXCTL *) {}

void CARSFX_Siren::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
}

void CARSFX_Siren::InitSFX() {
    if (IsSoundEnabled != 0) {
        SndBase::InitSFX();
        bIgnoreHealth = GetPhysCar()->mHealth < 0.01f;
        m_PrevSirenState = SIREN_OFF;
        m_SirenState = SIREN_OFF;
        tSirenState = g_pEAXSound->Random(2.0f) + 6.0f;
    }
}

void CARSFX_Siren::UpdateParams(float) {
    EAX_CarState *thisCar;
    float t_death;
    SirenState state;

    thisCar = GetPhysCar();
    if (!thisCar) {
        return;
    }

    t_death = (WorldTimer - mT_death).GetSeconds();
    state = thisCar->mSirenState;
    if (state == SIREN_OFF || (m_SirenState == SIREN_DIE && t_death > 10.0f)) {
        Disable();
    } else {
        Enable();
    }

    if (state == SIREN_OFF) {
        if (IsEnabled()) {
            Disable();
        }
        return;
    }

    if (!IsEnabled()) {
        Enable();
    }
}

void CARSFX_Siren::ProcessUpdate() {
    int CurDist;
    int input;
    SIREN *siren;

    if (IsEnabled()) {
        SetDMIX_Input(5, 0x7FFF);
        siren = mSiren;
        if (!siren) {
            g_pEAXSound->SetCsisName(this);
            siren = new SIREN(0, 0, 0, 0, 0, 25000, 0, 0x7FFF, 0);
            mSiren = siren;
            if (!siren) {
                return;
            }
        }

        input = GetDMixOutput(1, DMX_VOL);
        if (input < 0) {
            input = 0;
        } else if (input > 0x7FFF) {
            input = 0x7FFF;
        }
        siren->mData.vOL = input;

        input = GetDMixOutput(0, DMX_AZIM);
        if (input < 0) {
            input = 0;
        } else if (input > 0xFFFF) {
            input = 0xFFFF;
        }
        siren->mData.pAN = input;

        input = GetDMixOutput(3, DMX_PITCH) - 0x1000;
        if (input < -0x4000) {
            input = -0x4000;
        } else if (input > 0x4000) {
            input = 0x4000;
        }
        siren->mData.pITCH_OFFS = input;

        CurDist = static_cast<int>(Sound::DistanceToView(m_pStateBase->GetPhysCar()->GetPosition()) * 12.8f);
        if (CurDist < 0) {
            CurDist = 0;
        } else if (CurDist > 0x400) {
            CurDist = 0x400;
        }
        siren->mData.dISTANCE = CurDist;

        m_PrevSirenState = m_SirenState;
        m_SirenState = UpdateSirenState(m_pStateBase->GetDeltaTime());
        input = 0;
        switch (m_SirenState) {
        case SIREN_WAIL:
        case SIREN_YELP:
        case SIREN_SCREAM:
            input = m_SirenState;
            mT_death = WorldTimer;
            break;
        case SIREN_DIE:
            input = SIREN_DIE;
            if (m_PrevSirenState != SIREN_DIE) {
                mT_death = WorldTimer;
            }
            break;
        case SIREN_OFF:
            Disable();
            break;
        default:
            break;
        }

        if (input < SIREN_WAIL) {
            input = SIREN_WAIL;
        } else if (input > 7) {
            input = 7;
        }
        siren->mData.tYPE = input;
        siren->CommitMemberData();
    } else {
        SetDMIX_Input(5, 0);
        siren = mSiren;
        if (!siren) {
            return;
        }
        if (siren->mpClass) {
            siren->mpClass->Release();
        }
        Csis::System::Free(siren);
        mSiren = nullptr;
    }
}

SirenState CARSFX_Siren::UpdateSirenState(float t) {
    float fVar2;

    fVar2 = tSirenState - t;
    tSirenState = fVar2;
    if (fVar2 < 0.0f || m_pStateBase->GetPhysCar()->mSirenState == SIREN_SCREAM) {
        tSirenState = g_pEAXSound->Random(3.0f);
        return m_pStateBase->GetPhysCar()->mSirenState;
    }
    return m_SirenState;
}
