#include "ESetPlayerCarReset.hpp"

#include <new>

#include "Speed/Indep/Src/Interfaces/Simables/IResetable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Lua/source/lua.h"

ESetPlayerCarReset::ESetPlayerCarReset(int pUseTrigger, CARP::Trigger *pTrigger) : Event(0x10), fUseTrigger(pUseTrigger), fTrigger(pTrigger) {
}

ESetPlayerCarReset::~ESetPlayerCarReset() {
    UMath::Vector3 pos;
    UMath::Vector3 vec;
    IVehicle *ivehicle = IVehicle::First(VEHICLE_PLAYERS);
    IResetable *ireset;

    if (ivehicle) {
        if (ivehicle->QueryInterface(&ireset)) {
            ISimable *isimable = ivehicle->GetSimable();

            if (!fUseTrigger) {
                UMath::Matrix4 mat;

                isimable->GetTransform(mat);
                ireset->SetResetPosition(Vector4To3(mat.v3), Vector4To3(mat.v2));
            } else if (fTrigger) {
                pos = Vector4To3(fTrigger->fPosRadius);
                pos.y += 1.0f;
                vec = Vector4To3(fTrigger->fMatRow2Length);
                ireset->SetResetPosition(pos, vec);
            }
        }
    }
}

const char *ESetPlayerCarReset::GetEventName() const {
    return "ESetPlayerCarReset";
}

void ESetPlayerCarReset_MakeEvent_Callback(const void *staticData) {
    new ESetPlayerCarReset(((ESetPlayerCarReset::StaticData *) staticData)->fUseTrigger, ((ESetPlayerCarReset::StaticData *) staticData)->fTrigger);
}

int ESetPlayerCarReset_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 2) {
        new ESetPlayerCarReset((int) lua_tonumber(L, 1), (CARP::Trigger *) lua_tostring(L, 2));
    }
    return 0;
}

void ESetPlayerCarReset_ResolveEvent_Callback(void *event, const UGroup *group) {
    new (&((ESetPlayerCarReset::StaticData *) event)->fTrigger) CARP::TagReference(group);
}
