#include "EHideObject.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Interfaces/SimModels/IModel.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"

EHideObject::EHideObject(unsigned int phModel) : Event(0x10), fhModel(phModel) {
}

EHideObject::~EHideObject() {
    IModel *model = IModel::FindInstance((HMODEL) fhModel);

    if (model) {
        model->HideModel();
    }
}

const char *EHideObject::GetEventName() const {
    return "EHideObject";
}

void EHideObject_MakeEvent_Callback(const void *staticData) {
    new EHideObject(gEventDynamicData.fhModel);
}

int EHideObject_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 0) {
        new EHideObject(gEventDynamicData.fhModel);
    }
    return 0;
}

void EHideObject_ResolveEvent_Callback(void *event, const UGroup *group) {
}
