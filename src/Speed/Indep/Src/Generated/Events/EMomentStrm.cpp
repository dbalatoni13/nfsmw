#include "EMomentStrm.hpp"

#include <new>

#include "Speed/Indep/Src/Generated/Messages/MGamePlayMoment.h"
#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"

EMomentStrm::EMomentStrm(UMath::Vector4 pPosition, UMath::Vector4 pVector, UMath::Vector4 pVelocity, uintptr_t phSimable, const char *pAttribStrm,
                         unsigned int pAttribKey)
    : Event(0x50), fPosition(pPosition), fVector(pVector), fVelocity(pVelocity), fhSimable(phSimable), fAttribStrm(pAttribStrm),
      fAttribKey(pAttribKey) {
}

EMomentStrm::~EMomentStrm() {
    if (fAttribKey == 0 && fAttribStrm) {

        fAttribKey = Attrib::StringToKey(fAttribStrm);
    }

    MGamePlayMoment(fPosition, fVector, fVelocity, fhSimable, fAttribKey).Send("MomentStrm");
}

const char *EMomentStrm::GetEventName() const {
    return "EMomentStrm";
}

void EMomentStrm_MakeEvent_Callback(const void *staticData) {
    new EMomentStrm(gEventDynamicData.fPosition, gEventDynamicData.fVector, gEventDynamicData.fVelocity, gEventDynamicData.fhSimable,
                    ((EMomentStrm::StaticData *) staticData)->fAttribStrm, ((EMomentStrm::StaticData *) staticData)->fAttribKey);
}

int EMomentStrm_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 2) {
        new EMomentStrm(gEventDynamicData.fPosition, gEventDynamicData.fVector, gEventDynamicData.fVelocity, gEventDynamicData.fhSimable,
                        lua_tostring(L, 1), (unsigned int) lua_tostring(L, 2));
    }
    return 0;
}

void EMomentStrm_ResolveEvent_Callback(void *event, const UGroup *group) {
    new (&((EMomentStrm::StaticData *) event)->fAttribStrm) CARP::TagReference(group);
}
