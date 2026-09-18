#include "EPerfectShift.hpp"

#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/World/VehicleRenderConn.h"

EPerfectShift::EPerfectShift(HSIMABLE phSimable, float pBonus) : Event(0x10), fhSimable(phSimable), fBonus(pBonus) {
}

EPerfectShift::~EPerfectShift() {
    ISimable *isimable = ISimable::FindInstance(fhSimable);

    if (isimable && isimable->IsPlayer()) {
        VehicleRenderConn *connection = VehicleRenderConn::Find(isimable->GetWorldID());

        if (connection) {
            connection->HandleEvent(VehicleRenderConn::E_PERFECT_SHIFT);
        }

        GRacerInfo *racerInfo = GRaceStatus::Get().GetRacerInfo(isimable);

        if (racerInfo) {
            racerInfo->IncPerfectShifts();
        }
    }
}

const char *EPerfectShift::GetEventName() const {
    return "EPerfectShift";
}

void EPerfectShift_MakeEvent_Callback(const void *staticData) {
    new EPerfectShift(((EPerfectShift::StaticData *) staticData)->fhSimable, ((EPerfectShift::StaticData *) staticData)->fBonus);
}
