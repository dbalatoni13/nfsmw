#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_Rain.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Generated/Messages/MGamePlayMoment.h"
#include "Speed/Indep/Src/World/Rain.hpp"

DEFINE_CREATABLE(0x200B0, CARSFX_Rain, SndBase);

CARSFX_Rain::CARSFX_Rain() : CARSFX() {
    this->m_pCsisRain = nullptr;
    this->bFadingOut = false;
    this->m_BlockStrmTest = 0;
    this->m_fThunderTime = 0.0f;
    this->m_fThunderDeltaTime = 0.0f;
    this->m_pTunnelCtl = nullptr;
}

CARSFX_Rain::~CARSFX_Rain() {
    this->Destroy();
}

int CARSFX_Rain::GetController(int Index) {
    return Index != 0 ? -1 : 6;
}

void CARSFX_Rain::AttachController(SFXCTL *psfxctl) {
    if (psfxctl->GetObjectIndex() == 6) {
        this->m_pTunnelCtl = static_cast<SFXCTL_Tunnel *>(psfxctl);
    }
}

void CARSFX_Rain::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
}

void CARSFX_Rain::InitSFX() {
    SndBase::InitSFX();
}

void CARSFX_Rain::Play() {
    if (this->m_pCsisRain != nullptr) {
        this->Destroy();
        bFadingOut = false;
    } else {
        g_pEAXSound->SetCsisName(this);
        this->m_pCsisRain = new Csis::FX_Weather(0, 0, 0, 0, 0, 0, 25000, 0, 0x7FFF, 0);
        this->bFadingOut = false;
    }
}

void CARSFX_Rain::Stop() {
    if (this->m_pCsisRain != nullptr) {
        this->bFadingOut = true;
        this->m_fTimeLeftToFadeOut = 10.0f;
        this->m_pCsisRain->SetRain_on_off(1);
        this->m_pCsisRain->CommitMemberData();
    }
}

void CARSFX_Rain::Destroy() {
    delete this->m_pCsisRain;
    this->m_pCsisRain = nullptr;
    this->bFadingOut = false;
}

void CARSFX_Rain::QueueWeatherStream() {
    if (this->m_bWeatherStreamQueued == true) {
        return;
    }

    int nweathertype = 0;
    if (Speech::Manager::GetSpeechModule(0)->DonePlaying() == true) {
        if (this->m_fWeatherIntensity < 0.5f) {
            nweathertype = 1;
            // TODO aud_moment_strm::key_thunder_distant
            MGamePlayMoment(UMath::Vector4::kZero, UMath::Vector4::kZero, UMath::Vector4::kZero, 0, 0x016C4FA1).Send(UCrc32("MomentStrm"));
            this->m_bWeatherStreamQueued = true;
        } else {
            // aud_moment_strm::key_thunder_close
            MGamePlayMoment(UMath::Vector4::kZero, UMath::Vector4::kZero, UMath::Vector4::kZero, 0, 0xD2C5E045).Send(UCrc32("MomentStrm"));
            this->m_bWeatherStreamQueued = true;
        }
    }
}

void CARSFX_Rain::UpdateParams(float t) {
    SndBase::UpdateParams(t);
    eView *view = eGetView(1, false);
    if (view != nullptr) {
        this->m_fPrevWeatherIntensity = this->m_fWeatherIntensity;
        this->m_fWeatherIntensity = (view->Precipitation != nullptr) ? view->Precipitation->GetRainIntensity() : 0.0f;
    } else {
        this->m_fPrevWeatherIntensity = 0.0f;
        this->m_fWeatherIntensity = 0.0f;
    }

    if (0.01f < this->m_fWeatherIntensity) {
        if (this->m_pCsisRain == nullptr) {
            this->Play();
        }
    } else if ((this->m_pCsisRain != nullptr) && !this->bFadingOut) {
        this->Stop();
    }

    bool IsWorldDataStreaming(unsigned int strmhandle);

    this->m_fThunderTime += t;
    if (0.25f < this->m_fWeatherIntensity && !IsWorldDataStreaming(0) && this->m_fThunderTime > this->m_fThunderDeltaTime) {
        if (this->m_BlockStrmTest == 0) {
            this->m_fThunderTime = 0.0f;
            if (this->m_bWeatherStreamQueued == true) {
                this->m_bWeatherStreamQueued = false;
            }
            this->QueueWeatherStream();
            this->m_fThunderDeltaTime = static_cast<float>(g_pEAXSound->Random(20)) + 25.0f;
            this->m_BlockStrmTest++;
        } else {
            this->m_BlockStrmTest++;
            if (this->m_BlockStrmTest > 32) {
                this->m_BlockStrmTest = 0;
            }
        }
    }

    if (this->bFadingOut) {
        if (1) {
            float fdt = this->m_fTimeLeftToFadeOut - t;
            this->m_fTimeLeftToFadeOut = fdt;
            if (fdt < 0.0f) {
                this->Destroy();
            }
        }
    }
}

void CARSFX_Rain::UpdateMixerOutputs() {
    if ((this->m_pCsisRain != nullptr) && !this->bFadingOut) {
        this->SetDMIX_Input(2, 0x7FFF);
    } else {
        this->SetDMIX_Input(2, 0);
    }
}

void CARSFX_Rain::ProcessUpdate() {
    if (this->m_pCsisRain != nullptr) {
        int RefCnt = this->m_pCsisRain->GetRefCount();
        int Tmp;
        bool bInsideRain = false;
        if (this->m_pEAXCar->IsHoodCameraOn()) {
            bInsideRain = true;
        } else if (this->m_pEAXCar->IsBumperCameraOn()) {
            bInsideRain = true;
        }

        if (bInsideRain) {
            Tmp = this->GetDMixOutput(1, DMX_VOL);
        } else {
            Tmp = this->GetDMixOutput(0, DMX_VOL);
        }

        this->m_pCsisRain->SetVolume(Tmp);
        this->m_pCsisRain->SetPitch_Offset(0);
        this->m_pCsisRain->SetHood_Rain(bInsideRain ? 1 : 0);
        this->m_pCsisRain->SetHood_Rain_Vol_Substract(0);
        this->m_pCsisRain->SetWidth(7000);
        this->m_pCsisRain->CommitMemberData();
    }
}
