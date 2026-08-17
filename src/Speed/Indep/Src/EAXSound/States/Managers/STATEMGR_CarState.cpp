#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_CarState.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/Src/EAXSound/EAXSoundTypes.h"
#include "Speed/Indep/Src/EAXSound/SndCamera.hpp"
#include "Speed/Indep/Src/EAXSound/SoundConn.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/engineaudio_hash.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"

CSTATEMGR_CarState::MappingList CSTATEMGR_CarState::FinalMapping;           // size: 0x0, Decl: 14
CSTATEMGR_CarState::EngineList CSTATEMGR_CarState::FinalEngines;            // size: 0x0, Decl: 15
CSTATEMGR_CarState::EngineList CSTATEMGR_CarState::FinalCopV8Engines;       // size: 0x0, Decl: 16
CSTATEMGR_CarState::EngToCarMapList CSTATEMGR_CarState::EngineToCarMapping; // size: 0x0, Decl: 18

bool CSTATEMGR_CarState::CopsCanBeInGame = true; // Decl: 20

CSTATEMGR_CarState::CSTATEMGR_CarState() {
    this->m_fConnectDistance = 135.0f;
    this->m_CarContext = Sound::CONTEXT_AIRACER;
    this->CopsCanBeInGame = true;
}

CSTATEMGR_CarState::~CSTATEMGR_CarState() {}

void CSTATEMGR_CarState::UpdateParams(float t) {
    ProfileNode profile_node("TODO", 0);

    for (CarSoundConn::List::const_iterator iter = CarSoundConn::GetList().begin(); iter != CarSoundConn::GetList().end(); ++iter) {
        CarSoundConn *pconn = *iter;

        if (!pconn->mConnected) {
            continue;
        }

        EAX_CarState *eax_car = pconn->GetState();

        if (eax_car == nullptr || eax_car->GetContext() != this->m_CarContext) {
            continue;
        }

        CSTATE_Base *attached = this->m_pHeadStateObj;
        while (attached != nullptr) {
            if (attached->IsAttached() && attached->GetPhysCar() == eax_car) {
                break;
            }

            attached = attached->m_pNextState;
        }

        if (!eax_car->IsSimUpdating() && ((this->m_CarContext != Sound::CONTEXT_AIRACER && this->m_CarContext != Sound::CONTEXT_COP) ||
                                          INIS::Get() == nullptr || !INIS::Get()->IsPlaying())) {
            if (attached != nullptr) {
                attached->Detach();
            }
        } else {
            bool IsInRadius = false;
            int CarID = 0;
            GetClosestPlayerCar(eax_car->GetPosition(), true, CarID);
            IsInRadius = bDistBetween(eax_car->GetPosition(), SndCamera::GetCamPos3(CarID)) < this->m_fConnectDistance;

            if (attached != nullptr) {
                if (!IsInRadius) {
                    attached->Detach();
                }
            } else if (IsInRadius) {
                CSTATE_Base *pFoundCar = this->GetFreeState(eax_car);

                if (pFoundCar != nullptr && (this->m_CarContext != Sound::CONTEXT_COP || !Sim::IsSplitScreen())) {
                    pFoundCar->Attach(eax_car);
                }
            }
        }
    }

    CSTATEMGR_Base::UpdateParams(t);
}

// size: 0x10, address: 0x803DD468, Decl: 231
static const Attrib::Key V8CopEngines[4] = {
    Attrib::Hash::engineaudio::key_corvette_z06,
    Attrib::Hash::engineaudio::key_for_mus_b,
    Attrib::Hash::engineaudio::key_for_mus_c,
    Attrib::Hash::engineaudio::key_hum_h1,
};

bool sort_engine_priority(unsigned int firstkey, unsigned int secondkey) {
    Attrib::Gen::engineaudio First(firstkey, 0, nullptr);
    Attrib::Gen::engineaudio Second(secondkey, 0, nullptr);

    return First.Priority() > Second.Priority();
}

void CSTATEMGR_CarState::ResetCarBanks() {
    CSTATEMGR_CarState *CarStateMgr = static_cast<CSTATEMGR_CarState *>(EAXSound::GetStateMgr(eMM_PLAYERCAR));

    FinalMapping.clear();
    FinalEngines.clear();
    FinalCopV8Engines.clear();
    EngineToCarMapping.clear();
}

static const int DEBUG_PRINT_CAR_BANK_RESOLVE = 0; // size: 0x4, Decl: 263
int DEBUG_CAR_BANK_TEST_CASE = -1;                 // size: 0x4, address: 0x80417F58, Decl: 264

bool ForcePrintResolveInfo = false; // size: 0x1, address: 0x80417F5C, Decl: 283

// UNSOLVED
void CSTATEMGR_CarState::ResolveCarBanks() {
    UTL::FixedVector<unsigned int, 8> AIEnginesWeWantToLoad;
    UTL::FixedVector<unsigned int, 8> EnginesThatCanUpgradeToV8;
    UTL::FixedVector<unsigned int, 8> EnginesThatAreV8;

    AIEnginesWeWantToLoad.clear();
    EnginesThatCanUpgradeToV8.clear();
    EnginesThatAreV8.clear();

    bool CopsCanBeInGame = !FEDatabase->IsSplitScreenMode(); // TODO this is actually !IsQuickRaceMode()
    ForcePrintResolveInfo = false;

    if (DEBUG_CAR_BANK_TEST_CASE == -1) {
        int NumEnginesWeWantToLoad;

        for (CarSoundConn *const *iter = CarSoundConn::GetList().begin(); iter != CarSoundConn::GetList().end(); ++iter) {
            CarSoundConn *pconn = *iter;
            EAX_CarState *eax_car = pconn->GetState();

            if ((eax_car->GetContext() != Sound::CONTEXT_AIRACER && eax_car->GetContext() != Sound::CONTEXT_COP) || pconn->mConnected) {
                continue;
            }
            Attrib::Gen::engineaudio *AIEngine = eax_car->GetEngineInfo();
            unsigned int *found = std::find(FinalEngines.begin(), FinalEngines.end(), AIEngine->GetCollection());

            if (found == FinalEngines.end()) {
                found = std::find(AIEnginesWeWantToLoad.begin(), AIEnginesWeWantToLoad.end(), AIEngine->GetCollection());
                if (found == AIEnginesWeWantToLoad.end()) {
                    AIEnginesWeWantToLoad.push_back(AIEngine->GetCollection());
                }
            }
        }

        if (AIEnginesWeWantToLoad.size() == 0) {
            return;
        }

        NumEnginesWeWantToLoad = static_cast<int>(AIEnginesWeWantToLoad.size() + FinalEngines.size());
        if (!CopsCanBeInGame) {
            if (NumEnginesWeWantToLoad <= 4) {
                goto LoadRemainingEngines;
            }
        } else {
            if (NumEnginesWeWantToLoad <= 3) {
                goto LoadRemainingEngines;
            }
            if (NumEnginesWeWantToLoad <= 4 && EnginesThatAreV8.size() != 0) {
                goto LoadRemainingEngines;
            }
        }
    }

    std::sort(AIEnginesWeWantToLoad.begin(), AIEnginesWeWantToLoad.end(), sort_engine_priority);

    for (const unsigned int *iter = AIEnginesWeWantToLoad.begin(); iter != AIEnginesWeWantToLoad.end(); ++iter) {
        unsigned int EngKey = *iter;
        Attrib::Gen::engineaudio AIEngine(EngKey, 0, nullptr);

        if (AIEngine.MaybeV8()) {
            EnginesThatCanUpgradeToV8.push_back(AIEngine.GetCollection());
        }
        if (AIEngine.EngType() == eENGINE_V8) {
            EnginesThatAreV8.push_back(AIEngine.GetCollection());
        }
    }

    if (CopsCanBeInGame && FinalCopV8Engines.size() == 0) {
        static int LastV8Used = 0;

        if (EnginesThatAreV8.size() != 0) {
            LastV8Used %= EnginesThatAreV8.size();
            if (DEBUG_PRINT_CAR_BANK_RESOLVE) {
                Attrib::Gen::engineaudio first(EnginesThatAreV8[LastV8Used], 0, nullptr);
            }

            FinalEngines.push_back(EnginesThatAreV8[LastV8Used]);
            FinalCopV8Engines.push_back(EnginesThatAreV8[LastV8Used]);
            AddMapping(EnginesThatAreV8[LastV8Used], EnginesThatAreV8[LastV8Used]);

            unsigned int *found = std::find(AIEnginesWeWantToLoad.begin(), AIEnginesWeWantToLoad.end(), EnginesThatAreV8[LastV8Used]);
            AIEnginesWeWantToLoad.erase(found);
            LastV8Used++;
        } else {
            int V8ToLoad = g_pEAXSound->Random(4);
            unsigned int copengkey = V8CopEngines[V8ToLoad];
            if (DEBUG_PRINT_CAR_BANK_RESOLVE) {
                Attrib::Gen::engineaudio first(copengkey, 0, nullptr);
            }

            EnginesThatAreV8.push_back(copengkey);
            FinalEngines.push_back(copengkey);
            FinalCopV8Engines.push_back(copengkey);
            AddMapping(copengkey, copengkey);
        }

    BeginRule2:
        LastV8Used %= EnginesThatAreV8.size();
        if (EnginesThatCanUpgradeToV8.size() != 0) {
            EngineMappingPair mapping;
            mapping.Start = EnginesThatCanUpgradeToV8[0];
            mapping.Finish = FinalEngines[0];
            AddMapping(mapping.Start, mapping.Finish);

            unsigned int *found = std::find(AIEnginesWeWantToLoad.begin(), AIEnginesWeWantToLoad.end(), EnginesThatCanUpgradeToV8[0]);
            if (found != AIEnginesWeWantToLoad.end()) {
                AIEnginesWeWantToLoad.erase(found);
            }

            unsigned int *removefirstupgraded = EnginesThatCanUpgradeToV8.begin();
            if (removefirstupgraded != nullptr) {
                EnginesThatCanUpgradeToV8.erase(removefirstupgraded);
            }
            LastV8Used++;

            if (FinalEngines.size() + AIEnginesWeWantToLoad.size() <= 4) {
                goto LoadRemainingEngines;
            }
            goto BeginRule2;
        }
    }

BeginRule3: {
    int n = static_cast<int>(AIEnginesWeWantToLoad.size()) - 1;
    for (; n >= 0; --n) {
        int m = n - 1;
        for (; m >= 0; --m) {
            Attrib::Gen::engineaudio HighPriority(AIEnginesWeWantToLoad[m], 0, nullptr);
            Attrib::Gen::engineaudio LowerPriority(AIEnginesWeWantToLoad[n], 0, nullptr);

            if (HighPriority.EngType() == LowerPriority.EngType()) {

                AddMapping(AIEnginesWeWantToLoad[n], AIEnginesWeWantToLoad[m]);
                AddMapping(AIEnginesWeWantToLoad[m], AIEnginesWeWantToLoad[m]);
                FinalEngines.push_back(AIEnginesWeWantToLoad[m]);

                unsigned int *first = std::find(AIEnginesWeWantToLoad.begin(), AIEnginesWeWantToLoad.end(), AIEnginesWeWantToLoad[n]);
                unsigned int *second = std::find(AIEnginesWeWantToLoad.begin(), AIEnginesWeWantToLoad.end(), AIEnginesWeWantToLoad[m]);
                AIEnginesWeWantToLoad.erase(first);
                AIEnginesWeWantToLoad.erase(second);

                if (FinalEngines.size() + AIEnginesWeWantToLoad.size() <= 4) {
                    goto LoadRemainingEngines;
                }
                goto BeginRule3;
            }
        }
    }
}

BeginRule4: {
    int n = static_cast<int>(AIEnginesWeWantToLoad.size()) - 1;
    if (n >= 0) {
        Attrib::Gen::engineaudio wantstoload(AIEnginesWeWantToLoad[n], 0, nullptr);

        for (int m = 0; m < static_cast<int>(FinalEngines.size()); ++m) {
            Attrib::Gen::engineaudio isloaded(FinalEngines[m], 0, nullptr);

            if (wantstoload.EngType() == isloaded.EngType()) {
                EngineMappingPair mapping;
                mapping.Start = AIEnginesWeWantToLoad[n];
                mapping.Finish = FinalEngines[m];
                if (DEBUG_PRINT_CAR_BANK_RESOLVE) {
                    Attrib::Gen::engineaudio HighPriority(mapping.Start, 0, nullptr);
                    Attrib::Gen::engineaudio LowerPriority(mapping.Finish, 0, nullptr);
                }
                AddMapping(mapping.Start, mapping.Finish);

                unsigned int *first = std::find(AIEnginesWeWantToLoad.begin(), AIEnginesWeWantToLoad.end(), AIEnginesWeWantToLoad[n]);
                AIEnginesWeWantToLoad.erase(first);

                if (FinalEngines.size() + AIEnginesWeWantToLoad.size() <= 4) {
                    goto LoadRemainingEngines;
                }
                goto BeginRule4;
            }
        }

        EngineMappingPair mapping;
        mapping.Start = AIEnginesWeWantToLoad[n];
        mapping.Finish = FinalEngines[g_pEAXSound->Random(static_cast<int>(FinalEngines.size()))];
        if (DEBUG_PRINT_CAR_BANK_RESOLVE) {
            Attrib::Gen::engineaudio HighPriority(mapping.Start, 0, nullptr);
            Attrib::Gen::engineaudio LowerPriority(mapping.Finish, 0, nullptr);
        }
        AddMapping(mapping.Start, mapping.Finish);

        unsigned int *first = std::find(AIEnginesWeWantToLoad.begin(), AIEnginesWeWantToLoad.end(), AIEnginesWeWantToLoad[n]);
        AIEnginesWeWantToLoad.erase(first);

        if (FinalEngines.size() + AIEnginesWeWantToLoad.size() > 4) {
            goto BeginRule4;
        }
    }
}

LoadRemainingEngines:
    while (AIEnginesWeWantToLoad.size() != 0) {
        unsigned int eng = *AIEnginesWeWantToLoad.begin();

        FinalEngines.push_back(eng);

        EngineMappingPair mapping;
        mapping.Start = eng;
        mapping.Finish = eng;
        AddMapping(mapping.Start, mapping.Finish);
        AIEnginesWeWantToLoad.erase(AIEnginesWeWantToLoad.begin());
    }

    for (CarSoundConn *const *iter = CarSoundConn::GetList().begin(); iter != CarSoundConn::GetList().end(); ++iter) {
        CarSoundConn *pconn = *iter;
        EAX_CarState *eax_car = pconn->GetState();

        if ((eax_car->GetContext() != Sound::CONTEXT_AIRACER && eax_car->GetContext() != Sound::CONTEXT_COP) || pconn->mConnected) {
            continue;
        }

        bool found = false;
        for (const EngineMappingPair *iter = FinalMapping.begin(); iter != FinalMapping.end() && !found; ++iter) {
            EngineMappingPair mapping = *iter;
            if (eax_car->GetEngineInfo()->GetCollection() == mapping.Start) {
                EngToCarStruct carmapping;
                eax_car->GetEngineInfo()->ChangeWithDefault(mapping.Finish);
                found = true;

                carmapping.EngineKey = mapping.Finish;
                carmapping.pCar = eax_car;
                EngineToCarMapping.push_back(carmapping);
            }
        }
    }
}

void CSTATEMGR_CarState::DestroyCar(EAX_CarState *eax_car) {
    if (eax_car->GetContext() != Sound::CONTEXT_AIRACER && eax_car->GetContext() != Sound::CONTEXT_COP) {
        return;
    }

    unsigned int engkey = eax_car->GetEngineInfo()->GetCollection();
    bool IsStillRefed = false;

    for (EngToCarStruct *iter = EngineToCarMapping.begin(); iter != EngineToCarMapping.end();) {
        EngToCarStruct engcarmap = *iter;

        if (engcarmap.pCar == eax_car) {
            EngineToCarMapping.erase(iter);
        } else {
            if (engcarmap.EngineKey == engkey) {
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

        for (EngineMappingPair *iter3 = FinalMapping.begin(); iter3 != FinalMapping.end();) {
            EngineMappingPair mapping = *iter3;

            if (mapping.Finish == engkey) {
                FinalMapping.erase(iter3);
            } else {
                ++iter3;
            }
        }
    }
}

void CSTATEMGR_CarState::AddMapping(unsigned int key1, unsigned int key2) {
    EngineMappingPair mapping;
    mapping.Start = key1;
    mapping.Finish = key2;

    EngineMappingPair *finditer = std::find(FinalMapping.begin(), FinalMapping.end(), mapping);
    if (finditer == FinalMapping.end()) {
        FinalMapping.push_back(mapping);
    } else {
        int break_here;
    }
}
