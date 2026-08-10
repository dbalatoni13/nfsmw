#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Enviro.hpp"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Src/EAXSound/SndCamera.hpp"

STATETYPE_IMPLEMENT(0x60000, CSTATE_WorldObject, CSTATE_Base);

IMPLEMENT_LISTABLE(ISndAttachable);

void CSTATEMGR_Enviro::AddWorldObject(float x, float y, float z, WORLDOBJECT_TYPES type) {
    bVector3 Vec(x, y, z);
    WorldObject *newworldobj = new ("AUD: WorldObject", 0) WorldObject(Vec, type);
    this->m_WorldObjects.push_back(newworldobj);
}

void CSTATEMGR_Enviro::RegisterWorldObjects() {
    this->m_WorldObjects.clear();
    this->AddWorldObject(4734.0f, 933.0f, 17.0f, WORLDOBJECT_FOUNTAIN);
    this->AddWorldObject(3415.0f, 1024.0f, 76.0f, WORLDOBJECT_FOUNTAIN);
    this->AddWorldObject(3550.0f, 1359.0f, 92.0f, WORLDOBJECT_FOUNTAIN);
    this->AddWorldObject(3582.0f, 1079.0f, 85.0f, WORLDOBJECT_FOUNTAIN);
}

CSTATE_WorldObject::CSTATE_WorldObject()
    : CSTATE_Base(), //
      mSndAttachment(nullptr) {}

CSTATE_WorldObject::~CSTATE_WorldObject() {
    this->mSndAttachment = nullptr;
}

void CSTATE_WorldObject::Attach(void *pAttachment) {
    this->mSndAttachment = static_cast<ISndAttachable *>(pAttachment);
    CSTATE_Base::Attach(pAttachment);
}

bool CSTATE_WorldObject::Detach() {
    this->mSndAttachment = nullptr;
    return CSTATE_Base::Detach();
}

CSTATEMGR_Enviro::CSTATEMGR_Enviro() {
    this->RegisterWorldObjects();
}

CSTATEMGR_Enviro::~CSTATEMGR_Enviro() {
    for (UTL::Std::list<WorldObject *, _type_list>::iterator i = m_WorldObjects.begin(); i != m_WorldObjects.end(); ++i) {
        delete *i;
    }

    m_WorldObjects.clear();
}

void CSTATEMGR_Enviro::EnterWorld(eSndGameMode esgm) {
    int worldsfx = 1;
    for (int n = 0; n < 5; n++) {
        CSTATE_Base *NewState = CreateState(0, worldsfx);
        NewState->Setup(worldsfx);
    }
    CSTATEMGR_Base::EnterWorld(esgm);
}

void CSTATEMGR_Enviro::UpdateParams(float t) {
    if (SndCamera::NumPlayers < 1) {
        return;
    }

    for (ISndAttachable::List::const_iterator iter = ISndAttachable::GetList().begin(); iter != ISndAttachable::GetList().end(); ++iter) {
        ISndAttachable *psound = *iter;
        bool IsInRadius;
        EAX_CarState *pClosePlayer = GetClosestPlayerCar(psound->GetPosition());
        IsInRadius = IsCarInRadius(pClosePlayer, psound->GetPosition(), 70.0f);
        CSTATE_Base *pstate = this->GetStateObj(psound);

        if (pstate != nullptr) {
            if (!IsInRadius) {
                pstate->Detach();
            }
        } else if (IsInRadius) {
            CSTATE_Base *pFreeState = this->GetFreeState(psound);
            if (pFreeState != nullptr) {
                pFreeState->Attach(psound);
            }
        }
    }

    CSTATEMGR_Base::UpdateParams(t);
}
