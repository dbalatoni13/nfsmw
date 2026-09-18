#include "EPlayerShift.hpp"

#include "Speed/Indep/Src/World/VehicleRenderConn.h"

EPlayerShift::EPlayerShift(HSIMABLE phSimable, ShiftStatus pStatus, bool pAutomatic, GearID pFrom, GearID pTo)
    : Event(0x20), fhSimable(phSimable), fStatus(pStatus), fAutomatic(pAutomatic), fFrom(pFrom), fTo(pTo) {
}

EPlayerShift::~EPlayerShift() {
    ISimable *isimable = ISimable::FindInstance(fhSimable);

    if (isimable) {
        if (isimable->IsPlayer()) {
            VehicleRenderConn *connection = VehicleRenderConn::Find(isimable->GetWorldID());

            if (connection) {
                if (fTo != G_NEUTRAL) {
                    if (fTo > fFrom) {
                        connection->HandleEvent(VehicleRenderConn::E_UPSHIFT);
                    } else {
                        connection->HandleEvent(VehicleRenderConn::E_DOWNSHIFT);
                    }
                }
            }
        }
    }
}

const char *EPlayerShift::GetEventName() const {
    return "EPlayerShift";
}

void EPlayerShift_MakeEvent_Callback(const void *staticData) {
    new EPlayerShift(((EPlayerShift::StaticData *) staticData)->fhSimable, ((EPlayerShift::StaticData *) staticData)->fStatus, ((EPlayerShift::StaticData *) staticData)->fAutomatic, ((EPlayerShift::StaticData *) staticData)->fFrom, ((EPlayerShift::StaticData *) staticData)->fTo);
}
