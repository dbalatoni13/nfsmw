#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_BottomOut.hpp"
#include "Speed/Indep/Src/EAXSound/EAXTunerCar.hpp"

float DOT_PROD_FOR_HEAVY_LEAN = 0.8f;                         // size: 0x4, address: 0x804181D4, Decl: 4
static const float MIN_HANGTIME_FOR_LANDJUMP_SND = 0.12f;     // size: 0x4, Decl: 5
static const float MIN_HANGTIME_FOR_HARD_LANDJUMP_SND = 0.7f; // size: 0x4, Decl: 6

Slope JumpLandingIntensity(0.0f, 1.0f, 0.0f, 0.65f); // size: 0x1C, address: 0x8045E548, Decl: 10

int JumpLandingVolumes[4] = {13000, 15000, 24000, 0x7FFF}; // size: 0x10, address: 0x804181D8, Decl: 16

DEFINE_CREATABLE(0x200d0, CARSFX_BottomOut, SndBase);

CARSFX_BottomOut::CARSFX_BottomOut() : CARSFX() {
    this->m_pBottomOut = nullptr;
    for (int n = 0; n < NUM_ELEMENTS(this->m_pStichLandJump); n++) {
        this->m_pStichLandJump[n] = nullptr;
        this->m_Intesity[n] = 0.0f;
    }
    this->m_pJumpCamCrash = nullptr;
    this->FrontWheelsTouched = true;
    this->RearWheelsTouched = true;
    this->RearHangTime = 0.0f;
    this->FrontHangTime = 0.0f;
    this->IsCarLeaningHeavily = false;
}

CARSFX_BottomOut::~CARSFX_BottomOut() {
    this->Destroy();
}

void CARSFX_BottomOut::InitSFX() {
    SndBase::InitSFX();
}

void CARSFX_BottomOut::Destroy() {
    SndBase::Destroy();

    for (int n = 0; n < NUM_ELEMENTS(this->m_pStichLandJump); n++) {
        delete this->m_pStichLandJump[n];
        this->m_pStichLandJump[n] = nullptr;
    }

    delete this->m_pBottomOut;
    this->m_pBottomOut = nullptr;

    delete this->m_pJumpCamCrash;
    this->m_pJumpCamCrash = nullptr;
}

void CARSFX_BottomOut::LandJumpPlay(float Intensity, bool HardLanding) {
    int Index = -1;

    for (int n = 0; n < 3; n++) {
        if (this->m_pStichLandJump[n] == nullptr) {
            Index = n;
        }
    }

    if (Index == -1) {
        return;
    }

    float fIntensity = bClamp(Intensity, 0.0f, 127.0f);
    int sampleOffset;
    if (HardLanding) {
        GEN_RND_OFFSET(sampleOffset, fIntensity, 93, 1, 4);
    } else {
        GEN_RND_OFFSET(sampleOffset, fIntensity, 81, 4, 4);
    }

    SND_Stich &StichData = g_pEAXSound->GetStichPlayer()->GetStich(STICH_TYPE_COLLISION, sampleOffset);
    this->m_pStichLandJump[Index] = new cStichWrapper(StichData);
    this->m_pStichLandJump[Index]->Play(0, 0, 0);
    this->m_Intesity[Index] = Intensity;
    this->SetDMIX_Input(1, 0x7FFF);
    this->SetDMIX_Input(3, static_cast<int>(Intensity) << 8);
}

void CARSFX_BottomOut::BottomOutPlay(unsigned int Intensity) {
    if (this->m_pBottomOut != nullptr) {
        return;
    }

    float fIntensity = bClamp(Intensity, 0.0f, 127.0f);
    int sampleOffset;
    GEN_RND_OFFSET(sampleOffset, fIntensity, 81, 4, 4);

    SND_Stich &StichData = g_pEAXSound->GetStichPlayer()->GetStich(STICH_TYPE_COLLISION, sampleOffset);
    this->m_pBottomOut = new cStichWrapper(StichData);
    this->m_pBottomOut->Play(0, 0, 0);
    this->SetDMIX_Input(2, 0x7FFF);
}

void CARSFX_BottomOut::Detach() {
    for (int n = 0; n < NUM_ELEMENTS(this->m_pStichLandJump); n++) {
        delete this->m_pStichLandJump[n];
        this->m_pStichLandJump[n] = nullptr;
    }

    delete this->m_pBottomOut;
    this->m_pBottomOut = nullptr;

    delete this->m_pJumpCamCrash;
    this->m_pJumpCamCrash = nullptr;
}

void CARSFX_BottomOut::UpdateParams(float t) {
    this->SndBase::UpdateParams(t);
    if (this->GetPhysCar() == nullptr) {
        return;
    }

    bool TmpFrontTouched;
    bool TmpBackTouched;
    bool TmpRightTouched;
    bool TmpLeftTouched;

    TmpLeftTouched = false;
    TmpRightTouched = false;
    TmpBackTouched = false;
    TmpFrontTouched = false;

    // TODO is this right?
    if (this->GetPhysCar()->IsWheelTouchingGround(0) && this->GetPhysCar()->IsWheelTouchingGround(1)) {
        TmpFrontTouched = true;
    }

    if (this->GetPhysCar()->IsWheelTouchingGround(1) && this->GetPhysCar()->IsWheelTouchingGround(2)) {
        TmpRightTouched = true;
    }

    if (this->GetPhysCar()->IsWheelTouchingGround(2) && this->GetPhysCar()->IsWheelTouchingGround(3)) {
        TmpBackTouched = true;
    }

    if (this->GetPhysCar()->IsWheelTouchingGround(3) && this->GetPhysCar()->IsWheelTouchingGround(0)) {
        TmpLeftTouched = true;
    }

    if ((this->GetPhysCar() != nullptr) && this->GetPhysCar()->IsLocalPlayerCar()) {
        EAXTunerCar *pTunerCar = static_cast<EAXTunerCar *>(this->m_pEAXCar);

        if (pTunerCar->BottomOutPlay) {
            pTunerCar->BottomOutPlay = false;
            this->BottomOutPlay(pTunerCar->BottomOutIntensity);
        }
    }

    if (!this->IsCarLeaningHeavily) {
        float DotProd = bDot(*this->GetPhysCar()->GetUpVector(), bVector3(0.0f, 0.0f, 1.0f));

        if (DotProd < DOT_PROD_FOR_HEAVY_LEAN) {
            if (static_cast<float>(this->GetPhysCar()->GetWheelsOnGround()) < 1.0f) {
                this->IsCarLeaningHeavily = true;
            }
        }
    }

    bool PlayJumpLanding = false;
    bool IsHardLanding = false;
    bool FrontTouching = false;
    bool BackTouching = false;

    FrontTouching = this->GetPhysCar()->IsWheelTouchingGround(0) && this->GetPhysCar()->IsWheelTouchingGround(1) &&
                    (this->FrontHangTime > MIN_HANGTIME_FOR_LANDJUMP_SND) && !this->FrontWheelsTouched;

    BackTouching = this->GetPhysCar()->IsWheelTouchingGround(2) && this->GetPhysCar()->IsWheelTouchingGround(3) &&
                   (this->RearHangTime > MIN_HANGTIME_FOR_LANDJUMP_SND) && !this->RearWheelsTouched;

    if (FrontTouching || (BackTouching && !this->IsCarLeaningHeavily)) {
        PlayJumpLanding = true;
        if ((FrontTouching && (this->FrontHangTime > MIN_HANGTIME_FOR_HARD_LANDJUMP_SND)) ||
            (BackTouching && (this->RearHangTime > MIN_HANGTIME_FOR_HARD_LANDJUMP_SND))) {
            IsHardLanding = true;
        } else {
            IsHardLanding = false;
        }
    }

    if (this->IsCarLeaningHeavily && ((TmpFrontTouched && !this->FrontWheelsTouched) || (TmpBackTouched && !this->RearWheelsTouched) ||
                                      (TmpRightTouched && !this->RightWheelsTouched) || (TmpLeftTouched && !this->LeftWheelsTouched))) {
        PlayJumpLanding = true;
        IsHardLanding = true;
    }

    if (PlayJumpLanding) {
        float LandingIntensity = 0.0f;

        if (this->GetPhysCar()->IsLocalPlayerCar()) {
            for (int i = 0; i < 4; i++) {
                float ZForce;

                if (FrontTouching) {
                    ZForce = this->GetPhysCar()->GetWheelZForce(0) + this->GetPhysCar()->GetWheelZForce(1);
                } else {
                    ZForce = this->GetPhysCar()->GetWheelZForce(2) + this->GetPhysCar()->GetWheelZForce(3);
                }

                LandingIntensity += JumpLandingIntensity.GetValue(ZForce);
            }

            LandingIntensity *= 63.5f;
            this->LandJumpPlay(LandingIntensity, IsHardLanding);
        } else {
            LandingIntensity = 127.0f;
            this->LandJumpPlay(LandingIntensity, IsHardLanding);
        }
    }

    if ((static_cast<float>(this->GetPhysCar()->GetWheelsOnGround()) == 1.0f) && this->IsCarLeaningHeavily) {
        this->IsCarLeaningHeavily = false;
    }

    if (this->GetPhysCar()->IsWheelTouchingGround(0) && this->GetPhysCar()->IsWheelTouchingGround(1)) {
        this->FrontWheelsTouched = true;
        this->FrontHangTime = 0.0f;
    } else {
        this->FrontWheelsTouched = false;
        this->FrontHangTime += t;
    }

    if (this->GetPhysCar()->IsWheelTouchingGround(2) && this->GetPhysCar()->IsWheelTouchingGround(3)) {
        this->RearWheelsTouched = true;
        this->RearHangTime = 0.0f;
    } else {
        this->RearWheelsTouched = false;
        this->RearHangTime += t;
    }

    if (this->GetPhysCar()->IsWheelTouchingGround(1) && this->GetPhysCar()->IsWheelTouchingGround(2)) {
        this->RightWheelsTouched = true;
        this->RightHangTime = 0.0f;
    } else {
        this->RightWheelsTouched = false;
        this->RightHangTime += t;
    }

    if (this->GetPhysCar()->IsWheelTouchingGround(0) && this->GetPhysCar()->IsWheelTouchingGround(3)) {
        this->LeftWheelsTouched = true;
        this->LeftHangTime = 0.0f;
    } else {
        this->LeftWheelsTouched = false;
        this->LeftHangTime += t;
    }
}

void CARSFX_BottomOut::ProcessUpdate() {
    this->SndBase::ProcessUpdate();
    this->SetDMIX_Input(2, 0);
    this->SetDMIX_Input(1, 0);

    if (this->m_pBottomOut != nullptr) {
        SND_Params TmpParams;
        TmpParams.ID = 0;
        TmpParams.Az = 0;
        TmpParams.Mag = 0;
        TmpParams.RVerb = 0;
        TmpParams.Az = this->GetDMixOutput(1, DMX_AZIM);
        TmpParams.Vol = this->GetDMixOutput(1, DMX_VOL);
        TmpParams.Pitch = 0x1000;
        this->m_pBottomOut->Update(&TmpParams);

        if (!this->m_pBottomOut->IsPlaying()) {
            delete this->m_pBottomOut;
            this->m_pBottomOut = nullptr;
        }
    }

    if (this->m_pJumpCamCrash != nullptr) {
        SND_Params TmpParams;
        TmpParams.ID = 0;
        TmpParams.Az = 0;
        TmpParams.Mag = 0;
        TmpParams.RVerb = 0;
        TmpParams.Az = this->GetDMixOutput(0, DMX_AZIM);
        TmpParams.Vol = this->GetDMixOutput(3, DMX_VOL);
        TmpParams.Pitch = 0x1000;
        this->m_pJumpCamCrash->Update(&TmpParams);

        if (!this->m_pJumpCamCrash->IsPlaying()) {
            delete this->m_pJumpCamCrash;
            this->m_pJumpCamCrash = nullptr;
        }
    }

    for (int n = 0; n < NUM_ELEMENTS(this->m_pStichLandJump); n++) {
        if (this->m_pStichLandJump[n] != nullptr) {
            SND_Params TmpParams;
            TmpParams.ID = 0;
            TmpParams.Az = 0;
            TmpParams.Mag = 0;
            TmpParams.RVerb = 0;
            TmpParams.Az = this->GetDMixOutput(0, DMX_AZIM);
            TmpParams.Vol = this->GetDMixOutput(1, DMX_VOL);

            int VolToUse = bClamp(static_cast<int>(this->m_Intesity[n]) >> 5, 0, 3);

            TmpParams.Pitch = 0x1000;
            TmpParams.Vol = (JumpLandingVolumes[VolToUse] * TmpParams.Vol) >> 15;
            this->m_pStichLandJump[n]->Update(&TmpParams);

            if (!this->m_pStichLandJump[n]->IsPlaying() || g_EAXIsPaused()) {
                delete this->m_pStichLandJump[n];
                this->m_pStichLandJump[n] = nullptr;
            }
        }
    }
}
