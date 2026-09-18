#include "EGTriggerInternal.hpp"

#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GTrigger.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"

EGTriggerInternal::EGTriggerInternal(unsigned int phSimable, int pTriggerStimulus, unsigned int pTriggerKey)
    : Event(0x20), fhSimable(phSimable), fTriggerStimulus(pTriggerStimulus), fTriggerKey(pTriggerKey) {
    if (fhSimable) {
        ISimable *isim = ISimable::FindInstance((HSIMABLE) fhSimable);

        if (isim) {
            ITrafficAI *itraffic;

            if (!isim->QueryInterface(&itraffic)) {
                GRuntimeInstance *triggerInst = GManager::Get().FindInstance(fTriggerKey);

                if (triggerInst) {
                    GTrigger *trigger = static_cast<GTrigger *>(triggerInst);

                    trigger->NotifySimableTrigger(isim, fTriggerStimulus);
                }
            }
        }
    }
}

EGTriggerInternal::~EGTriggerInternal() {
}

const char *EGTriggerInternal::GetEventName() const {
    return "EGTriggerInternal";
}

void EGTriggerInternal_MakeEvent_Callback(const void *staticData) {
    new EGTriggerInternal(gEventDynamicData.fhSimable, gEventDynamicData.fTriggerStimulus, ((EGTriggerInternal::StaticData *) staticData)->fTriggerKey);
}
