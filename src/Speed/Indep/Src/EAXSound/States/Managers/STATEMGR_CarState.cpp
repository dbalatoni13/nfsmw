#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_CarState.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/Src/EAXSound/SndCamera.hpp"
#include "Speed/Indep/Src/EAXSound/SoundConn.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"

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

CSTATEMGR_CarState::CSTATEMGR_CarState() {
    m_fConnectDistance = 135.0f;
    m_CarContext = Sound::CONTEXT_AIRACER;
    CopsCanBeInGame = true;
}

CSTATEMGR_CarState::~CSTATEMGR_CarState() {}

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

void CSTATEMGR_CarState::UpdateParams(float t) {
    ProfileNode profile_node("TODO", 0);
    typedef UTL::Collections::Listable<CarSoundConn, 10> CarList;

    for (CarSoundConn *const *iter = CarList::GetList().begin(); iter != CarList::GetList().end(); ++iter) {
        CarSoundConn *pconn = *iter;
        EAX_CarState *eax_car;
        CSTATE_Base *attached;

        if (!pconn->mConnected || !(eax_car = pconn->GetState()) || eax_car->GetContext() != m_CarContext) {
            continue;
        }

        attached = m_pHeadStateObj;
        while (attached) {
            if (attached->IsAttached() && attached->GetPhysCar() == eax_car) {
                break;
            }

            attached = attached->m_pNextState;
        }

        if (!eax_car->IsSimUpdating() &&
            (static_cast<unsigned int>(m_CarContext - kRaceContext_Online) > 1 || !INIS::Get() ||
             !INIS::Get()->IsPlaying())) {
            if (attached) {
                attached->Detach();
            }
        } else {
            bool IsInRadius;
            int CarID;

            CarID = 0;
            GetClosestPlayerCar(eax_car->GetPosition(), true, CarID);
            IsInRadius = bDistBetween(eax_car->GetPosition(), SndCamera::GetCamPos3(CarID)) < m_fConnectDistance;
            if (attached) {
                if (!IsInRadius) {
                    attached->Detach();
                }
            } else if (IsInRadius) {
                attached = GetFreeState(eax_car);
                if (attached &&
                    (m_CarContext != kRaceContext_Career || Sim::GetUserMode() != Sim::USER_SPLIT_SCREEN)) {
                    attached->Attach(eax_car);
                }
            }
        }
    }

    CSTATEMGR_Base::UpdateParams(t);
}

void CSTATEMGR_CarState::ResolveCarBanks() {
    static int LastV8Used = 0;

    UTL::FixedVector<unsigned int, 8, 16> AIEnginesWeWantToLoad;
    UTL::FixedVector<unsigned int, 8, 16> EnginesThatCanUpgradeToV8;
    UTL::FixedVector<unsigned int, 8, 16> EnginesThatAreV8;

    AIEnginesWeWantToLoad.clear();
    EnginesThatCanUpgradeToV8.clear();
    EnginesThatAreV8.clear();
    bool CopsCanBeInGame = !FEDatabase->IsQuickRaceMode();
    ForcePrintResolveInfo = false;

    typedef UTL::Collections::Listable<CarSoundConn, 10> CarList;
    if (DEBUG_CAR_BANK_TEST_CASE == -1) {
        for (CarSoundConn *const *iter = CarList::GetList().begin(); iter != CarList::GetList().end(); ++iter) {
            CarSoundConn *pconn = *iter;
            EAX_CarState *eax_car = pconn->GetState();

            if ((static_cast<int>(eax_car->GetContext()) == kRaceContext_Online ||
                 static_cast<int>(eax_car->GetContext()) == kRaceContext_Career) &&
                !pconn->mConnected) {
                Attrib::Gen::engineaudio *AIEngine = eax_car->GetEngineInfo();
                unsigned int *found =
                    std::find(FinalEngines.begin(), FinalEngines.end(), AIEngine->GetCollection());

                if (found == FinalEngines.end()) {
                    found = std::find(AIEnginesWeWantToLoad.begin(),
                                      AIEnginesWeWantToLoad.end(),
                                      AIEngine->GetCollection());
                    if (found == AIEnginesWeWantToLoad.end()) {
                        AIEnginesWeWantToLoad.push_back(AIEngine->GetCollection());
                    }
                }
            }
        }
    }

    int NumEnginesWeWantToLoad = static_cast<int>(AIEnginesWeWantToLoad.size());

    if (NumEnginesWeWantToLoad == 0) {
        return;
    }

    if (DEBUG_CAR_BANK_TEST_CASE != -1 ||
        (CopsCanBeInGame ? NumEnginesWeWantToLoad + static_cast<int>(FinalEngines.size()) > 3
                         : NumEnginesWeWantToLoad + static_cast<int>(FinalEngines.size()) > 4)) {
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
            int V8ToLoad;

            if (EnginesThatAreV8.size() == 0) {
                unsigned int copengkey = V8CopEngines[g_pEAXSound->Random(4)];
                EnginesThatAreV8.push_back(copengkey);
                FinalEngines.push_back(copengkey);
                FinalCopV8Engines.push_back(copengkey);
                AddMapping(copengkey, copengkey);
            } else {
                V8ToLoad = LastV8Used % EnginesThatAreV8.size();
                FinalEngines.push_back(EnginesThatAreV8[V8ToLoad]);
                FinalCopV8Engines.push_back(EnginesThatAreV8[V8ToLoad]);
                AddMapping(EnginesThatAreV8[V8ToLoad], EnginesThatAreV8[V8ToLoad]);

                unsigned int *found = std::find(AIEnginesWeWantToLoad.begin(),
                                                AIEnginesWeWantToLoad.end(),
                                                EnginesThatAreV8[V8ToLoad]);
                if (found != AIEnginesWeWantToLoad.end()) {
                    AIEnginesWeWantToLoad.erase(found);
                }
                ++LastV8Used;
            }

            while (FinalEngines.size() + AIEnginesWeWantToLoad.size() > 4) {
                LastV8Used %= EnginesThatAreV8.size();
                if (EnginesThatCanUpgradeToV8.size() == 0) {
                    break;
                }

                AddMapping(EnginesThatCanUpgradeToV8[0], FinalEngines[0]);

                unsigned int *found = std::find(AIEnginesWeWantToLoad.begin(),
                                                AIEnginesWeWantToLoad.end(),
                                                EnginesThatCanUpgradeToV8[0]);
                if (found != AIEnginesWeWantToLoad.end()) {
                    AIEnginesWeWantToLoad.erase(found);
                }
                EnginesThatCanUpgradeToV8.erase(EnginesThatCanUpgradeToV8.begin());
                ++LastV8Used;
            }
        }

        while (AIEnginesWeWantToLoad.size() > 0) {
            int n = static_cast<int>(AIEnginesWeWantToLoad.size()) - 1;
            int m;

            for (m = n - 1; m >= 0; --m) {
                Attrib::Gen::engineaudio HighPriority(AIEnginesWeWantToLoad[m], 0, nullptr);
                Attrib::Gen::engineaudio LowerPriority(AIEnginesWeWantToLoad[n], 0, nullptr);

                if (HighPriority.EngType() == LowerPriority.EngType()) {
                    AddMapping(AIEnginesWeWantToLoad[n], AIEnginesWeWantToLoad[m]);
                    AddMapping(AIEnginesWeWantToLoad[m], AIEnginesWeWantToLoad[m]);
                    FinalEngines.push_back(AIEnginesWeWantToLoad[m]);

                    unsigned int *first = std::find(AIEnginesWeWantToLoad.begin(),
                                                    AIEnginesWeWantToLoad.end(),
                                                    AIEnginesWeWantToLoad[n]);
                    unsigned int *second = std::find(AIEnginesWeWantToLoad.begin(),
                                                     AIEnginesWeWantToLoad.end(),
                                                     AIEnginesWeWantToLoad[m]);
                    if (first != AIEnginesWeWantToLoad.end()) {
                        AIEnginesWeWantToLoad.erase(first);
                    }
                    if (second != AIEnginesWeWantToLoad.end()) {
                        AIEnginesWeWantToLoad.erase(second);
                    }
                    break;
                }
            }

            if (m < 0) {
                break;
            }

            if (FinalEngines.size() + AIEnginesWeWantToLoad.size() < 5) {
                break;
            }
        }

        while (AIEnginesWeWantToLoad.size() > 0) {
            int n = static_cast<int>(AIEnginesWeWantToLoad.size()) - 1;
            Attrib::Gen::engineaudio wantstoload(AIEnginesWeWantToLoad[n], 0, nullptr);
            int m;

            for (m = 0; m < static_cast<int>(FinalEngines.size()); ++m) {
                Attrib::Gen::engineaudio LowerPriority(FinalEngines[m], 0, nullptr);

                if (wantstoload.EngType() == LowerPriority.EngType()) {
                    AddMapping(AIEnginesWeWantToLoad[n], FinalEngines[m]);

                    unsigned int *first =
                        std::find(AIEnginesWeWantToLoad.begin(),
                                  AIEnginesWeWantToLoad.end(),
                                  AIEnginesWeWantToLoad[n]);
                    if (first != AIEnginesWeWantToLoad.end()) {
                        AIEnginesWeWantToLoad.erase(first);
                    }
                    break;
                }
            }

            if (m >= static_cast<int>(FinalEngines.size())) {
                AddMapping(AIEnginesWeWantToLoad[n],
                           FinalEngines[g_pEAXSound->Random(static_cast<int>(FinalEngines.size()))]);

                unsigned int *first = std::find(AIEnginesWeWantToLoad.begin(),
                                                AIEnginesWeWantToLoad.end(),
                                                AIEnginesWeWantToLoad[n]);
                if (first != AIEnginesWeWantToLoad.end()) {
                    AIEnginesWeWantToLoad.erase(first);
                }
            }

            if (FinalEngines.size() + AIEnginesWeWantToLoad.size() < 5) {
                break;
            }
        }
    }

    while (AIEnginesWeWantToLoad.size() != 0) {
        unsigned int eng = AIEnginesWeWantToLoad[0];

        FinalEngines.push_back(eng);
        AddMapping(eng, eng);
        AIEnginesWeWantToLoad.erase(AIEnginesWeWantToLoad.begin());
    }

    for (CarSoundConn *const *iter = CarList::GetList().begin(); iter != CarList::GetList().end(); ++iter) {
        CarSoundConn *pconn = *iter;
        EAX_CarState *eax_car = pconn->GetState();

        if ((static_cast<int>(eax_car->GetContext()) == kRaceContext_Online ||
             static_cast<int>(eax_car->GetContext()) == kRaceContext_Career) &&
            !pconn->mConnected) {
            bool found = false;

            for (const EngineMappingPair *iter = FinalMapping.begin();
                 iter != FinalMapping.end() && !found;
                 ++iter) {
                EngineMappingPair mapping = *iter;

                if (eax_car->GetEngineInfo()->GetCollection() == mapping.Start) {
                    eax_car->GetEngineInfo()->ChangeWithDefault(mapping.Finish);
                    found = true;

                    EngToCarStruct carmapping;
                    carmapping.EngineKey = mapping.Finish;
                    carmapping.pCar = eax_car;
                    EngineToCarMapping.push_back(carmapping);
                }
            }
        }
    }
}

void CSTATEMGR_CarState::ResetCarBanks() {
    FinalMapping.clear();
    FinalEngines.clear();
    FinalCopV8Engines.clear();
    EngineToCarMapping.clear();
}

void CSTATEMGR_CarState::DestroyCar(EAX_CarState *eax_car) {
    if (static_cast<int>(eax_car->GetContext()) == kRaceContext_Online ||
        static_cast<int>(eax_car->GetContext()) == kRaceContext_Career) {
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
