#include "ECollision.hpp"

#include "Speed/Indep/Src/Interfaces/Simables/ICause.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"

ECollision::ECollision(COLLISION_INFO pInfo) : Event(0x90), fInfo(pInfo) {
}

ECollision::~ECollision() {
    ISimable *bodyA = ISimable::FindInstance(fInfo.objA);

    if (bodyA) {
        ISimable *bodyB = fInfo.type == Sim::Collision::Info::OBJECT ? ISimable::FindInstance(fInfo.objB) : NULL;

        Sim::Collision::Info cinfo = fInfo;

        Sim::Collision::Respond(cinfo);

        if (cinfo.type == Sim::Collision::Info::OBJECT && bodyA && bodyB) {
            ISimable *causer = NULL;
            ISimable *causee = NULL;
            HCAUSE hrootcause = NULL;

            HCAUSE hcauseA = bodyA->GetCausality();
            HCAUSE hcauseB = bodyB->GetCausality();

            if (!hcauseA) {
                if (hcauseB) {
                    causer = bodyB;
                    causee = bodyA;

                    hrootcause = hcauseB;
                }
            } else if (!hcauseB) {

                causer = bodyA;
                causee = bodyB;

                hrootcause = hcauseA;
            }

            if (causer && causee) {
                ICause *irootcause = ICause::FindInstance(hrootcause);

                if (irootcause) {

                    irootcause->OnCausedCollision(cinfo, causer, causee);
                }
            }
        }
    }
}

const char *ECollision::GetEventName() const {
    return "ECollision";
}
