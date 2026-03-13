#include "Speed/Indep/Src/AI/AIRoadBlock.h"

#include "Speed/Indep/Src/Interfaces/SimModels/IPlaceableScenery.h"

Sim::IActivity *AIRoadBlock::Construct(Sim::Param params) {
    AIRoadBlock *rb = new AIRoadBlock(params);
    if (rb) {
        return static_cast<Sim::IActivity *>(rb);
    }
    return nullptr;
}

bool AIRoadBlock::AddVehicle(IVehicle *vehicle) {
    bool result = Attach(vehicle);
    IPursuitAI *ipv;
    if (vehicle->QueryInterface(&ipv)) {
        ipv->StartRoadBlock();
    }
    return result;
}

bool AIRoadBlock::RemoveVehicle(IVehicle *vehicle) {
    return Detach(vehicle);
}

void AIRoadBlock::ReleaseAllSmackables() {
    for (IPlaceableScenery *const *iter = RoadblockSmackableList.begin();
         iter != RoadblockSmackableList.end(); ++iter) {
        IPlaceableScenery *smckble = *iter;
        smckble->Destroy();
    }
    RoadblockSmackableList.clear();
    mNumSpikeStrips = 0;
}

int AIRoadBlock::GetNumCops() {
    return VehicleList.size();
}

void AIRoadBlock::SetPursuit(IPursuit *pursuit) {
    if (Pursuit != pursuit) {
        if (Pursuit) {
            Detach(Pursuit);
            Pursuit = nullptr;
        }
        Attach(pursuit);
        Pursuit = pursuit;
    }
}
