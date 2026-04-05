#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_CarState.hpp"

#include <algorithm>

bool sort_engine_priority(unsigned int firstkey, unsigned int secondkey) {
    Attrib::Gen::engineaudio First(firstkey, 0, nullptr);
    Attrib::Gen::engineaudio Second(secondkey, 0, nullptr);

    return First.Priority() > Second.Priority();
}

UTL::FixedVector<EngineMappingPair, 24, 16> CSTATEMGR_CarState::FinalMapping;
UTL::FixedVector<unsigned int, 8, 16> CSTATEMGR_CarState::FinalEngines;
UTL::FixedVector<unsigned int, 8, 16> CSTATEMGR_CarState::FinalCopV8Engines;
UTL::FixedVector<CSTATEMGR_CarState::EngToCarStruct, 24, 16> CSTATEMGR_CarState::EngineToCarMapping;
bool CSTATEMGR_CarState::CopsCanBeInGame;

void CSTATEMGR_CarState::ResetCarBanks() {
    FinalMapping.clear();
    FinalEngines.clear();
    FinalCopV8Engines.clear();
    EngineToCarMapping.clear();
}

void CSTATEMGR_CarState::DestroyCar(EAX_CarState *eax_car) {
    if (eax_car->GetContext() == Sound::CONTEXT_ONLINE || eax_car->GetContext() == Sound::kRaceContext_Career) {
        unsigned int engkey = eax_car->GetEngineInfo()->GetCollection();
        bool IsStillRefed = false;

        EngToCarStruct *iter = EngineToCarMapping.begin();
        while (iter != EngineToCarMapping.end()) {
            if (iter->pCar == eax_car) {
                iter = EngineToCarMapping.erase(iter);
            } else {
                if (iter->EngineKey == engkey) {
                    IsStillRefed = true;
                }
                ++iter;
            }
        }

        if ((!CopsCanBeInGame || FinalCopV8Engines.size() == 0 || FinalCopV8Engines[0] != engkey) && !IsStillRefed) {
            unsigned int *finditer = std::find(FinalEngines.begin(), FinalEngines.end(), engkey);
            if (finditer != FinalEngines.end()) {
                FinalEngines.erase(finditer);
            }

            unsigned int *finditer2 = std::find(FinalCopV8Engines.begin(), FinalCopV8Engines.end(), engkey);
            if (finditer2 != FinalCopV8Engines.end()) {
                FinalCopV8Engines.erase(finditer2);
            }

            EngineMappingPair *iter3 = FinalMapping.begin();
            while (iter3 != FinalMapping.end()) {
                if (iter3->Finish == engkey) {
                    iter3 = FinalMapping.erase(iter3);
                } else {
                    ++iter3;
                }
            }
        }
    }
}
