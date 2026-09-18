#include "ECinematicMoment.hpp"

#include <new>

#include "Speed/Indep/Src/Camera/CameraAI.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Lua/source/lua.h"

ECinematicMoment::ECinematicMoment(const char *pGenericGroupName, const char *pGenericTrackName, float pEventRadius)
    : Event(0x20), fGenericGroupName(pGenericGroupName), fGenericTrackName(pGenericTrackName), fEventRadius(pEventRadius) {
}

ECinematicMoment::~ECinematicMoment() {
    if (IPlayer::Count(PLAYER_ALL) <= 1) {
        IPlayer *player = IPlayer::First(PLAYER_LOCAL);

        if (player) {
            TheICEManager.SetGenericCameraToPlay(fGenericGroupName, fGenericTrackName);
            CameraAI::SetAction(EVIEW_PLAYER1, "CDActionIce");
        }
    }
}

const char *ECinematicMoment::GetEventName() const {
    return "ECinematicMoment";
}

void ECinematicMoment_MakeEvent_Callback(const void *staticData) {
    new ECinematicMoment(((ECinematicMoment::StaticData *) staticData)->fGenericGroupName, ((ECinematicMoment::StaticData *) staticData)->fGenericTrackName, ((ECinematicMoment::StaticData *) staticData)->fEventRadius);
}

int ECinematicMoment_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 3) {
        new ECinematicMoment(lua_tostring(L, 1), lua_tostring(L, 2), lua_tonumber(L, 3));
    }
    return 0;
}

void ECinematicMoment_ResolveEvent_Callback(void *event, const UGroup *group) {
    new (&((ECinematicMoment::StaticData *) event)->fGenericGroupName) CARP::TagReference(group);
    new (&((ECinematicMoment::StaticData *) event)->fGenericTrackName) CARP::TagReference(group);
}
