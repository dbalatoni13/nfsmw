#include "EDynamicRegion.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"
#include "Speed/Indep/Src/World/TrackPath.hpp"

EDynamicRegion::EDynamicRegion(int pOn, UMath::Vector4 pPosition, UMath::Vector4 pVector, UMath::Vector4 pVelocity, unsigned int phSimable)
    : Event(0x40), fOn(pOn), fPosition(pPosition), fVector(pVector), fVelocity(pVelocity), fhSimable(phSimable) {
}

EDynamicRegion::~EDynamicRegion() {
    bVector3 vpos;

    vpos.x = fPosition.z;
    vpos.y = -fPosition.x;
    vpos.z = fPosition.y;

    bVector2 pos2d(vpos.x, vpos.y);

    TrackPathZone *zone = NULL;

    while ((zone = TheTrackPathManager.FindZone(&pos2d, TRACK_PATH_ZONE_DYNAMIC, zone)) != NULL) {
        zone->SetVisitInfo(fOn != 0);
    }
}

const char *EDynamicRegion::GetEventName() const {
    return "EDynamicRegion";
}

void EDynamicRegion_MakeEvent_Callback(const void *staticData) {
    new EDynamicRegion(((EDynamicRegion::StaticData *) staticData)->fOn, gEventDynamicData.fPosition, gEventDynamicData.fVector,
                       gEventDynamicData.fVelocity, gEventDynamicData.fhSimable);
}

int EDynamicRegion_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 1) {
        new EDynamicRegion((int) lua_tonumber(L, 1), gEventDynamicData.fPosition, gEventDynamicData.fVector, gEventDynamicData.fVelocity,
                           gEventDynamicData.fhSimable);
    }
    return 0;
}

void EDynamicRegion_ResolveEvent_Callback(void *event, const UGroup *group) {
}
