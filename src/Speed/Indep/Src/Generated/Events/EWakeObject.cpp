#include "EWakeObject.hpp"

#include "Speed/Indep/Src/Interfaces/SimModels/IModel.h"
#include "Speed/Indep/Src/Interfaces/SimModels/ISceneryModel.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"

EWakeObject::EWakeObject(unsigned int phModel) : Event(0x10), fhModel(phModel) {
}

EWakeObject::~EWakeObject() {
    IModel *model = IModel::FindInstance((HMODEL) fhModel);

    if (model) {
        ISceneryModel *iscenery;
        IVehicle *ivehicle;

        if (model->QueryInterface(&iscenery)) {
            iscenery->WakeUp();
        } else if (model->GetSimable() && model->GetSimable()->QueryInterface(&ivehicle)) {
            ivehicle->Activate();
        }
    }
}

const char *EWakeObject::GetEventName() const {
    return "EWakeObject";
}

void EWakeObject_MakeEvent_Callback(const void *staticData) {
    new EWakeObject(gEventDynamicData.fhModel);
}

int EWakeObject_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 0) {
        new EWakeObject(gEventDynamicData.fhModel);
    }
    return 0;
}

void EWakeObject_ResolveEvent_Callback(void *event, const UGroup *group) {
}
