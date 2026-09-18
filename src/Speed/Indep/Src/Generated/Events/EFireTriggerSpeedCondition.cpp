#include "EFireTriggerSpeedCondition.hpp"

#include <new>

#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"
#include "Speed/Indep/Src/World/WTrigger.h"

EFireTriggerSpeedCondition::EFireTriggerSpeedCondition(CARP::Trigger *pTrigger, float pSpeed, int pTriggerIfSpeedBelow, unsigned int phSimable)
    : Event(0x20), fTrigger(pTrigger), fSpeed(pSpeed), fTriggerIfSpeedBelow(pTriggerIfSpeedBelow), fhSimable(phSimable) {
}

EFireTriggerSpeedCondition::~EFireTriggerSpeedCondition() {
    if (fTrigger) {
        ISimable *sim = ISimable::FindInstance((HSIMABLE) fhSimable);

        if (sim) {
            IRigidBody &rBody = *sim->GetRigidBody();
            const UMath::Vector3 &vel = rBody.GetLinearVelocity();
            float speed = UMath::Sqrt(vel.x * vel.x + vel.y * vel.y + vel.z * vel.z);

            if (fTriggerIfSpeedBelow ? speed < fSpeed : speed > fSpeed) {
                static_cast<WTrigger *>(fTrigger)->FireEvents((HSIMABLE) fhSimable);
            }
        }
    }
}

const char *EFireTriggerSpeedCondition::GetEventName() const {
    return "EFireTriggerSpeedCondition";
}

void EFireTriggerSpeedCondition_MakeEvent_Callback(const void *staticData) {
    new EFireTriggerSpeedCondition(((EFireTriggerSpeedCondition::StaticData *) staticData)->fTrigger, ((EFireTriggerSpeedCondition::StaticData *) staticData)->fSpeed, ((EFireTriggerSpeedCondition::StaticData *) staticData)->fTriggerIfSpeedBelow, gEventDynamicData.fhSimable);
}

int EFireTriggerSpeedCondition_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 3) {
        new EFireTriggerSpeedCondition((CARP::Trigger *) lua_tostring(L, 1), lua_tonumber(L, 2), (int) lua_tonumber(L, 3), gEventDynamicData.fhSimable);
    }
    return 0;
}

void EFireTriggerSpeedCondition_ResolveEvent_Callback(void *event, const UGroup *group) {
    new (&((EFireTriggerSpeedCondition::StaticData *) event)->fTrigger) CARP::TagReference(group);
}
