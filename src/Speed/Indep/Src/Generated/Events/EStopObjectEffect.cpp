#include "EStopObjectEffect.hpp"

#include "Speed/Indep/Src/Interfaces/SimModels/IModel.h"
#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"

EStopObjectEffect::EStopObjectEffect(UCrc32 pID, unsigned int phModel) : Event(0x10), fID(pID), fhModel(phModel) {
}

EStopObjectEffect::~EStopObjectEffect() {
    if (fID == UCrc32::kNull) {
        return;
    }

    IModel *iowner = IModel::FindInstance((HMODEL) fhModel);

    if (iowner) {
        iowner->StopEffect(fID);
    }
}

const char *EStopObjectEffect::GetEventName() const {
    return "EStopObjectEffect";
}

void EStopObjectEffect_MakeEvent_Callback(const void *staticData) {
    new EStopObjectEffect(((EStopObjectEffect::StaticData *) staticData)->fID, gEventDynamicData.fhModel);
}

int EStopObjectEffect_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 1) {
        new EStopObjectEffect(UCrc32(lua_tostring(L, 1)), gEventDynamicData.fhModel);
    }
    return 0;
}

void EStopObjectEffect_ResolveEvent_Callback(void *event, const UGroup *group) {
}
