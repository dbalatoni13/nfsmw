#include "ELoadLost.hpp"

#include "Speed/Indep/Src/Generated/Messages/MSetTrafficSpeed.h"
#include "Speed/Indep/Src/Interfaces/IAttachable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"
#include "Speed/Indep/Src/Physics/PVehicle.h"

ELoadLost::ELoadLost(unsigned int phSimable) : Event(0x10), fhSimable(phSimable) {
}

ELoadLost::~ELoadLost() {
    ISimable *object = ISimable::FindInstance((HSIMABLE) fhSimable);

    if (object) {
        const IAttachable::List *attachements = object->GetAttachments();

        if (attachements) {
            for (IAttachable::List::const_iterator iter = attachements->begin(); iter != attachements->end(); ++iter) {
                IAttachable *ia = *iter;
                IVehicle *tractor;

                if (ia->QueryInterface(&tractor)) {
                    if (tractor->GetVehicleClass() == VehicleClass::TRACTOR) {
                        MSetTrafficSpeed ai_msg(0.0f, 0.0f, 1);

                        ai_msg.SetID(tractor->GetSimable()->GetWorldID());
                        ai_msg.Post(UCrc32("AIAction"));
                    }
                }
            }
        }
    }
}

const char *ELoadLost::GetEventName() const {
    return "ELoadLost";
}

void ELoadLost_MakeEvent_Callback(const void *staticData) {
    new ELoadLost(gEventDynamicData.fhSimable);
}

int ELoadLost_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 0) {
        new ELoadLost(gEventDynamicData.fhSimable);
    }
    return 0;
}

void ELoadLost_ResolveEvent_Callback(void *event, const UGroup *group) {
}
