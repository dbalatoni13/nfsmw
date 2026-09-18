#include "EMissShift.hpp"

#include "Speed/Indep/Src/World/VehicleRenderConn.h"

EMissShift::EMissShift(HSIMABLE phSimable, float pBonus) : Event(0x10), fhSimable(phSimable), fBonus(pBonus) {
}

EMissShift::~EMissShift() {
    ISimable *isimable = ISimable::FindInstance(fhSimable);

    if (isimable && isimable->IsPlayer()) {
        VehicleRenderConn *connection = VehicleRenderConn::Find(isimable->GetWorldID());

        if (connection) {
            connection->HandleEvent(VehicleRenderConn::E_MISS_SHIFT);
        }
    }
}

const char *EMissShift::GetEventName() const {
    return "EMissShift";
}

void EMissShift_MakeEvent_Callback(const void *staticData) {
    new EMissShift(((EMissShift::StaticData *) staticData)->fhSimable, ((EMissShift::StaticData *) staticData)->fBonus);
}
