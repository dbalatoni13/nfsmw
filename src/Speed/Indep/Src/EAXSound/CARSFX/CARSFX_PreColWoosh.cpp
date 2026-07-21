#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_PreColWoosh.hpp"
#include "Speed/Indep/Src/EAXSound/SndBase.hpp"
#include "Speed/Indep/Src/EAXSound/SND_GEN/STITCH_WSH.h"

DEFINE_CREATABLE(0x20120, CARSFX_PreColWoosh, SndBase);

// UNSOLVED, stack issues (because the second UCRC32 reuses sp8), but functionally equivalent
CARSFX_PreColWoosh::CARSFX_PreColWoosh() {
    this->mMsgBarrier = Hermes::Handler::Create<MAudioReflection, CARSFX_PreColWoosh, CARSFX_PreColWoosh>(this, &CARSFX_PreColWoosh::MsgBarrier,
                                                                                                          UCrc32("FRONT_BARRIER"), 0);
    this->mMsgBarrierHit = Hermes::Handler::Create<MAudioReflection, CARSFX_PreColWoosh, CARSFX_PreColWoosh>(this, &CARSFX_PreColWoosh::MsgBarrierHit,
                                                                                                             UCrc32("FRONT_BARRIER_HIT"), 0);
    this->m_pWoosh = nullptr;
    this->mResetTime = 0.0f;
    this->mDurationActive = 0.0f;
}

CARSFX_PreColWoosh::~CARSFX_PreColWoosh() {
    if (this->mMsgBarrier != nullptr) {
        Hermes::Handler::Destroy(this->mMsgBarrier);
    }

    if (this->mMsgBarrierHit != nullptr) {
        Hermes::Handler::Destroy(this->mMsgBarrierHit);
    }

    if (this->m_pWoosh != nullptr) {
        delete this->m_pWoosh;
    }

    this->m_pWoosh = nullptr;
}

void CARSFX_PreColWoosh::InitSFX() {
    SndBase::InitSFX();
    this->bGoingToCollide = false;
    this->bBailOnAll = false;
    this->bBarrierDetected = false;
    this->WooshFadeOut.Initialize(1.0f, 1.0f, 1, LINEAR);
}

void CARSFX_PreColWoosh::Destroy() {
    SndBase::Destroy();
}

// UNSOLVED, instruction swap
void CARSFX_PreColWoosh::MsgBarrier(const MAudioReflection &message) {
    if (message.GetPlayerNum() != this->GetStateBase()->m_InstNum) {
        return;
    }

    this->bBarrierDetected = true;
    if (!this->bGoingToCollide && !this->bBailOnAll && this->mResetTime == 0.0f) {
        this->bGoingToCollide = true;
        this->mDurationActive = 0.0f;
        this->WooshFadeOut.Initialize(1.0f, 1.0f, 1, LINEAR);
    }
}

void CARSFX_PreColWoosh::MsgBarrierHit(const MAudioReflection &message) {
    if ((this->GetPhysCar() != nullptr) && message.GetPlayerNum() == static_cast<int>(this->GetPhysCar()->mWorldID)) {
        this->BailOnWoosh();
    }
}

void CARSFX_PreColWoosh::BailOnWoosh() {
    this->bBailOnAll = true;
    this->bGoingToCollide = false;
    this->WooshFadeOut.Initialize(this->WooshFadeOut.GetValue(), 0.0f, 0x82, LINEAR);
    this->mResetTime = 0.4f;
}

void CARSFX_PreColWoosh::Detach() {
    SndBase::Detach();
}

void CARSFX_PreColWoosh::UpdateParams(float t) {
    SndBase::UpdateParams(t);
    this->WooshFadeOut.Update(t);
    this->mResetTime -= t;
    if (this->mResetTime < 0.0f) {
        this->mResetTime = 0.0f;
    }

    this->SetDMIX_Input(2, 0);
    if (this->bGoingToCollide) {
        this->SetDMIX_Input(2, 0x7FFF);
        this->mDurationActive += t;
        if (this->m_pWoosh == nullptr) {
            extern Slope g_WooshVol_vs_Vel;
            float fVelRatio = bClamp(g_WooshVol_vs_Vel.GetValue(this->GetPhysCar()->GetVelocityMagnitude()), 0.0f, 0.99f);
            float fVelInensity = bClamp(fVelRatio * 127.0f, 0.0f, 127.0f);
            int StitchID;

            void GetWooshBlockSizeParams(eDRIVE_BY_TYPE type, enum STICH_WHOOSH_TYPE & base, int &numblocks, int &sizeperblock);
            int numblocks;

            int sizeperblock;
            STICH_WHOOSH_TYPE base;
            GetWooshBlockSizeParams(DRIVE_BY_PRE_COL, base, numblocks, sizeperblock);
            {
                static int LastRandom = 0;

                StitchID = LastRandom - (LastRandom / sizeperblock) * sizeperblock;
                LastRandom = StitchID + 1;
            }

            SND_Stich *stitchdata = &g_pEAXSound->GetStichPlayer()->GetStich(
                STICH_TYPE_WOOSH, base + static_cast<int>(fVelInensity / 128.0f * static_cast<float>(numblocks)) * sizeperblock + StitchID);

            SND_Params sndparams;
            sndparams.ID = 0;
            sndparams.Mag = 0;
            sndparams.RVerb = 0;
            sndparams.Az = 0;
            sndparams.Pitch = 0;
            sndparams.Vol = 0;
            this->m_pWoosh = new cStichWrapper(*stitchdata);
            this->m_pWoosh->Play(&sndparams);
        }

        if (!this->bBarrierDetected || this->mDurationActive > 0.4f) {
            this->BailOnWoosh();
        }
    }

    if (this->bBailOnAll && this->WooshFadeOut.GetValue() < 0.01f) {
        delete this->m_pWoosh;
        this->m_pWoosh = nullptr;
        this->bBailOnAll = false;
    }

    this->WooshFadeOut.GetValue();
}

void CARSFX_PreColWoosh::ProcessUpdate() {
    SndBase::ProcessUpdate();
    if (this->m_pWoosh != nullptr) {
        if (!this->m_pWoosh->IsPlaying()) {
            delete this->m_pWoosh;
            this->m_pWoosh = nullptr;
        } else {
            SND_Params params;

            params.ID = 0;
            params.Az = 0;
            params.Mag = 0;
            params.RVerb = 0;
            params.Pitch = 0x1000;
            params.Vol = static_cast<int>(static_cast<float>(this->GetDMixOutput(1, DMX_VOL)) * this->WooshFadeOut.GetValue());
            params.Az = this->GetDMixOutput(0, DMX_AZIM);
            this->m_pWoosh->Update(&params);
        }
    }

    this->bBarrierDetected = false;
}
