#include "./CARSFX_RoadNoise.hpp"

#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/STICH_Playback.h"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Wheel.hpp"
#include "Speed/Indep/Src/EAXSound/snd_gen/ENVIRO_AEMS.h"

struct cStitchLoop {
    void Update(const SND_Params *Params, float dt);
};

extern int RoadNoiseVolumes[9];

SndBase::TypeInfo CARSFX_RoadNoise::s_TypeInfo = { 0, "CARSFX_RoadNoise", nullptr, CARSFX_RoadNoise::CreateObject };

SndBase::TypeInfo *CARSFX_RoadNoise::GetTypeInfo() const { return &s_TypeInfo; }

const char *CARSFX_RoadNoise::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase *CARSFX_RoadNoise::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (CARSFX_RoadNoise::GetStaticTypeInfo()->typeName, false) CARSFX_RoadNoise();
    }
    return new (CARSFX_RoadNoise::GetStaticTypeInfo()->typeName, true) CARSFX_RoadNoise();
}

CARSFX_RoadNoise::CARSFX_RoadNoise() {
    m_pWheelCtl = nullptr;
    m_pLeftWheelPos = nullptr;
    m_pRightWheelPos = nullptr;
    m_eDriverType = DRIVER_TYPE_NONE;
    m_pWetRoad[0] = nullptr;
    m_pWetRoad[1] = nullptr;
    m_pRoadNoiseControl[0] = nullptr;
    m_pRoadNoiseControl[1] = nullptr;
    m_pStitchLoopControl[0] = nullptr;
    m_pStitchLoopControl[1] = nullptr;
    m_pTransition[0] = nullptr;
    m_pTransition[1] = nullptr;
    m_pStitchTransition[0] = nullptr;
    m_pStitchTransition[1] = nullptr;
    TransitionVol[0] = 0;
    TransitionVol[1] = 0;
    LoopID[0] = FXROADNOISE_LOOP_NONE;
    LoopID[1] = FXROADNOISE_LOOP_NONE;
    m_nRTRoadNoiseVol = 0;
    m_nLTRoadNoiseVol = 0;
    m_nRTRoadNoisePitch = 0x1000;
    m_nLTRoadNoisePitch = 0x1000;
}

CARSFX_RoadNoise::~CARSFX_RoadNoise() {}

int CARSFX_RoadNoise::GetController(int Index) { return -1; }

void CARSFX_RoadNoise::AttachController(SFXCTL *psfxctl) {}

void CARSFX_RoadNoise::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
}

void CARSFX_RoadNoise::InitSFX() {
    SndBase::InitSFX();
}

void CARSFX_RoadNoise::UpdateParams(float t) {}

void CARSFX_RoadNoise::Destroy() {}

void CARSFX_RoadNoise::Detach() {}

eVOL_ROADNOISE CARSFX_RoadNoise::MapLoopToVolume(FXROADNOISE_LOOP ID) {
    switch (ID) {
    case FXROADNOISE_LOOP_ASPHALT:
        return eVOL_ROADNOISE_ASPHALT;
    case FXROADNOISE_LOOP_CONCRETE:
        return eVOL_ROADNOISE_CONCRETE;
    case FXROADNOISE_LOOP_DIRT_GRAVEL:
        return eVOL_ROADNOISE_DIRT_GRAVEL;
    case FXROADNOISE_LOOP_WETROAD:
        return eVOL_ROADNOISE_WET_ROAD;
    default:
        return eVOL_ROADNOISE_ASPHALT;
    }
}

void CARSFX_RoadNoise::PlayTransition(FXROADNOISE_TRANSITION TransitionID, int n) {}

void CARSFX_RoadNoise::GenerateRoadNoise() {}

void CARSFX_RoadNoise::Play(FXROADNOISE_LOOP ID, int n) {}

void CARSFX_RoadNoise::ProcessUpdate() {}
