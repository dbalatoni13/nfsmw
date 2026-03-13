#include "Speed/Indep/Src/AI/AIRoadBlock.h"

#include "Speed/Indep/Src/Interfaces/SimModels/IPlaceableScenery.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"

AIRoadBlock::AIRoadBlock(Sim::Param params)
    : Activity(1) //
    , IRoadBlock(this) //
    , VehicleList() //
    , RoadblockSmackableList() //
    , Pursuit(nullptr)
    , mDodged(false)
    , mNumCopsDamaged(0)
    , mNumCopsDestroyed(0)
    , mNumSpikeStrips(0)
    , mPerpCheatTime(0.0f)
    , mPerpCheating(false) {
}

AIRoadBlock::~AIRoadBlock() {
}

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

void AIRoadBlock::OnAttached(IAttachable *pOther) {
    IVehicle *ivehicle;
    if (pOther->QueryInterface(&ivehicle)) {
        VehicleList.push_back(ivehicle);
    }
}

void AIRoadBlock::OnDetached(IAttachable *pOther) {
    if (UTL::COM::ComparePtr(Pursuit, pOther)) {
        Pursuit = nullptr;
    } else {
        IVehicle *ivehicle;
        if (pOther->QueryInterface(&ivehicle)) {
            IVehicle **it = _STL::find(VehicleList.begin(), VehicleList.end(), ivehicle);
            if (it != VehicleList.end()) {
                VehicleList.erase(it);
            }
        }
    }
}

IVehicle *AIRoadBlock::IsComprisedOf(HSIMABLE obj) {
    if (!VehicleList.empty()) {
        for (IVehicle *const *iter = VehicleList.begin(); iter != VehicleList.end(); ++iter) {
            IVehicle *car = *iter;
            ISimable *isim;
            car->QueryInterface(&isim);
            if (isim->GetOwnerHandle() == obj) {
                return car;
            }
        }
    }
    return nullptr;
}

void AIRoadBlock::AddSmackable(IPlaceableScenery *smackable, bool isSpikeStrip) {
    RoadblockSmackableList.push_back(smackable);
    if (isSpikeStrip) {
        mNumSpikeStrips++;
    }
}
