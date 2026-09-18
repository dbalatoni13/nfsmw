#include "ESimulate.hpp"

#include "Speed/Indep/Src/Sim/Simulation.h"

ESimulate::ESimulate(UTL::COM::IUnknown *pSim) : Event(0x10), fSim(pSim) {
}

ESimulate::~ESimulate() {
    Sim::Update();
}

const char *ESimulate::GetEventName() const {
    return "ESimulate";
}

void ESimulate_MakeEvent_Callback(const void *staticData) {
    new ESimulate(((ESimulate::StaticData *) staticData)->fSim);
}
