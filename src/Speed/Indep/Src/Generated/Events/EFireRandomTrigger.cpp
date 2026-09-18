#include "EFireRandomTrigger.hpp"

#include <new>

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Render/Common/RRandom.h"
#include "Speed/Indep/Src/World/WTrigger.h"

EFireRandomTrigger::EFireRandomTrigger(CARP::Trigger *pTrigger1, float pChance1, CARP::Trigger *pTrigger2, float pChance2, CARP::Trigger *pTrigger3,
                                       float pChance3, CARP::Trigger *pTrigger4, float pChance4, unsigned int phSimable)
    : Event(0x30), fTrigger1(pTrigger1), fChance1(pChance1), fTrigger2(pTrigger2), fChance2(pChance2), fTrigger3(pTrigger3), fChance3(pChance3),
      fTrigger4(pTrigger4), fChance4(pChance4), fhSimable(phSimable) {
}

EFireRandomTrigger::~EFireRandomTrigger() {
    CARP::Trigger *fTrigger = NULL;

    float cum1 = fChance1;
    float cum2 = fChance2 + cum1;
    float cum3 = fChance3 + cum2;
    float cum4 = fChance4 + cum3;

    float amount = RRandom::FloatRange(100.0f);

    if (amount <= cum1) {
        fTrigger = fTrigger1;
    } else if (amount <= cum2) {
        fTrigger = fTrigger2;
    } else if (amount <= cum3) {
        fTrigger = fTrigger3;
    } else if (amount <= cum4) {
        fTrigger = fTrigger4;
    }

    if (fTrigger) {
        static_cast<WTrigger *>(fTrigger)->FireEvents((HSIMABLE) fhSimable);
    }
}

const char *EFireRandomTrigger::GetEventName() const {
    return "EFireRandomTrigger";
}

void EFireRandomTrigger_MakeEvent_Callback(const void *staticData) {
    new EFireRandomTrigger(((EFireRandomTrigger::StaticData *) staticData)->fTrigger1, ((EFireRandomTrigger::StaticData *) staticData)->fChance1,
                           ((EFireRandomTrigger::StaticData *) staticData)->fTrigger2, ((EFireRandomTrigger::StaticData *) staticData)->fChance2,
                           ((EFireRandomTrigger::StaticData *) staticData)->fTrigger3, ((EFireRandomTrigger::StaticData *) staticData)->fChance3,
                           ((EFireRandomTrigger::StaticData *) staticData)->fTrigger4, ((EFireRandomTrigger::StaticData *) staticData)->fChance4,
                           gEventDynamicData.fhSimable);
}

int EFireRandomTrigger_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 8) {
        new EFireRandomTrigger((CARP::Trigger *) lua_tostring(L, 1), lua_tonumber(L, 2), (CARP::Trigger *) lua_tostring(L, 3), lua_tonumber(L, 4),
                               (CARP::Trigger *) lua_tostring(L, 5), lua_tonumber(L, 6), (CARP::Trigger *) lua_tostring(L, 7), lua_tonumber(L, 8),
                               gEventDynamicData.fhSimable);
    }
    return 0;
}

void EFireRandomTrigger_ResolveEvent_Callback(void *event, const UGroup *group) {
    new (&((EFireRandomTrigger::StaticData *) event)->fTrigger1) CARP::TagReference(group);
    new (&((EFireRandomTrigger::StaticData *) event)->fTrigger2) CARP::TagReference(group);
    new (&((EFireRandomTrigger::StaticData *) event)->fTrigger3) CARP::TagReference(group);
    new (&((EFireRandomTrigger::StaticData *) event)->fTrigger4) CARP::TagReference(group);
}
