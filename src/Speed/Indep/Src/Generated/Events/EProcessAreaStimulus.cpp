#include "Speed/Indep/Src/Generated/Events/EProcessStimulus.hpp"

#include "EProcessAreaStimulus.hpp"

#include "Speed/Indep/Src/Interfaces/SimModels/IModel.h"
#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"
#include "Speed/Indep/Src/Main/EventSequencer.h"

EProcessAreaStimulus::EProcessAreaStimulus(UCrc32 pSystemName, UCrc32 pStimulus, UCrc32 pQueueMode, float pRadius, UMath::Vector4 pPosition)
    : Event(0x30), fSystemName(pSystemName), fStimulus(pStimulus), fQueueMode(pQueueMode), fRadius(pRadius), fPosition(pPosition) {
}

EProcessAreaStimulus::~EProcessAreaStimulus() {
    float r2 = fRadius * fRadius;
    UMath::Vector4 pos = fPosition;

    for (IModel::List::const_iterator iter = IModel::GetList().begin(); iter != IModel::GetList().end(); ++iter) {

        IModel *model = *iter;

        EventSequencer::IEngine *iengine = model->GetEventSequencer();

        if (iengine) {
            UMath::Matrix4 m;

            model->GetTransform(m);

            if (UMath::DistanceSquarexyz(m.v3, pos) < r2) {
                new EProcessStimulus(fSystemName, fStimulus, fQueueMode, (unsigned int) iengine->GetInstanceHandle(), 0);
            }
        }
    }
}

const char *EProcessAreaStimulus::GetEventName() const {
    return "EProcessAreaStimulus";
}

void EProcessAreaStimulus_MakeEvent_Callback(const void *staticData) {
    new EProcessAreaStimulus(((EProcessAreaStimulus::StaticData *) staticData)->fSystemName,
                             ((EProcessAreaStimulus::StaticData *) staticData)->fStimulus,
                             ((EProcessAreaStimulus::StaticData *) staticData)->fQueueMode,
                             ((EProcessAreaStimulus::StaticData *) staticData)->fRadius, gEventDynamicData.fPosition);
}

int EProcessAreaStimulus_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 4) {
        new EProcessAreaStimulus(UCrc32(lua_tostring(L, 1)), UCrc32(lua_tostring(L, 2)), UCrc32(lua_tostring(L, 3)), lua_tonumber(L, 4),
                                 gEventDynamicData.fPosition);
    }
    return 0;
}

void EProcessAreaStimulus_ResolveEvent_Callback(void *event, const UGroup *group) {
}
