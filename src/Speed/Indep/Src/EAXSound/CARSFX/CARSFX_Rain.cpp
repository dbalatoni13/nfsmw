#include "./CARSFX_Rain.hpp"

#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/EAXS_StreamManager.h"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Generated/Messages/MGamePlayMoment.h"

namespace Csis {
InterfaceId FX_WeatherId = {"FX_Weather", 0x3EE6, 0x5390};
ClassHandle gFX_WeatherHandle;
} // namespace Csis

CARSFX_Rain::TypeInfo CARSFX_Rain::s_TypeInfo = {
    0x000200B0,
    "CARSFX_Rain",
    &SndBase::s_TypeInfo,
    CARSFX_Rain::CreateObject,
};

CARSFX_Rain::TypeInfo *CARSFX_Rain::GetTypeInfo() const {
    return &s_TypeInfo;
}

const char *CARSFX_Rain::GetTypeName() const {
    return s_TypeInfo.typeName;
}

SndBase *CARSFX_Rain::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (GetStaticTypeInfo()->typeName, false) CARSFX_Rain();
    }
    return new (GetStaticTypeInfo()->typeName, true) CARSFX_Rain();
}

CARSFX_Rain::CARSFX_Rain()
    : CARSFX() {
    m_pCsisRain = nullptr;
    *reinterpret_cast<int *>(&bFadingOut) = 0;
    m_BlockStrmTest = 0;
    m_fThunderTime = 0.0f;
    m_fThunderDeltaTime = 0.0f;
    m_pTunnelCtl = nullptr;
}

CARSFX_Rain::~CARSFX_Rain() {
    Destroy();
}

int CARSFX_Rain::GetController(int Index) {
    return Index != 0 ? -1 : 6;
}

void CARSFX_Rain::AttachController(SFXCTL *psfxctl) {
    if (psfxctl->GetObjectIndex() == 6) {
        m_pTunnelCtl = static_cast<SFXCTL_Tunnel *>(psfxctl);
    }
}

void CARSFX_Rain::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
}

void CARSFX_Rain::InitSFX() {
    SndBase::InitSFX();
}

void CARSFX_Rain::Play() {
    if (m_pCsisRain) {
        Destroy();
        *reinterpret_cast<int *>(&bFadingOut) = 0;
    } else {
        g_pEAXSound->SetCsisName(this);
        m_pCsisRain = new FX_Weather(0, 0, 0, 0, 0, 0, 25000, 0, 0x7FFF, 0);
        *reinterpret_cast<int *>(&bFadingOut) = 0;
    }
}

void CARSFX_Rain::Stop() {
    if (m_pCsisRain) {
        *reinterpret_cast<int *>(&bFadingOut) = 1;
        m_fTimeLeftToFadeOut = 10.0f;
        m_pCsisRain->SetRain_on_off(1);
        m_pCsisRain->CommitMemberData();
    }
}

void CARSFX_Rain::Destroy() {
    if (m_pCsisRain) {
        delete m_pCsisRain;
    }
    m_pCsisRain = nullptr;
    *reinterpret_cast<int *>(&bFadingOut) = 0;
}

void CARSFX_Rain::QueueWeatherStream() {
    int donePlaying;
    int nweathertype = 0;

    if (*reinterpret_cast<int *>(&m_bWeatherStreamQueued) != 1) {
        donePlaying = Speech::Manager::GetSpeechModule(0)->DonePlaying();
        if (donePlaying == 1) {
            if (m_fWeatherIntensity < 0.5f) {
                nweathertype = 0x016C4FA1;
                MGamePlayMoment(UMath::Vector4::kZero, UMath::Vector4::kZero, UMath::Vector4::kZero, 0, nweathertype)
                    .Send(UCrc32("MomentStrm"));
            } else {
                nweathertype = 0xD2C5E045;
                MGamePlayMoment(UMath::Vector4::kZero, UMath::Vector4::kZero, UMath::Vector4::kZero, 0, nweathertype)
                    .Send(UCrc32("MomentStrm"));
            }
            *reinterpret_cast<int *>(&m_bWeatherStreamQueued) = donePlaying;
        }
    }
}

void CARSFX_Rain::UpdateParams(float t) {
    eView *view;

    SndBase::UpdateParams(t);
    view = eGetView(1, false);
    if (view) {
        m_fPrevWeatherIntensity = m_fWeatherIntensity;
        m_fWeatherIntensity = eViews[1].Precipitation ? view->Precipitation->GetRainIntensity() : 0.0f;
    } else {
        m_fPrevWeatherIntensity = 0.0f;
        m_fWeatherIntensity = 0.0f;
    }

    if (0.01f < m_fWeatherIntensity) {
        if (!m_pCsisRain) {
            Play();
        }
    } else if (m_pCsisRain && *reinterpret_cast<int *>(&bFadingOut) == 0) {
        Stop();
    }

    m_fThunderTime = m_fThunderTime + t;
    if (0.25f < m_fWeatherIntensity && !IsWorldDataStreaming(0) && m_fThunderTime > m_fThunderDeltaTime) {
        if (m_BlockStrmTest == 0) {
            m_fThunderTime = 0.0f;
            if (*reinterpret_cast<int *>(&m_bWeatherStreamQueued) == 1) {
                *reinterpret_cast<int *>(&m_bWeatherStreamQueued) = 0;
            }
            QueueWeatherStream();
            m_fThunderDeltaTime = static_cast<float>(g_pEAXSound->Random(0x14)) + 25.0f;
            m_BlockStrmTest = m_BlockStrmTest + 1;
        } else {
            m_BlockStrmTest = m_BlockStrmTest + 1;
            if (m_BlockStrmTest > 0x20) {
                m_BlockStrmTest = 0;
            }
        }
    }

    if (*reinterpret_cast<int *>(&bFadingOut) != 0) {
        if (1) {
            float fdt;

            fdt = m_fTimeLeftToFadeOut - t;
            m_fTimeLeftToFadeOut = fdt;
            if (fdt < 0.0f) {
                Destroy();
            }
        }
    }
}

void CARSFX_Rain::UpdateMixerOutputs() {
    if (m_pCsisRain && *reinterpret_cast<int *>(&bFadingOut) == 0) {
        SetDMIX_Input(2, 0x7FFF);
    } else {
        SetDMIX_Input(2, 0);
    }
}

void CARSFX_Rain::ProcessUpdate() {
    if (m_pCsisRain) {
        int RefCnt;
        int Tmp;
        bool bInsideRain;

        RefCnt = m_pCsisRain->GetRefCount();
        bInsideRain = false;
        if (m_pEAXCar->IsHoodCameraOn()) {
            bInsideRain = true;
        } else if (m_pEAXCar->IsBumperCameraOn()) {
            bInsideRain = true;
        }

        if (bInsideRain) {
            Tmp = GetDMixOutput(1, DMX_VOL);
        } else {
            Tmp = GetDMixOutput(0, DMX_VOL);
        }

        m_pCsisRain->SetVolume(Tmp);
        m_pCsisRain->SetPitch_Offset(0);
        m_pCsisRain->SetHood_Rain(bInsideRain ? 1 : 0);
        m_pCsisRain->SetHood_Rain_Vol_Substract(0);
        m_pCsisRain->SetWidth(7000);
        m_pCsisRain->CommitMemberData();
    }
}
