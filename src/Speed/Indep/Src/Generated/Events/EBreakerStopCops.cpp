#include "EBreakerStopCops.hpp"

#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Generated/Messages/MBreakerStopCops.h"
#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"

EBreakerStopCops::EBreakerStopCops(float pEventRadius, float pDuration, UMath::Vector4 pPosition)
    : Event(0x20), fEventRadius(pEventRadius), fDuration(pDuration), fPosition(pPosition) {
}

EBreakerStopCops::~EBreakerStopCops() {
    UMath::Vector3 position = Vector4To3(fPosition);

    MBreakerStopCops(position, fDuration, fEventRadius).Post(UCrc32("AICopManager"));

    GManager::Get().HidePursuitBreakerIcon(position, 20.0f);
}

const char *EBreakerStopCops::GetEventName() const {
    return "EBreakerStopCops";
}

void EBreakerStopCops_MakeEvent_Callback(const void *staticData) {
    new EBreakerStopCops(((EBreakerStopCops::StaticData *) staticData)->fEventRadius, ((EBreakerStopCops::StaticData *) staticData)->fDuration,
                         gEventDynamicData.fPosition);
}

int EBreakerStopCops_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 2) {
        new EBreakerStopCops(lua_tonumber(L, 1), lua_tonumber(L, 2), gEventDynamicData.fPosition);
    }
    return 0;
}

void EBreakerStopCops_ResolveEvent_Callback(void *event, const UGroup *group) {
}
