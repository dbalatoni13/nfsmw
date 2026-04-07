#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_BottomOut.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/Src/EAXSound/EAXTunerCar.hpp"
#include "Speed/Indep/Src/EAXSound/STICH_Playback.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

float DOT_PROD_FOR_HEAVY_LEAN = 0.65f;
int JumpLandingVolumes[4] = {13000, 15000, 24000, 32767};
Slope JumpLandingIntensity(0.0f, 1.0f, 0.0f, 0.65f);

SndBase::TypeInfo CARSFX_BottomOut::s_TypeInfo = { 0, "CARSFX_BottomOut", nullptr, CARSFX_BottomOut::CreateObject };

SndBase::TypeInfo *CARSFX_BottomOut::GetTypeInfo() const { return &s_TypeInfo; }

const char *CARSFX_BottomOut::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase *CARSFX_BottomOut::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (CARSFX_BottomOut::GetStaticTypeInfo()->typeName, false) CARSFX_BottomOut();
    }
    return new (CARSFX_BottomOut::GetStaticTypeInfo()->typeName, true) CARSFX_BottomOut();
}

CARSFX_BottomOut::CARSFX_BottomOut()
    : CARSFX() {
    int n;

    m_pBottomOut = nullptr;
    for (n = 0; n < 3; n++) {
        m_pStichLandJump[n] = nullptr;
        m_Intesity[n] = 0.0f;
    }
    RearWheelsTouched = true;
    FrontHangTime = 0.0f;
    IsCarLeaningHeavily = false;
    m_pJumpCamCrash = nullptr;
    FrontWheelsTouched = true;
    RearHangTime = 0.0f;
}

CARSFX_BottomOut::~CARSFX_BottomOut() {}

void CARSFX_BottomOut::InitSFX() {
    SndBase::InitSFX();
}

void CARSFX_BottomOut::Destroy() {
    int n;

    for (n = 0; n < 3; n++) {
        if (m_pStichLandJump[n]) {
            delete m_pStichLandJump[n];
        }
        m_pStichLandJump[n] = nullptr;
    }

    if (m_pBottomOut) {
        delete m_pBottomOut;
    }
    m_pBottomOut = nullptr;

    if (m_pJumpCamCrash) {
        delete m_pJumpCamCrash;
    }
    m_pJumpCamCrash = nullptr;
}

void CARSFX_BottomOut::BottomOutPlay(unsigned int Intensity) {
    if (!m_pBottomOut) {
        static int LastRandom = 0;

        float fIntensity;
        int sampleOffset;

        fIntensity = bClamp(static_cast<float>(Intensity), 0.0f, 127.0f);
        sampleOffset = LastRandom % 4;
        LastRandom = sampleOffset + 1;
        sampleOffset += 0x51;
        fIntensity *= 0.03125f;
        SND_Stich &StichData = g_pEAXSound->GetStichPlayer()->GetStich(
            STICH_TYPE_COLLISION, static_cast<int>(fIntensity) * 4 + sampleOffset);
        m_pBottomOut = new cStichWrapper(StichData);
        m_pBottomOut->Play(0, 0, 0);
        SetDMIX_Input(2, 0x7FFF);
    }
}

void CARSFX_BottomOut::LandJumpPlay(float Intensity, bool HardLanding) {
    int Index;
    int n;

    Index = -1;
    for (n = 0; n < 3; n++) {
        if (!m_pStichLandJump[n]) {
            Index = n;
        }
    }

    if (Index != -1) {
        static int LastRandomHard = 0;
        static int LastRandomSoft = 0;

        float fIntensity;
        int sampleOffset;

        fIntensity = bClamp(Intensity, 0.0f, 127.0f);
        if (HardLanding) {
            fIntensity *= 0.0078125f;
            sampleOffset = LastRandomHard % 4;
            LastRandomHard = sampleOffset + 1;
            sampleOffset += 0x5D;
        } else {
            fIntensity *= 0.015625f;
            sampleOffset = LastRandomSoft % 4;
            LastRandomSoft = sampleOffset + 1;
            sampleOffset += 0x51;
        }

        SND_Stich &StichData = g_pEAXSound->GetStichPlayer()->GetStich(
            STICH_TYPE_COLLISION, static_cast<int>(fIntensity) * 4 + sampleOffset);
        m_pStichLandJump[Index] = new cStichWrapper(StichData);
        m_pStichLandJump[Index]->Play(0, 0, 0);
        m_Intesity[Index] = Intensity;
        SetDMIX_Input(1, 0x7FFF);
        SetDMIX_Input(3, static_cast<int>(Intensity) << 8);
    }
}

void CARSFX_BottomOut::Detach() {
    int n;

    for (n = 0; n < 3; n++) {
        if (m_pStichLandJump[n]) {
            delete m_pStichLandJump[n];
        }
        m_pStichLandJump[n] = nullptr;
    }

    if (m_pBottomOut) {
        delete m_pBottomOut;
    }
    m_pBottomOut = nullptr;

    if (m_pJumpCamCrash) {
        delete m_pJumpCamCrash;
    }
    m_pJumpCamCrash = nullptr;
}

void CARSFX_BottomOut::ProcessUpdate() {
    int n;

    SetDMIX_Input(2, 0);
    SetDMIX_Input(1, 0);
    if (m_pBottomOut) {
        SND_Params TmpParams;

        TmpParams.Az = GetDMixOutput(1, DMX_AZIM);
        TmpParams.Vol = GetDMixOutput(1, DMX_VOL);
        TmpParams.Pitch = 0x1000;
        m_pBottomOut->Update(&TmpParams);
        if (!m_pBottomOut->IsPlaying()) {
            delete m_pBottomOut;
            m_pBottomOut = nullptr;
        }
    }

    if (m_pJumpCamCrash) {
        SND_Params TmpParams;

        TmpParams.Az = GetDMixOutput(0, DMX_AZIM);
        TmpParams.Vol = GetDMixOutput(3, DMX_VOL);
        TmpParams.Pitch = 0x1000;
        m_pJumpCamCrash->Update(&TmpParams);
        if (!m_pJumpCamCrash->IsPlaying()) {
            delete m_pJumpCamCrash;
            m_pJumpCamCrash = nullptr;
        }
    }

    for (n = 0; n < 3; n++) {
        if (m_pStichLandJump[n]) {
            SND_Params TmpParams;
            int VolToUse;
            int volume;

            TmpParams.Az = GetDMixOutput(0, DMX_AZIM);
            volume = GetDMixOutput(1, DMX_VOL);
            VolToUse = bClamp(static_cast<int>(m_Intesity[n]) >> 5, 0, 3);
            TmpParams.Pitch = 0x1000;
            TmpParams.Vol = (JumpLandingVolumes[VolToUse] * volume) >> 15;
            m_pStichLandJump[n]->Update(&TmpParams);
            if (!m_pStichLandJump[n]->IsPlaying() || g_EAXIsPaused()) {
                delete m_pStichLandJump[n];
                m_pStichLandJump[n] = nullptr;
            }
        }
    }
}

void CARSFX_BottomOut::UpdateParams(float t) {
    if (!GetPhysCar()) {
        return;
    }

    bool TmpFrontTouched;
    bool TmpBackTouched;
    bool TmpRightTouched;
    bool TmpLeftTouched;
    bool PlayJumpLanding;
    bool IsHardLanding;
    bool FrontTouching;
    bool BackTouching;

    TmpFrontTouched = false;
    TmpBackTouched = false;
    TmpRightTouched = false;
    TmpLeftTouched = false;
    if (GetPhysCar()->IsWheelTouchingGround(0)) {
        if (GetPhysCar()->IsWheelTouchingGround(1)) {
            TmpFrontTouched = true;
        }
    }
    if (GetPhysCar()->IsWheelTouchingGround(1)) {
        if (GetPhysCar()->IsWheelTouchingGround(2)) {
            TmpRightTouched = true;
        }
    }
    if (GetPhysCar()->IsWheelTouchingGround(2)) {
        if (GetPhysCar()->IsWheelTouchingGround(3)) {
            TmpBackTouched = true;
        }
    }
    if (GetPhysCar()->IsWheelTouchingGround(3)) {
        if (GetPhysCar()->IsWheelTouchingGround(0)) {
            TmpLeftTouched = true;
        }
    }

    if (GetPhysCar()->IsLocalPlayerCar()) {
        EAXTunerCar *pTunerCar = static_cast<EAXTunerCar *>(m_pEAXCar);
        if (pTunerCar->BottomOutPlay) {
            pTunerCar->BottomOutPlay = false;
            BottomOutPlay(pTunerCar->BottomOutIntensity);
        }
    }

    if (!IsCarLeaningHeavily) {
        float DotProd = bDot(*GetPhysCar()->GetUpVector(), bVector3(0.0f, 0.0f, 1.0f));
        if (DotProd < DOT_PROD_FOR_HEAVY_LEAN) {
            if (static_cast<float>(GetPhysCar()->GetWheelsOnGround()) < 1.0f) {
                IsCarLeaningHeavily = true;
            }
        }
    }

    PlayJumpLanding = false;
    IsHardLanding = false;
    FrontTouching = false;
    if (TmpFrontTouched && (FrontHangTime > 0.05f) && !FrontWheelsTouched) {
        FrontTouching = true;
    }
    BackTouching = false;
    if (TmpBackTouched && (RearHangTime > 0.05f) && !RearWheelsTouched) {
        BackTouching = true;
    }

    if (!FrontTouching) {
        if (BackTouching && !IsCarLeaningHeavily) {
            PlayJumpLanding = true;
            if (RearHangTime > 0.1f) {
                IsHardLanding = true;
            }
        }
    } else {
        PlayJumpLanding = true;
        if ((FrontHangTime > 0.1f) || (BackTouching && (RearHangTime > 0.1f))) {
            IsHardLanding = true;
        }
    }

    if (IsCarLeaningHeavily && ((TmpFrontTouched && !FrontWheelsTouched) || (TmpBackTouched && !RearWheelsTouched) ||
                                (TmpRightTouched && !RightWheelsTouched) || (TmpLeftTouched && !LeftWheelsTouched))) {
            PlayJumpLanding = true;
            IsHardLanding = true;
    }

    if (PlayJumpLanding) {
        float LandingIntensity;

        if (GetPhysCar()->IsLocalPlayerCar()) {
            LandingIntensity = 0.0f;
            for (int i = 0; i < 4; i++) {
                float ZForce;

                if (!FrontTouching) {
                    ZForce = GetPhysCar()->GetWheelZForce(2) + GetPhysCar()->GetWheelZForce(3);
                } else {
                    ZForce = GetPhysCar()->GetWheelZForce(0) + GetPhysCar()->GetWheelZForce(1);
                }
                LandingIntensity += JumpLandingIntensity.GetValue(ZForce);
            }
            LandingIntensity *= 31.75f;
        } else {
            LandingIntensity = 127.0f;
        }
        LandJumpPlay(LandingIntensity, IsHardLanding);
    }

    if ((static_cast<float>(GetPhysCar()->GetWheelsOnGround()) == 1.0f) && IsCarLeaningHeavily) {
        IsCarLeaningHeavily = false;
    }

    if (!GetPhysCar()->IsWheelTouchingGround(0) || !GetPhysCar()->IsWheelTouchingGround(1)) {
        FrontWheelsTouched = false;
        FrontHangTime += t;
    } else {
        FrontWheelsTouched = true;
        FrontHangTime = 0.0f;
    }

    if (!GetPhysCar()->IsWheelTouchingGround(2) || !GetPhysCar()->IsWheelTouchingGround(3)) {
        RearWheelsTouched = false;
        RearHangTime += t;
    } else {
        RearWheelsTouched = true;
        RearHangTime = 0.0f;
    }

    if (!GetPhysCar()->IsWheelTouchingGround(1) || !GetPhysCar()->IsWheelTouchingGround(2)) {
        RightWheelsTouched = false;
        RightHangTime += t;
    } else {
        RightWheelsTouched = true;
        RightHangTime = 0.0f;
    }

    if (!GetPhysCar()->IsWheelTouchingGround(0) || !GetPhysCar()->IsWheelTouchingGround(3)) {
        LeftWheelsTouched = false;
        LeftHangTime += t;
    } else {
        LeftWheelsTouched = true;
        LeftHangTime = 0.0f;
    }
}
