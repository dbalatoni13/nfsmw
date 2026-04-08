#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_WindNoise.hpp"

#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"

namespace Csis {
InterfaceId FX_WIND_WeatherId = {"FX_WIND_Weather", 0x1981, 0x31ED};
ClassHandle gFX_WIND_WeatherHandle;
} // namespace Csis

Slope WeatherWindVolSlope(1.0f, 0.25f, 0.0f, 70.0f);

struct FX_WIND_WeatherStruct {
    int volume;         // offset 0x0, size 0x4
    int pitch_offset;   // offset 0x4, size 0x4
    int width;          // offset 0x8, size 0x4
    int intensity;      // offset 0xC, size 0x4
};

struct FX_WIND_Weather {
    static void *operator new(unsigned int size) {
        return Csis::System::Alloc(size);
    }

    static void operator delete(void *ptr) {
        Csis::System::Free(ptr);
    }

    FX_WIND_Weather(int volume, int pitch_offset, int width, int intensity)
        : mpClass(nullptr) {
        Csis::System::Result result;

        SetVolume(volume);
        SetPitch_offset(pitch_offset);
        SetWidth(width);
        SetIntensity(intensity);

        result = static_cast<Csis::System::Result>(
            Csis::Class::CreateInstance(&Csis::gFX_WIND_WeatherHandle, &mData, &mpClass));
        if (result < Csis::System::kResult_Ok) {
            Csis::gFX_WIND_WeatherHandle.Set(&Csis::FX_WIND_WeatherId);
            Csis::Class::CreateInstance(&Csis::gFX_WIND_WeatherHandle, &mData, &mpClass);
        }
    }

    ~FX_WIND_Weather() {
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

    void SetVolume(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        mData.volume = x;
    }

    void SetPitch_offset(int x) {
        mData.pitch_offset = x;
    }

    void SetWidth(int x) {
        mData.width = x;
    }

    void SetIntensity(int x) {
        mData.intensity = x;
    }

    Csis::Class *mpClass;        // offset 0x0, size 0x4
    FX_WIND_WeatherStruct mData; // offset 0x4, size 0x10
};

SndBase::TypeInfo CARSFX_WindWeather::s_TypeInfo = {
    0x000200C0,
    "CARSFX_WindWeather",
    &SndBase::s_TypeInfo,
    CARSFX_WindWeather::CreateObject,
};

SndBase::TypeInfo *CARSFX_WindWeather::GetTypeInfo() const {
    return &s_TypeInfo;
}

const char *CARSFX_WindWeather::GetTypeName() const {
    return s_TypeInfo.typeName;
}

SndBase *CARSFX_WindWeather::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (s_TypeInfo.typeName, false) CARSFX_WindWeather();
    }
    return new (s_TypeInfo.typeName, true) CARSFX_WindWeather();
}

CARSFX_WindWeather::CARSFX_WindWeather()
    : CARSFX() {
    refCnt = 0;
    m_pcsisWind = nullptr;
}

CARSFX_WindWeather::~CARSFX_WindWeather() {
    Destroy();
}

void CARSFX_WindWeather::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
}

void CARSFX_WindWeather::InitSFX() {
    SndBase::InitSFX();
}

void CARSFX_WindWeather::Destroy() {
    if (m_pcsisWind) {
        delete m_pcsisWind;
    }
    m_pcsisWind = nullptr;
}

void CARSFX_WindWeather::UpdateParams(float t) {
    if (!m_pcsisWind) {
        Play();
    }

    WeatherSpeedScale = WeatherWindVolSlope.GetValue(GetPhysCar()->GetVelocityMagnitudeMPH());
}

void CARSFX_WindWeather::Play() {
    g_pEAXSound->SetCsisName("SND: Csis WindWeather");
    m_pcsisWind = new FX_WIND_Weather(0, 0, 0, 0);
    refCnt = m_pcsisWind->GetRefCount();
}

void CARSFX_WindWeather::ProcessUpdate() {
    if (m_pcsisWind) {
        int TempVol = static_cast<int>(static_cast<float>(GetDMixOutput(0, DMX_VOL)) * WeatherSpeedScale);

        if (TempVol < 0) {
            TempVol = 0;
        } else if (TempVol > 0x7FFF) {
            TempVol = 0x7FFF;
        }

        m_pcsisWind->SetVolume(TempVol);
        m_pcsisWind->SetWidth(4000);
        m_pcsisWind->SetIntensity(0);
        m_pcsisWind->CommitMemberData();
    }
}
