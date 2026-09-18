#include "Speed/Indep/Src/Generated/Events/EQuitToFE.hpp"

#include "EQuitDemo.hpp"

#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Misc/DemoDisc.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"

EQuitDemo::EQuitDemo(DemoDiscEndReason pEndDemoReason) : Event(0x10), fEndDemoReason(pEndDemoReason) {
    if (cFEng::Get()->IsErrorState()) {
        cFEng::Get()->PopErrorPackage();
    }

    TheDemoDiscManager.SetEndReason(fEndDemoReason);

    if (TheGameFlowManager.IsInFrontend()) {
        TheGameFlowManager.UnloadFrontend();
    } else if (TheGameFlowManager.IsInGame()) {
        new EQuitToFE(GARAGETYPE_MAIN_FE, NULL);
    }
}

EQuitDemo::~EQuitDemo() {
}

const char *EQuitDemo::GetEventName() const {
    return "EQuitDemo";
}

void EQuitDemo_MakeEvent_Callback(const void *staticData) {
    new EQuitDemo(((EQuitDemo::StaticData *) staticData)->fEndDemoReason);
}
