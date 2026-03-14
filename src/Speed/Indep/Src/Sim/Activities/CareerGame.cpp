#include "CareerGame.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

UTL::COM::Factory<Sim::Param, Sim::IActivity, UCrc32>::Prototype _CareerGame("CareerGame", CareerGame::Construct);

Sim::IActivity *CareerGame::Construct(Sim::Param params) {
    return new CareerGame(Sim::Param(params));
}

CareerGame::CareerGame(Sim::Param params) : QuickGame(params) {}

CareerGame::~CareerGame() {}
