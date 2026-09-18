#include "EAccelerate.hpp"

#include "Speed/Indep/Libs/Support/Utility/UVector.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"
#include "Speed/Indep/Src/World/WTrigger.h"

int GetFoundationVideoMode();

EAccelerate::EAccelerate(float pAccelerationX, float pAccelerationY, float pAccelerationZ, int pLocalToObject, int pLocalToTrigger, int pPALOnly,
                         unsigned int phSimable, WTrigger *pTrigger)
    : Event(0x30), fAccelerationX(pAccelerationX), fAccelerationY(pAccelerationY), fAccelerationZ(pAccelerationZ), fLocalToObject(pLocalToObject),
      fLocalToTrigger(pLocalToTrigger), fPALOnly(pPALOnly), fhSimable(phSimable), fTrigger(pTrigger) {
}

EAccelerate::~EAccelerate() {
    HSIMABLE hSimable;
    ISimable *isimable;

    if (!fPALOnly || GetFoundationVideoMode() == 2) {

        hSimable = (HSIMABLE) fhSimable;
        isimable = ISimable::FindInstance(hSimable);

        if (!isimable) {
            return;
        }

        IRigidBody *irigidbody = isimable->GetRigidBody();

        if (irigidbody) {
            UVector3 force(fAccelerationX, fAccelerationY, fAccelerationZ);

            if (fLocalToObject) {

                irigidbody->ConvertLocalToWorld(force, false);
            } else if (fLocalToTrigger && fTrigger) {
                UMath::Matrix4 mat;

                fTrigger->MakeMatrix(mat, false, false);
                force *= mat;
            }

            force *= irigidbody->GetMass();

            irigidbody->ResolveForce(force);
        }
    }
}

const char *EAccelerate::GetEventName() const {
    return "EAccelerate";
}

void EAccelerate_MakeEvent_Callback(const void *staticData) {
    new EAccelerate(((EAccelerate::StaticData *) staticData)->fAccelerationX, ((EAccelerate::StaticData *) staticData)->fAccelerationY,
                    ((EAccelerate::StaticData *) staticData)->fAccelerationZ, ((EAccelerate::StaticData *) staticData)->fLocalToObject,
                    ((EAccelerate::StaticData *) staticData)->fLocalToTrigger, ((EAccelerate::StaticData *) staticData)->fPALOnly,
                    gEventDynamicData.fhSimable, gEventDynamicData.fTrigger);
}

int EAccelerate_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 6) {
        new EAccelerate(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), (int) lua_tonumber(L, 4), (int) lua_tonumber(L, 5),
                        (int) lua_tonumber(L, 6), gEventDynamicData.fhSimable, gEventDynamicData.fTrigger);
    }
    return 0;
}

void EAccelerate_ResolveEvent_Callback(void *event, const UGroup *group) {
}
