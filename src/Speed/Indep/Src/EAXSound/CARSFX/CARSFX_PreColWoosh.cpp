#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_PreColWoosh.hpp"

#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Woosh.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_DriveBy.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

extern Slope g_WooshVol_vs_Vel;

CARSFX_PreColWoosh::TypeInfo *CARSFX_PreColWoosh::GetTypeInfo() const {
    return &s_TypeInfo;
}

const char *CARSFX_PreColWoosh::GetTypeName() const {
    return s_TypeInfo.typeName;
}

SndBase *CARSFX_PreColWoosh::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (GetStaticTypeInfo()->typeName, false) CARSFX_PreColWoosh();
    }
    return new (GetStaticTypeInfo()->typeName, true) CARSFX_PreColWoosh();
}

CARSFX_PreColWoosh::CARSFX_PreColWoosh()
    : CARSFX() {
    mMsgBarrier = Hermes::Handler::Create<MAudioReflection, CARSFX_PreColWoosh, CARSFX_PreColWoosh>(
        this, &CARSFX_PreColWoosh::MsgBarrier, UCrc32("FRONT_BARRIER"), 0);
    mMsgBarrierHit = Hermes::Handler::Create<MAudioReflection, CARSFX_PreColWoosh, CARSFX_PreColWoosh>(
        this, &CARSFX_PreColWoosh::MsgBarrierHit, UCrc32("FRONT_BARRIER_HIT"), 0);
    m_pWoosh = nullptr;
    mDurationActive = 0.0f;
    mResetTime = 0.0f;
}

CARSFX_PreColWoosh::~CARSFX_PreColWoosh() {
    if (mMsgBarrier) {
        Hermes::Handler::Destroy(mMsgBarrier);
    }

    if (mMsgBarrierHit) {
        Hermes::Handler::Destroy(mMsgBarrierHit);
    }

    if (m_pWoosh) {
        delete m_pWoosh;
    }

    m_pWoosh = nullptr;
}

void CARSFX_PreColWoosh::InitSFX() {
    SndBase::InitSFX();
    bGoingToCollide = false;
    bBailOnAll = false;
    bBarrierDetected = false;
    WooshFadeOut.Initialize(1.0f, 1.0f, 1, LINEAR);
}

void CARSFX_PreColWoosh::Destroy() {
    SndBase::Destroy();
}

void CARSFX_PreColWoosh::MsgBarrier(const MAudioReflection &message) {
    if (message.GetPlayerNum() != m_pStateBase->m_StateInstType) {
        return;
    }

    bBarrierDetected = true;
    if (!bGoingToCollide && !bBailOnAll && mResetTime == 0.0f) {
        bGoingToCollide = true;
        mDurationActive = 0.0f;
        WooshFadeOut.Initialize(1.0f, 1.0f, 1, LINEAR);
    }
}

void CARSFX_PreColWoosh::MsgBarrierHit(const MAudioReflection &message) {
    if (GetPhysCar() && message.GetPlayerNum() == static_cast<int>(GetPhysCar()->mWorldID)) {
        BailOnWoosh();
    }
}

void CARSFX_PreColWoosh::BailOnWoosh() {
    bBailOnAll = true;
    bGoingToCollide = false;
    WooshFadeOut.Initialize(WooshFadeOut.GetValue(), 0.0f, 0x82, LINEAR);
    mResetTime = 0.4f;
}

void CARSFX_PreColWoosh::Detach() {
    SndBase::Detach();
}

void CARSFX_PreColWoosh::UpdateParams(float t) {
    SndBase::UpdateParams(t);
    WooshFadeOut.Update(t);
    mResetTime -= t;
    if (mResetTime < 0.0f) {
        mResetTime = 0.0f;
    }

    SetDMIX_Input(2, 0);
    if (bGoingToCollide) {
        SND_Stich *NewStichData;
        int iVar3;
        float fVar4;
        float fVar5 = 0.0f;
        SND_Params sndparams;
        STICH_WHOOSH_TYPE base;
        int numblocks;
        int sizeperblock;

        SetDMIX_Input(2, 0x7FFF);
        mDurationActive += t;
        if (!m_pWoosh) {
            fVar4 = bClamp(g_WooshVol_vs_Vel.GetValue(GetPhysCar()->GetVelocityMagnitude()), fVar5, 0.99f);
            fVar4 = bClamp(fVar4 * 127.0f, fVar5, 127.0f);
            GetWooshBlockSizeParams(DRIVE_BY_PRE_COL, base, numblocks, sizeperblock);
            {
                static int LastRandom;

                iVar3 = LastRandom - (LastRandom / sizeperblock) * sizeperblock;
                LastRandom = iVar3 + 1;
            }

            NewStichData = &g_pEAXSound->GetStichPlayer()->GetStich(
                STICH_TYPE_WOOSH, base + static_cast<int>(fVar4 * static_cast<float>(numblocks) * 0.0078125f) * sizeperblock + iVar3);
            sndparams.ID = 0;
            sndparams.Mag = 0;
            sndparams.RVerb = 0;
            sndparams.Az = 0;
            sndparams.Pitch = 0;
            sndparams.Vol = 0;
            m_pWoosh = new cStichWrapper(*NewStichData);
            m_pWoosh->Play(&sndparams);
        }

        if (!bBarrierDetected || mDurationActive > 0.4f) {
            BailOnWoosh();
        }
    }

    if (bBailOnAll && WooshFadeOut.CurValue < 0.01f) {
        if (m_pWoosh) {
            delete m_pWoosh;
        }
        bBailOnAll = false;
        m_pWoosh = nullptr;
    }
}

void CARSFX_PreColWoosh::ProcessUpdate() {
    SND_Params params;

    if (!m_pWoosh) {
        bBarrierDetected = false;
        return;
    }

    if (!m_pWoosh->IsPlaying()) {
        delete m_pWoosh;
        m_pWoosh = nullptr;
    } else {
        params.Vol = 0x7FFF;
        params.Pitch = 0x1000;
        params.ID = 0;
        params.Az = 0;
        params.Mag = 0;
        params.RVerb = 0;
        params.Vol = static_cast<int>(static_cast<float>(GetDMixOutput(1, DMX_VOL)) * WooshFadeOut.CurValue);
        params.Az = GetDMixOutput(0, DMX_AZIM);
        m_pWoosh->Update(&params);
    }

    bBarrierDetected = false;
}
CARSFX_PreColWoosh::TypeInfo CARSFX_PreColWoosh::s_TypeInfo = {
    0x00020120,
    "CARSFX_PreColWoosh",
    &SndBase::s_TypeInfo,
    CARSFX_PreColWoosh::CreateObject,
};
