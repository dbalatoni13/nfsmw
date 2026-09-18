#include "ETirePunctured.hpp"

#include "Speed/Indep/Src/Generated/Messages/MGamePlayMoment.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"

ETirePunctured::ETirePunctured(HSIMABLE phSimable, unsigned int pIndex) : Event(0x10), fhSimable(phSimable), fIndex(pIndex) {
}

ETirePunctured::~ETirePunctured() {
    ISimable *isimable = ISimable::FindInstance(fhSimable);
    ISuspension *suspension;

    if (isimable && isimable->QueryInterface(&suspension)) {

        if (isimable->IsPlayer()) {

            UMath::Vector3 position = suspension->GetWheelPos(fIndex);
            UMath::Vector4 pos4 = UMath::Vector4Make(position, 1.0f);

            MGamePlayMoment(pos4, UMath::Vector4::kZero, UMath::Vector4::kZero, (unsigned int) fhSimable, 0).Send("TireBlo");
        }
    }
}

const char *ETirePunctured::GetEventName() const {
    return "ETirePunctured";
}

void ETirePunctured_MakeEvent_Callback(const void *staticData) {
    new ETirePunctured(((ETirePunctured::StaticData *) staticData)->fhSimable, ((ETirePunctured::StaticData *) staticData)->fIndex);
}
