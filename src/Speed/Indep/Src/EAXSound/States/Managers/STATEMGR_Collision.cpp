#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Collision.hpp"
#include "Speed/Indep/Src/EAXSound/SoundCollision.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_Collision.hpp"

static const float AgeThresholdForPruningDups = 0.25f;   // size: 0x4, Decl: 10
static const float IntensityThresholdForPruning = 70.0f; // size: 0x4, Decl: 11

CSTATEMGR_Collision::CSTATEMGR_Collision() {}

CSTATEMGR_Collision::~CSTATEMGR_Collision() {}

void CSTATEMGR_Collision::EnterWorld(eSndGameMode esgm) {
    int ColSFXID = 1;

    for (int n = 0; n < 10; n++) {
        CSTATE_Base *NewState = this->CreateState(0, ColSFXID);
        NewState->Setup(ColSFXID);
    }

    CSTATEMGR_Base::EnterWorld(esgm);
}

void CSTATEMGR_Collision::UpdateParams(float t) {
    CSTATEMGR_Base::UpdateParams(t);
}

// TODO dwarf, missing to IsDescribed calls
int GetCollisionPriority(Sound::CollisionEvent *collision) {
    if (collision->IsDescribed(0x400)) {
        return 10;
    }

    if (collision->IsDescribed(1)) {
        if (collision->IsDescribed(4) || collision->IsDescribed(8)) {
            if (100.0f < static_cast<float>(collision->GetIntensity())) {
                return 9;
            }
            return 8;
        }

        if (collision->IsDescribed(2)) {
            if (100.0f < static_cast<float>(collision->GetIntensity())) {
                return 7;
            }
            return 6;
        }
        return 7;
    }

    if (collision->IsDescribed(0x10)) {
        if (collision->IsDescribed(4) || collision->IsDescribed(8)) {
            if (100.0f < static_cast<float>(collision->GetIntensity())) {
                return 6;
            }
            return 5;
        }

        if (collision->IsDescribed(2) && 100.0f < static_cast<float>(collision->GetIntensity())) {
            return 5;
        }
        return 4;
    }

    if (collision->IsDescribed(2) && collision->IsDescribed(4) && 100.0f < static_cast<float>(collision->GetIntensity())) {
        return 4;
    }
    return 3;
}

CSTATE_Base *CSTATEMGR_Collision::GetFreeState(void *ObjectPtr) {
    Sound::CollisionEvent *pCollision = static_cast<Sound::CollisionEvent *>(ObjectPtr);
    CSTATE_Collision *curcollision = static_cast<CSTATE_Collision *>(this->m_pHeadStateObj);

    while ((curcollision != nullptr) && pCollision->IsDescribed(2)) {
        if (curcollision->IsAttached() && curcollision->m_pCollisionEvent->GetAge() <= AgeThresholdForPruningDups &&
            curcollision->m_pCollisionEvent->IsDescribed(2) && !curcollision->m_pCollisionEvent->IsDescribed(0x200)) {

            if ((static_cast<float>(pCollision->GetIntensity()) <= IntensityThresholdForPruning ||
                 IntensityThresholdForPruning <= static_cast<float>(curcollision->m_pCollisionEvent->GetIntensity()))) {

                if ((pCollision->GetActor() == curcollision->m_pCollisionEvent->GetActor() &&
                     pCollision->GetActee() == curcollision->m_pCollisionEvent->GetActee()) ||
                    (pCollision->GetActor() == curcollision->m_pCollisionEvent->GetActee() &&
                     pCollision->GetActee() == curcollision->m_pCollisionEvent->GetActor())) {
                    return nullptr;
                }

                goto ContinueLooping;
            }
        }

    ContinueLooping:
        curcollision = static_cast<CSTATE_Collision *>(curcollision->m_pNextState);
    }

    CSTATE_Base *CurReturn = CSTATEMGR_Base::GetFreeState(ObjectPtr);
    if (CurReturn != nullptr) {
        return CurReturn;
    }

    int curpriority = GetCollisionPriority(pCollision);
    CSTATE_Collision *smallestFound = nullptr;
    int smalledpriority = 11;

    for (curcollision = static_cast<CSTATE_Collision *>(this->m_pHeadStateObj); curcollision != nullptr;
         curcollision = static_cast<CSTATE_Collision *>(curcollision->m_pNextState)) {
        if (curcollision->IsAttached()) {
            Sound::CollisionEvent *collision = curcollision->m_pCollisionEvent;
            int newpriority = GetCollisionPriority(collision);

            if (smalledpriority > newpriority) {
                smallestFound = curcollision;
                smalledpriority = newpriority;
            }
        }
    }

    if (curpriority > smalledpriority) {
        smallestFound->Detach();
        return smallestFound;
    }

    return nullptr;
}
