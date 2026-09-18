#include "ENISWorldAnimTrigger.hpp"

#include <new>

#include "Speed/Indep/Src/Animation/AnimEntity_WorldEntity.hpp"
#include "Speed/Indep/Src/Animation/AnimPlayer.hpp"
#include "Speed/Indep/Src/Animation/AnimWorldScene.hpp"
#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

ENISWorldAnimTrigger::ENISWorldAnimTrigger(const char *pAnimTreeName, float pTimeSet, int pAnimPause, int pAnimHide)
    : Event(0x20), fAnimTreeName(pAnimTreeName), fTimeSet(pTimeSet), fAnimPause(pAnimPause), fAnimHide(pAnimHide) {
    if (fAnimTreeName && bStrLen(fAnimTreeName)) {
        if (TheAnimPlayer.GetWorldAnimScene()) {
            CWorldAnimEntityTree *animTree = TheAnimPlayer.GetWorldAnimScene()->GetAnimTreeFromHash(bStringHash(fAnimTreeName));

            if (animTree) {
                if (fTimeSet >= 0.0f) {
                    animTree->SetTime(fTimeSet);
                }

                if (fAnimHide) {
                    animTree->Stop();
                } else if (fAnimPause) {
                    animTree->Pause();
                } else {
                    animTree->Play();
                }
            }
        }
    }
}

ENISWorldAnimTrigger::~ENISWorldAnimTrigger() {
}

const char *ENISWorldAnimTrigger::GetEventName() const {
    return "ENISWorldAnimTrigger";
}

void ENISWorldAnimTrigger_MakeEvent_Callback(const void *staticData) {
    new ENISWorldAnimTrigger(((ENISWorldAnimTrigger::StaticData *) staticData)->fAnimTreeName, ((ENISWorldAnimTrigger::StaticData *) staticData)->fTimeSet, ((ENISWorldAnimTrigger::StaticData *) staticData)->fAnimPause, ((ENISWorldAnimTrigger::StaticData *) staticData)->fAnimHide);
}

int ENISWorldAnimTrigger_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 4) {
        new ENISWorldAnimTrigger(lua_tostring(L, 1), lua_tonumber(L, 2), (int) lua_tonumber(L, 3), (int) lua_tonumber(L, 4));
    }
    return 0;
}

void ENISWorldAnimTrigger_ResolveEvent_Callback(void *event, const UGroup *group) {
    new (&((ENISWorldAnimTrigger::StaticData *) event)->fAnimTreeName) CARP::TagReference(group);
}
