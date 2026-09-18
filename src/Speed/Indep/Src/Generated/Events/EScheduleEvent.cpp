#include "Speed/Indep/Src/Main/Scheduler.h"
#include "Speed/Indep/Src/Generated/Events/EScheduleEventUpdate.hpp"

#include "EScheduleEvent.hpp"

#include <new>

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Sim/Simulation.h"

EScheduleEvent::EScheduleEvent(CARP::EventList *pEventList, float pTimeDelay) : Event(0x10), fEventList(pEventList), fTimeDelay(pTimeDelay) {
}

EScheduleEvent::~EScheduleEvent() {
    EScheduleEventUpdate::StaticData *staticData = (EScheduleEventUpdate::StaticData *) gFastMem.Alloc(sizeof(EScheduleEventUpdate::StaticData), NULL);

    staticData->fStaticData = staticData;
    staticData->fEventID = 0xC945BC1F;
    staticData->fEventList = fEventList;
    staticData->fExecuteOnSimTime = fTimeDelay + Sim::GetTime();
    staticData->fResetCount = 0;
    staticData->fTaskHandle = Scheduler::Get().fSchedule_SimRate->AddTask(0xC945BC1F, staticData, 0, true, 0, 0);
}

const char *EScheduleEvent::GetEventName() const {
    return "EScheduleEvent";
}

void EScheduleEvent_MakeEvent_Callback(const void *staticData) {
    new EScheduleEvent(((EScheduleEvent::StaticData *) staticData)->fEventList, ((EScheduleEvent::StaticData *) staticData)->fTimeDelay);
}

int EScheduleEvent_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 2) {
        new EScheduleEvent((CARP::EventList *) lua_tostring(L, 1), lua_tonumber(L, 2));
    }
    return 0;
}

void EScheduleEvent_ResolveEvent_Callback(void *event, const UGroup *group) {
    new (&((EScheduleEvent::StaticData *) event)->fEventList) CARP::TagReference(group);
}
