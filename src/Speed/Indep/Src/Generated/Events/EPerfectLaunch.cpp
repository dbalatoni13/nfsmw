#include "EPerfectLaunch.hpp"

#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/World/VehicleRenderConn.h"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"

EPerfectLaunch::EPerfectLaunch(HSIMABLE phSimable, float pBonus) : Event(0x10), fhSimable(phSimable), fBonus(pBonus) {
}

EPerfectLaunch::~EPerfectLaunch() {
    ISimable *isimable = ISimable::FindInstance(fhSimable);

    if (isimable) {
        if (isimable->IsPlayer()) {
            VehicleRenderConn *connection = VehicleRenderConn::Find(isimable->GetWorldID());

            if (connection) {
                connection->HandleEvent(VehicleRenderConn::E_PERFECT_LAUNCH);
            }

            IHud *hud = isimable->GetPlayer()->GetHud();

            if (hud) {
                IGenericMessage *igenericmessage;

                if (hud->QueryInterface(&igenericmessage)) {
                    igenericmessage->RequestGenericMessage(GetTranslatedString(0xb1e2e5c2), false, FEHASH_ZOOMINGREEN, 0, 0, GenericMessage_Priority_3);
                }
            }
        }
    }
}

const char *EPerfectLaunch::GetEventName() const {
    return "EPerfectLaunch";
}

void EPerfectLaunch_MakeEvent_Callback(const void *staticData) {
    new EPerfectLaunch(((EPerfectLaunch::StaticData *) staticData)->fhSimable, ((EPerfectLaunch::StaticData *) staticData)->fBonus);
}
