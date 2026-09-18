#include "ENISOverlayMessage.hpp"

#include <new>

#include "Speed/Indep/Src/FEng/FEList.h"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

ENISOverlayMessage::ENISOverlayMessage(const char *pOverlayName, const char *pOverlayMessage) : Event(0x10), fOverlayName(pOverlayName), fOverlayMessage(pOverlayMessage) {
    if (fOverlayName && fOverlayMessage && bStrLen(fOverlayName) && bStrLen(fOverlayMessage)) {
        cFEng *feng = cFEng::Get();

        if (feng) {
            feng->QueuePackageMessage(FEHashUpper(fOverlayMessage), fOverlayName, NULL);
        }
    }
}

ENISOverlayMessage::~ENISOverlayMessage() {
}

const char *ENISOverlayMessage::GetEventName() const {
    return "ENISOverlayMessage";
}

void ENISOverlayMessage_MakeEvent_Callback(const void *staticData) {
    new ENISOverlayMessage(((ENISOverlayMessage::StaticData *) staticData)->fOverlayName, ((ENISOverlayMessage::StaticData *) staticData)->fOverlayMessage);
}

int ENISOverlayMessage_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 2) {
        new ENISOverlayMessage(lua_tostring(L, 1), lua_tostring(L, 2));
    }
    return 0;
}

void ENISOverlayMessage_ResolveEvent_Callback(void *event, const UGroup *group) {
    new (&((ENISOverlayMessage::StaticData *) event)->fOverlayName) CARP::TagReference(group);
    new (&((ENISOverlayMessage::StaticData *) event)->fOverlayMessage) CARP::TagReference(group);
}
