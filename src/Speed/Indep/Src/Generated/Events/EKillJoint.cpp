#include "EKillJoint.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"
#include "Speed/Indep/Src/Physics/Behaviors/RigidBody.h"

EKillJoint::EKillJoint(unsigned int phSimable) : Event(0x10), fhSimable(phSimable) {
}

EKillJoint::~EKillJoint() {
    ISimable *isimable = ISimable::FindInstance((HSIMABLE) fhSimable);

    if (isimable) {
        IDynamicsEntity *ientity;

        if (isimable->QueryInterface(&ientity)) {
            Dynamics::Articulation::Release(ientity);
        }
    }
}

const char *EKillJoint::GetEventName() const {
    return "EKillJoint";
}

void EKillJoint_MakeEvent_Callback(const void *staticData) {
    new EKillJoint(gEventDynamicData.fhSimable);
}

int EKillJoint_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 0) {
        new EKillJoint(gEventDynamicData.fhSimable);
    }
    return 0;
}

void EKillJoint_ResolveEvent_Callback(void *event, const UGroup *group) {
}
