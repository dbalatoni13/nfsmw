#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Collision.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_Collision.hpp"

CSTATEMGR_Collision::CSTATEMGR_Collision() {}

CSTATEMGR_Collision::~CSTATEMGR_Collision() {}

void CSTATEMGR_Collision::EnterWorld(eSndGameMode esgm) {
    int ColSFXID = 1;

    for (int n = 0; n < 10; n++) {
        CSTATE_Base *NewState = CreateState(0, ColSFXID);
        NewState->Setup(ColSFXID);
    }

    CSTATEMGR_Base::EnterWorld(esgm);
}

void CSTATEMGR_Collision::UpdateParams(float t) {
    CSTATEMGR_Base::UpdateParams(t);
}
static int GetCollisionPriority(Sound::CollisionEvent *collision) {
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

    if (collision->IsDescribed(2) && collision->IsDescribed(4) &&
        100.0f < static_cast<float>(collision->GetIntensity())) {
        return 4;
    }
    return 3;
}

CSTATE_Base *CSTATEMGR_Collision::GetFreeState(void *ObjectPtr) {
    Sound::CollisionEvent *pCollision;
    CSTATE_Collision *curcollision;
    CSTATE_Base *CurReturn;
    int curpriority;
    CSTATE_Collision *smallestFound;
    int smalledpriority;

    pCollision = static_cast<Sound::CollisionEvent *>(ObjectPtr);
    curcollision = static_cast<CSTATE_Collision *>(m_pHeadStateObj);
    while (curcollision && pCollision->IsDescribed(2)) {
        if (curcollision->IsAttached() && curcollision->m_pCollisionEvent->GetAge() <= 0.25f &&
            curcollision->m_pCollisionEvent->IsDescribed(2) && !curcollision->m_pCollisionEvent->IsDescribed(0x200) &&
            (static_cast<float>(pCollision->GetIntensity()) <= 70.0f ||
             70.0f <= static_cast<float>(curcollision->m_pCollisionEvent->GetIntensity()))) {
            if (pCollision->GetActor() == curcollision->m_pCollisionEvent->GetActor()) {
                if (pCollision->GetActee() == curcollision->m_pCollisionEvent->GetActee()) {
                    return nullptr;
                }
            }
            if (pCollision->GetActor() == curcollision->m_pCollisionEvent->GetActee()) {
                if (pCollision->GetActee() == curcollision->m_pCollisionEvent->GetActor()) {
                    return nullptr;
                }
            }
        }

        curcollision = static_cast<CSTATE_Collision *>(curcollision->m_pNextState);
    }

    CurReturn = CSTATEMGR_Base::GetFreeState(ObjectPtr);
    if (CurReturn) {
        return CurReturn;
    }

    curpriority = GetCollisionPriority(pCollision);
    smallestFound = nullptr;
    smalledpriority = 11;
    for (curcollision = static_cast<CSTATE_Collision *>(m_pHeadStateObj); curcollision;
         curcollision = static_cast<CSTATE_Collision *>(curcollision->m_pNextState)) {
        if (curcollision->IsAttached()) {
            int newpriority = GetCollisionPriority(curcollision->m_pCollisionEvent);

            if (smalledpriority > newpriority) {
                smallestFound = curcollision;
                smalledpriority = newpriority;
            }
        }
    }

    CurReturn = nullptr;
    if (curpriority > smalledpriority) {
        smallestFound->Detach();
        CurReturn = smallestFound;
    }

    return CurReturn;
}

