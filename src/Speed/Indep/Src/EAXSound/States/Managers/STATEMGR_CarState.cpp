#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_CarState.hpp"

#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"

#include <algorithm>

static const unsigned int V8CopEngines[4] = {0xF6A7F776, 0x6F1563C2, 0xEFF995DD, 0x7A947AB6};

int DEBUG_CAR_BANK_TEST_CASE = -1;
bool ForcePrintResolveInfo = false;

bool sort_engine_priority(unsigned int firstkey, unsigned int secondkey) {
    Attrib::Gen::engineaudio First(firstkey, 0, nullptr);
    Attrib::Gen::engineaudio Second(secondkey, 0, nullptr);

    return First.Priority() > Second.Priority();
}

UTL::FixedVector<EngineMappingPair, 24, 16> CSTATEMGR_CarState::FinalMapping;
UTL::FixedVector<unsigned int, 8, 16> CSTATEMGR_CarState::FinalEngines;
UTL::FixedVector<unsigned int, 8, 16> CSTATEMGR_CarState::FinalCopV8Engines;
UTL::FixedVector<CSTATEMGR_CarState::EngToCarStruct, 24, 16> CSTATEMGR_CarState::EngineToCarMapping;
bool CSTATEMGR_CarState::CopsCanBeInGame = true;

void CSTATEMGR_CarState::AddMapping(unsigned int key1, unsigned int key2) {
    EngineMappingPair mapping;
    EngineMappingPair *finditer;

    mapping.Start = key1;
    mapping.Finish = key2;
    finditer = std::find(FinalMapping.begin(), FinalMapping.end(), mapping);
    if (finditer == FinalMapping.end()) {
        FinalMapping.push_back(mapping);
    } else {
        int break_here;
    }
}

void CSTATEMGR_CarState::ResetCarBanks() {
    FinalMapping.clear();
    FinalEngines.clear();
    FinalCopV8Engines.clear();
    EngineToCarMapping.clear();
}

void CSTATEMGR_CarState::DestroyCar(EAX_CarState *eax_car) {
    if (eax_car->GetContext() == kRaceContext_Online || eax_car->GetContext() == kRaceContext_Career) {
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
