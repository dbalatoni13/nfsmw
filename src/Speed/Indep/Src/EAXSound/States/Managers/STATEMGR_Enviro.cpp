#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Enviro.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"

CSTATE_Base::StateInfo CSTATE_WorldObject::s_StateInfo = {
    0x00060000,
    "CSTATE_WorldObject",
    &CSTATE_Base::s_StateInfo,
    CSTATE_WorldObject::CreateState,
};

CSTATE_WorldObject::StateInfo *CSTATE_WorldObject::GetStateInfo() const {
    return &s_StateInfo;
}

const char *CSTATE_WorldObject::GetStateName() const {
    return s_StateInfo.stateName;
}

CSTATE_Base *CSTATE_WorldObject::CreateState(unsigned int allocator) {
    if (allocator == 0) {
        return new (AudioMemBase::operator new(
            sizeof(CSTATE_WorldObject), GetStaticStateInfo()->stateName, false)) CSTATE_WorldObject;
    }
    return new (AudioMemBase::operator new(
        sizeof(CSTATE_WorldObject), GetStaticStateInfo()->stateName, true)) CSTATE_WorldObject;
}

void CSTATEMGR_Enviro::AddWorldObject(float x, float y, float z, WORLDOBJECT_TYPES type) {
    bVector3 Vec(x, y, z);
    WorldObject *newworldobj;

    newworldobj = new ("AUD: WorldObject", 0) WorldObject(Vec, type);
    m_WorldObjects.push_back(newworldobj);
}

void CSTATEMGR_Enviro::RegisterWorldObjects() {
    m_WorldObjects.clear();
    AddWorldObject(4734.0f, 933.0f, 17.0f, WORLDOBJECT_FOUNTAIN);
    AddWorldObject(3415.0f, 1024.0f, 76.0f, WORLDOBJECT_FOUNTAIN);
    AddWorldObject(3550.0f, 1359.0f, 92.0f, WORLDOBJECT_FOUNTAIN);
    AddWorldObject(3582.0f, 1079.0f, 85.0f, WORLDOBJECT_FOUNTAIN);
}

CSTATE_WorldObject::CSTATE_WorldObject()
    : CSTATE_Base()
    , mSndAttachment(nullptr) {}

CSTATE_WorldObject::~CSTATE_WorldObject() {
    mSndAttachment = nullptr;
}

void CSTATE_WorldObject::Attach(void *pAttachment) {
    mSndAttachment = static_cast<ISndAttachable *>(pAttachment);
    CSTATE_Base::Attach(pAttachment);
}

bool CSTATE_WorldObject::Detach() {
    mSndAttachment = nullptr;
    return CSTATE_Base::Detach();
}

CSTATEMGR_Enviro::CSTATEMGR_Enviro() {
    RegisterWorldObjects();
}

CSTATEMGR_Enviro::~CSTATEMGR_Enviro() {
    UTL::Std::list<WorldObject *, _type_list>::iterator i;

    for (i = m_WorldObjects.begin(); i != m_WorldObjects.end(); ++i) {
        if (*i) {
            (*i)->~WorldObject();
        }
    }

    m_WorldObjects.clear();
}

void CSTATEMGR_Enviro::EnterWorld(eSndGameMode esgm) {
    int worldsfx;
    int n;

    worldsfx = 1;
    n = 0;
    do {
        CSTATE_Base *NewState = CreateState(0, worldsfx);
        NewState->Setup(worldsfx);
        n = n + 1;
    } while (n < 5);
    CSTATEMGR_Base::EnterWorld(esgm);
}

void CSTATEMGR_Enviro::UpdateParams(float t) {
    if (SndCamera::NumPlayers < 1) {
        return;
    }

    for (ISndAttachable *const *iter = ISndAttachable::GetList().begin(); iter != ISndAttachable::GetList().end(); ++iter) {
        ISndAttachable *psound = *iter;
        EAX_CarState *pClosePlayer = GetClosestPlayerCar(psound->GetPosition());
        bool IsInRadius = IsCarInRadius(pClosePlayer, psound->GetPosition(), 70.0f);
        CSTATE_Base *pstate = GetStateObj(psound);

        if (!pstate) {
            if (IsInRadius) {
                CSTATE_Base *pFreeState = GetFreeState(psound);
                if (pFreeState) {
                    pFreeState->Attach(psound);
                }
            }
        } else if (!IsInRadius) {
            pstate->Detach();
        }
    }

    CSTATEMGR_Base::UpdateParams(t);
}
