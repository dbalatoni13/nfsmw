#include "ERaceSheetOff.hpp"

#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"

ERaceSheetOff::ERaceSheetOff() : Event(0x10) {
}

ERaceSheetOff::~ERaceSheetOff() {
    cFEng::Get()->QueuePackagePop(0);
}

const char *ERaceSheetOff::GetEventName() const {
    return "ERaceSheetOff";
}

void ERaceSheetOff_MakeEvent_Callback(const void *staticData) {
    new ERaceSheetOff();
}
