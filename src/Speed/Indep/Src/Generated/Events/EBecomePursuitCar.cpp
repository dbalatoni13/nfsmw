#include "EBecomePursuitCar.hpp"

#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"

EBecomePursuitCar::EBecomePursuitCar(unsigned int phSimable) : Event(0x10), fhSimable(phSimable) {
}

EBecomePursuitCar::~EBecomePursuitCar() {
    ISimable *isimable = NULL;

    if (fhSimable) {
        isimable = ISimable::FindInstance((HSIMABLE) fhSimable);
    }

    if (isimable) {
        IPursuitAI *ipv;

        if (isimable->QueryInterface(&ipv)) {
            ipv->EndPursuit();

            ISimable *targetSimable = IPlayer::First(PLAYER_LOCAL)->GetSimable();

            ipv->StartPursuit(NULL, targetSimable);
        }
    }
}

const char *EBecomePursuitCar::GetEventName() const {
    return "EBecomePursuitCar";
}

void EBecomePursuitCar_MakeEvent_Callback(const void *staticData) {
    new EBecomePursuitCar(gEventDynamicData.fhSimable);
}

int EBecomePursuitCar_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 0) {
        new EBecomePursuitCar(gEventDynamicData.fhSimable);
    }
    return 0;
}

void EBecomePursuitCar_ResolveEvent_Callback(void *event, const UGroup *group) {
}
