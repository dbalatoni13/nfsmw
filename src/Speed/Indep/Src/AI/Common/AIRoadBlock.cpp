#include "Speed/Indep/Src/AI/AIRoadBlock.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pursuitlevels.h"
#include "Speed/Indep/Src/Interfaces/IAttachable.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IActivity.h"
#include "Speed/Indep/Src/Interfaces/SimModels/IPlaceableScenery.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Sim/SimActivity.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

#include <algorithm>
#include <cfloat>

AIRoadBlock::AIRoadBlock(Sim::Param params)
    : Sim::Activity(1),      //
      IRoadBlock(this),      //
      Pursuit(nullptr),      //
      mDodged(false),        //
      mNumCopsDamaged(0),    //
      mNumCopsDestroyed(0),  //
      mNumSpikeStrips(0),    //
      mPerpCheatTime(0.0f),  //
      mPerpCheating(false) {
    mPerpCheatPoint = UMath::Vector3::kZero;
}

AIRoadBlock::~AIRoadBlock() {
    DetachAll();
}

Sim::IActivity *AIRoadBlock::Construct(Sim::Param params) {
    return new AIRoadBlock(params);
}

bool AIRoadBlock::AddVehicle(IVehicle *vehicle) {
    IPursuitAI *ipv;
    bool result = Attach(vehicle);

    if (vehicle->QueryInterface(&ipv)) {
        ipv->StartRoadBlock();
    }
    return result;
}

void AIRoadBlock::AddSmackable(IPlaceableScenery *smackable, bool isSpikeStrip) {
    RoadblockSmackableList.push_back(smackable);

    if (isSpikeStrip) {
        mNumSpikeStrips++;
    }
}

bool AIRoadBlock::RemoveVehicle(IVehicle *vehicle) {
    bool result = Detach(vehicle);

    return result;
}

void AIRoadBlock::ReleaseAllSmackables() {
    for (Smackables::const_iterator iter = RoadblockSmackableList.begin(); iter != RoadblockSmackableList.end(); ++iter) {

        IPlaceableScenery *smckble = *iter;
        smckble->Destroy();
    }
    RoadblockSmackableList.clear();
    mNumSpikeStrips = 0;
}

float AIRoadBlock::GetMinDistanceToTarget(float dT, float &distxz, IVehicle **minDistVehicle) {
    if (!GetPursuit() || !GetPursuit()->GetTarget()) {

        return 200.0f;
    }

    const UMath::Vector3 &targpos = GetPursuit()->GetTarget()->GetPosition();

    Attrib::Gen::pursuitlevels *pursuitLevelAttrib = GetPursuit()->GetPursuitLevelAttrib();
    float engageRadius = 300.0f;

    float min3 = FLT_MAX;
    float minxz = min3;

    for (Vehicles::const_iterator iter = VehicleList.begin(); iter != VehicleList.end(); ++iter) {

        IVehicle *ivehicle = *iter;
        if (ivehicle->IsActive() && !ivehicle->IsDestroyed()) {

            float distancey = bAbs(ivehicle->GetPosition().y - targpos.y);
            float distance3 = UMath::Distance(ivehicle->GetPosition(), targpos);
            float distancexz = UMath::Distancexz(targpos, ivehicle->GetPosition());
            if (distance3 < min3) {

                min3 = distance3;

                if (minDistVehicle) {

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

        mPerpCheating = false;
        if (minxz < engageRadius + 200.0f) {


            float d2perpPoint = UMath::DistanceSquare(targpos, mRoadBlockCentre);
            if (d2perpPoint < 90000.0f) {


                mPerpCheatTime += dT;
                if (mPerpCheatTime > 20.0f || GetPursuit()->GetPursuitStatus() == PS_COOL_DOWN) {

                    mPerpCheating = true;
                }
            } else {

                mPerpCheatPoint = targpos;
                mPerpCheatTime = 0.0f;
            }
        }
    }

    return min3;
}

int AIRoadBlock::GetNumCops() {
    return VehicleList.size();
}

void AIRoadBlock::OnAttached(IAttachable *pOther) {
    IVehicle *ivehicle;

    if (pOther->QueryInterface(&ivehicle)) {
        VehicleList.push_back(ivehicle);
    }
}

void AIRoadBlock::OnDetached(IAttachable *pOther) {
    IVehicle *ivehicle;

    if (ComparePtr(pOther, Pursuit)) {

        Pursuit = nullptr;
    } else if (pOther->QueryInterface(&ivehicle)) {

        VehicleList.erase(std::find(VehicleList.begin(), VehicleList.end(), ivehicle));
    }
}

IVehicle *AIRoadBlock::IsComprisedOf(HSIMABLE obj) {
    if (VehicleList.empty()) {
        return nullptr;
    }

    for (Vehicles::const_iterator iter = VehicleList.begin(); iter != VehicleList.end(); ++iter) {

        IVehicle *car = *iter;
        if (car->GetSimable()->GetOwnerHandle() == obj) {
            return car;
        }
    }

    return nullptr;
}
