#ifndef EAXSOUND_STATES_MANAGERS_STATEMGR_CARSTATE_H
#define EAXSOUND_STATES_MANAGERS_STATEMGR_CARSTATE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UTLVector.h"
#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Base.hpp"

// total size: 0x8
struct EngineMappingPair {
    unsigned int Start;  // offset 0x0, size 0x4
    unsigned int Finish; // offset 0x4, size 0x4

    bool operator==(const EngineMappingPair &cmp) const {
        return Start == cmp.Start && Finish == cmp.Finish;
    }
};

// total size: 0x24
struct CSTATEMGR_CarState : public CSTATEMGR_Base {
    // total size: 0x8
    struct EngToCarStruct {
        unsigned int EngineKey; // offset 0x0, size 0x4
        EAX_CarState *pCar;     // offset 0x4, size 0x4
    };

    static bool CopsCanBeInGame;
    static UTL::FixedVector<EngineMappingPair, 24, 16> FinalMapping;
    static UTL::FixedVector<unsigned int, 8, 16> FinalEngines;
    static UTL::FixedVector<unsigned int, 8, 16> FinalCopV8Engines;
    static UTL::FixedVector<EngToCarStruct, 24, 16> EngineToCarMapping;

    static void ResolveCarBanks();
    static void ResetCarBanks();
    static void DestroyCar(EAX_CarState *pCar);

    Sound::Context m_CarContext; // offset 0x1C, size 0x4
    float m_fConnectDistance;    // offset 0x20, size 0x4
};

#endif
