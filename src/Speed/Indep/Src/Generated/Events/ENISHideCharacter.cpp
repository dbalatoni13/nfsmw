#include "ENISHideCharacter.hpp"

#include <new>

#include "Speed/Indep/Src/Animation/AnimEntity.hpp"
#include "Speed/Indep/Src/Animation/AnimScene.hpp"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

ENISHideCharacter::ENISHideCharacter(const char *pAnimModelName, int pAnimShow) : Event(0x10), fAnimModelName(pAnimModelName), fAnimShow(pAnimShow) {
    if (fAnimModelName && bStrLen(fAnimModelName) && INIS::Get() && INIS::Get()->GetAnimScene()) {
        CAnimScene *aScene = INIS::Get()->GetAnimScene();
        IAnimEntity *aEntity = aScene->GetAnimEntityWithModelName(fAnimModelName);

        if (aEntity && aEntity->GetWorldModel()) {
            aEntity->GetWorldModel()->SetEnabledFlag(fAnimShow != 0);
        }
    }
}

ENISHideCharacter::~ENISHideCharacter() {
}

const char *ENISHideCharacter::GetEventName() const {
    return "ENISHideCharacter";
}

void ENISHideCharacter_MakeEvent_Callback(const void *staticData) {
    new ENISHideCharacter(((ENISHideCharacter::StaticData *) staticData)->fAnimModelName, ((ENISHideCharacter::StaticData *) staticData)->fAnimShow);
}

int ENISHideCharacter_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 2) {
        new ENISHideCharacter(lua_tostring(L, 1), (int) lua_tonumber(L, 2));
    }
    return 0;
}

void ENISHideCharacter_ResolveEvent_Callback(void *event, const UGroup *group) {
    new (&((ENISHideCharacter::StaticData *) event)->fAnimModelName) CARP::TagReference(group);
}
