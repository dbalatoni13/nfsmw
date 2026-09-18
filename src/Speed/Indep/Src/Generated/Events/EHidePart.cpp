#include "EHidePart.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Interfaces/SimModels/IModel.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"

EHidePart::EHidePart(UCrc32 pPartName, unsigned int phModel) : Event(0x10), fPartName(pPartName), fhModel(phModel) {
}

EHidePart::~EHidePart() {
    IModel *model = IModel::FindInstance((HMODEL) fhModel);

    if (model) {
        model->HidePart(fPartName);
    }
}

const char *EHidePart::GetEventName() const {
    return "EHidePart";
}

void EHidePart_MakeEvent_Callback(const void *staticData) {
    new EHidePart(((EHidePart::StaticData *) staticData)->fPartName, gEventDynamicData.fhModel);
}

int EHidePart_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 1) {
        new EHidePart(UCrc32(lua_tostring(L, 1)), gEventDynamicData.fhModel);
    }
    return 0;
}

void EHidePart_ResolveEvent_Callback(void *event, const UGroup *group) {
}
