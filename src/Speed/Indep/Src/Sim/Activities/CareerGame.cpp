#include "CareerGame.h"

Sim::IActivity *CareerGame::Construct(Sim::Param params) {
    return new CareerGame(Sim::Param(params));
}

CareerGame::CareerGame(Sim::Param params) : QuickGame(params) {}

CareerGame::~CareerGame() {}
