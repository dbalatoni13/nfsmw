#include "ETriggerMomentNIS.hpp"

#include <new>

#include "Speed/Indep/Src/Animation/AnimPlayer.hpp"
#include "Speed/Indep/Src/Interfaces/SimModels/IModel.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"
#include "Speed/Indep/Src/Sim/Simulation.h"

ETriggerMomentNIS::ETriggerMomentNIS(const char *pSceneName, unsigned int phModel) : Event(0x10), fSceneName(pSceneName), fhModel(phModel) {
}

ETriggerMomentNIS::~ETriggerMomentNIS() {
    IModel *model = IModel::FindInstance((HMODEL) fhModel);

    if (model) {
        HCAUSE hcause = model->GetCausality();
        ICause *causer = ICause::FindInstance(hcause);

        if (causer) {
            ISimable *who;

            if (causer->QueryInterface(&who)) {
                if (who->IsPlayer()) {
                    g_TriggerMomentNISTime = Sim::GetTime();
                }
            }
        }
    }
}

const char *ETriggerMomentNIS::GetEventName() const {
    return "ETriggerMomentNIS";
}

void ETriggerMomentNIS_MakeEvent_Callback(const void *staticData) {
    new ETriggerMomentNIS(((ETriggerMomentNIS::StaticData *) staticData)->fSceneName, gEventDynamicData.fhModel);
}

int ETriggerMomentNIS_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 1) {
        new ETriggerMomentNIS(lua_tostring(L, 1), gEventDynamicData.fhModel);
    }
    return 0;
}

void ETriggerMomentNIS_ResolveEvent_Callback(void *event, const UGroup *group) {
    new (&((ETriggerMomentNIS::StaticData *) event)->fSceneName) CARP::TagReference(group);
}
