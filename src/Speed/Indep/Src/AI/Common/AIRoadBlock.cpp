#include "Speed/Indep/Src/AI/AIRoadBlock.h"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IActivity.h"
#include <cfloat>

static const float kRoadBlockCheatTimeout = 20.0f; // size: 0x4, Decl: 20

AIRoadBlock::AIRoadBlock(Sim::Param params)
    : Activity(1),              //
      IRoadBlock(this),         //
      VehicleList(),            //
      RoadblockSmackableList(), //
      Pursuit(nullptr),         //
      mDodged(false),           //
      mNumCopsDamaged(0),       //
      mNumCopsDestroyed(0),     //
      mNumSpikeStrips(0),       //
      mPerpCheatTime(0.0f),     //
      mPerpCheating(false) {
    this->mPerpCheatPoint = UMath::Vector3::kZero;
}

AIRoadBlock::~AIRoadBlock() {
    this->DetachAll();
}

Sim::IActivity *AIRoadBlock::Construct(Sim::Param params) {
    return new AIRoadBlock(params);
}

bool AIRoadBlock::AddVehicle(IVehicle *vehicle) {
    bool result = this->Attach(vehicle);
    IPursuitAI *ipv;
    if (vehicle->QueryInterface(&ipv)) {
        ipv->StartRoadBlock();
    }
    return result;
}

void AIRoadBlock::AddSmackable(IPlaceableScenery *smackable, bool isSpikeStrip) {
    this->RoadblockSmackableList.push_back(smackable);
    if (isSpikeStrip) {
        this->mNumSpikeStrips++;
    }
}

bool AIRoadBlock::RemoveVehicle(IVehicle *vehicle) {
    bool result = this->Detach(vehicle);
    return result;
}

void AIRoadBlock::ReleaseAllSmackables() {
    for (IRoadBlock::Smackables::const_iterator iter = this->RoadblockSmackableList.begin(); iter != this->RoadblockSmackableList.end(); ++iter) {
        IPlaceableScenery *smckble = *iter;
        smckble->Destroy();
    }
    this->RoadblockSmackableList.clear();
    this->mNumSpikeStrips = 0;
}

float AIRoadBlock::GetMinDistanceToTarget(float dT, float &distxz, IVehicle **minDistVehicle) {
    if (this->GetPursuit() == nullptr) {
        return 200.0f;
    }

    if (this->GetPursuit()->GetTarget() == nullptr) {
        return 200.0f;
    }

    const UMath::Vector3 &targpos = this->GetPursuit()->GetTarget()->GetPosition();
    Attrib::Gen::pursuitlevels *pursuitLevelAttrib = this->GetPursuit()->GetPursuitLevelAttrib();

    float engageRadius = 300.0f;
    float min3 = FLT_MAX;
    float minxz = min3;

    for (IRoadBlock::Vehicles::const_iterator iter = this->VehicleList.begin(); iter != this->VehicleList.end(); ++iter) {
        IVehicle *ivehicle = *iter;

        if (ivehicle->IsActive() && !ivehicle->IsDestroyed()) {
            float distancey = bAbs(ivehicle->GetPosition().y - targpos.y);
            float distance3 = UMath::Distance(ivehicle->GetPosition(), targpos);
            float distancexz = UMath::Distancexz(ivehicle->GetPosition(), targpos);

            if (distance3 < min3) {
                min3 = distance3;
                if (minDistVehicle != nullptr) {
                    *minDistVehicle = ivehicle;
                }
            }

            if (distancey < 1.5f && distancexz < minxz) {
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

    if (dT > 0.0f) {
        this->mPerpCheating = false;

        if (minxz < engageRadius + 200.0f) {
            float d2perpPoint = UMath::DistanceSquare(targpos, this->mRoadBlockCentre);

            if (d2perpPoint < 90000.0f) {
                this->mPerpCheatTime += dT;

                if (this->mPerpCheatTime > 20.0f || this->GetPursuit()->GetPursuitStatus() == PS_COOL_DOWN) {
                    this->mPerpCheating = true;
                }
            } else {
                this->mPerpCheatPoint = targpos;
                this->mPerpCheatTime = 0.0f;
            }
        }
    }

    return min3;
}

int AIRoadBlock::GetNumCops() {
    return this->VehicleList.size();
}

void AIRoadBlock::OnAttached(IAttachable *pOther) {
    IVehicle *ivehicle;
    if (pOther->QueryInterface(&ivehicle)) {
        this->VehicleList.push_back(ivehicle);
    }
}

void AIRoadBlock::OnDetached(IAttachable *pOther) {
    if (UTL::COM::ComparePtr(pOther, this->Pursuit)) {
        this->Pursuit = nullptr;
        return;
    }
    IVehicle *ivehicle;
    if (pOther->QueryInterface(&ivehicle)) {
        this->VehicleList.erase(std::find(this->VehicleList.begin(), this->VehicleList.end(), ivehicle));
    }
}

IVehicle *AIRoadBlock::IsComprisedOf(HSIMABLE obj) {
    if (this->VehicleList.empty()) {
        return nullptr;
    }
    for (IRoadBlock::Vehicles::const_iterator iter = this->VehicleList.begin(); iter != this->VehicleList.end(); ++iter) {
        IVehicle *car = *iter;
        if (car->GetSimable()->GetOwnerHandle() == obj) {
            return car;
        }
    }
    return nullptr;
}
