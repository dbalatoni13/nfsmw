#include "Speed/Indep/Src/AI/AIRoadBlock.h"

#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pursuitlevels.h"
#include "Speed/Indep/Src/Interfaces/SimModels/IPlaceableScenery.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
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
    DetachAll();
}

Sim::IActivity *AIRoadBlock::Construct(Sim::Param params) {
    AIRoadBlock *rb = new AIRoadBlock(params);
    Sim::IActivity *result = nullptr;
    if (rb) {
        result = static_cast<Sim::IActivity *>(rb);
    }
    return result;
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

float AIRoadBlock::GetMinDistanceToTarget(float dT, float &distxz, IVehicle **minDistVehicle) {
    IPursuit *pursuit = GetPursuit();
    if (pursuit == nullptr) {
        return -1.0f;
    }
    AITarget *target = pursuit->GetTarget();
    if (target == nullptr) {
        return -1.0f;
    }
    const UMath::Vector3 &targpos = target->GetPosition();
    Attrib::Gen::pursuitlevels *pursuitLevelAttrib = pursuit->GetPursuitLevelAttrib();
    float engageRadius = pursuitLevelAttrib->FullEngagementRadius();
    float min3 = 100000.0f;
    float minxz = engageRadius;

    for (IVehicle *const *iter = VehicleList.begin(); iter != VehicleList.end(); ++iter) {
        IVehicle *ivehicle = *iter;
        if (ivehicle->IsActive() && !ivehicle->IsDestroyed()) {
            UMath::Vector3 &pos = ivehicle->GetPosition();
            float distanceY = bAbs(pos.y - targpos.y);
            float distance3 = UMath::Distance(pos, targpos);
            float distancexz = UMath::Distancexz(pos, targpos);
            if (distance3 < min3) {
                min3 = distance3;
                if (minDistVehicle != nullptr) {
                    *minDistVehicle = ivehicle;
                }
            }
            if (distanceY < 10.0f && distancexz < minxz) {
                minxz = distancexz;
            }
            if (distance3 < engageRadius) {
                IPursuitAI *ipursuitai;
                if (ivehicle->QueryInterface(&ipursuitai)) {
                    ipursuitai->SetWithinEngagementRadius();
                }
            }
        }
    }

    distxz = minxz;
    if (0.0f < dT) {
        mPerpCheating = false;
        if (minxz < engageRadius + -1.0f) {
            float d2perpPoint = UMath::DistanceSquare(targpos, mRoadBlockCentre);
            if (d2perpPoint >= 100.0f) {
                mPerpCheatPoint = targpos;
                mPerpCheatTime = 0.0f;
            } else {
                mPerpCheatTime += dT;
                if (mPerpCheatTime > 5.0f) {
                    if (pursuit->GetPursuitStatus() == PS_COOL_DOWN) {
                        mPerpCheating = true;
                    }
                }
            }
        }
    }

    return min3;
}
