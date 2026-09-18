#include "EWorldMapOff.hpp"

#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"

EWorldMapOff::EWorldMapOff() : Event(0x10) {
}

EWorldMapOff::~EWorldMapOff() {
    if (cFEng::Get()->IsPackagePushed("WorldMapMain.fng")) {
        cFEng::Get()->QueuePackagePop(0);
    }
}

const char *EWorldMapOff::GetEventName() const {
    return "EWorldMapOff";
}

void EWorldMapOff_MakeEvent_Callback(const void *staticData) {
    new EWorldMapOff();
}
