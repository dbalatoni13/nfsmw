#include "DrawVehicle.h"

#include "Speed/Indep/Src/Sim/SimConn.h"

namespace RenderConn {

class Pkt_Car_Service : public Sim::Packet {
  public:
    float mCompressions[4];   // offset 0x4, size 0x10
    float mWheelSpeed[4];     // offset 0x14, size 0x10
    float mTireSkid[4];       // offset 0x24, size 0x10
    float mTireSlip[4];       // offset 0x34, size 0x10
    float mSteering[2];       // offset 0x44, size 0x8
    int mGroundState;         // offset 0x4C, size 0x4
    unsigned int mDamageInfo; // offset 0x50, size 0x4
    char mPartState[0xC];     // offset 0x54, size 0xC
    unsigned int mLights;     // offset 0x60, size 0x4
    unsigned int mBrokenLights; // offset 0x64, size 0x4
    bool mInView;             // offset 0x68, size 0x1
    char _pad0[3];
    float mDistanceToView;    // offset 0x6C, size 0x4
    bool mFlashing;           // offset 0x70, size 0x1
    char _pad1[3];
    bool mNos;                // offset 0x74, size 0x1
    char _pad2[3];
    bool mEngineBlown;        // offset 0x78, size 0x1
    char _pad3[3];
    unsigned int mShift;      // offset 0x7C, size 0x4
    unsigned int mGear;       // offset 0x80, size 0x4
    float mEnginePower;       // offset 0x84, size 0x4
    float mEngineSpeed;       // offset 0x88, size 0x4
    float mExtraBodyRoll;     // offset 0x8C, size 0x4
    float mExtraBodyPitch;    // offset 0x90, size 0x4
    int mBlowOuts;            // offset 0x94, size 0x4
    float mHealth;            // offset 0x98, size 0x4
    float mAnimatedCarPitch;  // offset 0x9C, size 0x4
    float mAnimatedCarRoll;   // offset 0xA0, size 0x4
    float mAnimatedCarShake;  // offset 0xA4, size 0x4
};

} // namespace RenderConn

Behavior *DrawTraffic::Construct(const BehaviorParams &params) {
    return new DrawTraffic(params);
}

DrawTraffic::DrawTraffic(const BehaviorParams &params)
    : DrawCar(params, CarRenderUsage_AITraffic) {}

DrawPerformanceCar::DrawPerformanceCar(const BehaviorParams &params, CarRenderUsage usage)
    : DrawCar(params, usage) {
    GetOwner()->QueryInterface(&mTransmission);
    GetOwner()->QueryInterface(&mEngine);
}

void DrawPerformanceCar::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_ENGINE) {
        GetOwner()->QueryInterface(&mTransmission);
        GetOwner()->QueryInterface(&mEngine);
    }
    DrawCar::OnBehaviorChange(mechanic);
}

void DrawPerformanceCar::OnService(RenderConn::Pkt_Car_Service &pkt) {
    DrawCar::OnService(pkt);
    if (mEngine && mTransmission) {
        pkt.mGear = mTransmission->GetGear();
        pkt.mShift = mTransmission->GetShiftStatus();
        pkt.mNos = mEngine->IsNOSEngaged();

        if (mTransmission->GetGear() < 2 || GetVehicle()->GetDriverClass() != DRIVER_HUMAN) {
            float engineSpeed = 0.0f;
            float engineSpeedRange = mEngine->GetMaxRPM() - mEngine->GetMinRPM();
            if (engineSpeedRange > 1e-06f) {
                engineSpeed = (mEngine->GetRPM() - mEngine->GetMinRPM()) / engineSpeedRange;
                engineSpeed = UMath::Min(engineSpeed, 1.0f);
                engineSpeed = UMath::Max(engineSpeed, 0.0f);
            }

            float enginePower = 0.0f;
            float enginePowerRange = mEngine->GetMaxHorsePower() - mEngine->GetMinHorsePower();
            if (enginePowerRange > 1e-06f) {
                enginePower = (mEngine->GetHorsePower() - mEngine->GetMinHorsePower()) / enginePowerRange;
                enginePower = UMath::Min(enginePower, 1.0f);
                enginePower = UMath::Max(enginePower, 0.0f);
            }

            pkt.mEngineSpeed = engineSpeed;
            pkt.mEnginePower = enginePower;
        }
    }
}

Behavior *DrawNISCar::Construct(const BehaviorParams &params) {
    return new DrawNISCar(params);
}

DrawNISCar::DrawNISCar(const BehaviorParams &params)
    : DrawPerformanceCar(params, carRenderUsage_NISCar) {}

Behavior *DrawCopCar::Construct(const BehaviorParams &params) {
    return new DrawCopCar(params);
}

DrawCopCar::DrawCopCar(const BehaviorParams &params)
    : DrawPerformanceCar(params, CarRenderUsage_AICop) {}

Behavior *DrawRaceCar::Construct(const BehaviorParams &params) {
    IVehicle *vehicle = nullptr;
    static_cast<ISimable *>(params.fowner)->QueryInterface(&vehicle);
    if (!vehicle) {
        return nullptr;
    }

    DriverClass driverClass = vehicle->GetDriverClass();
    CarRenderUsage usage;
    if (driverClass != DRIVER_RACER) {
        if (driverClass < DRIVER_NONE) {
            if (driverClass == DRIVER_HUMAN) {
                usage = CarRenderUsage_Player;
            } else {
                usage = CarRenderUsage_AIRacer;
            }
        } else if (driverClass == DRIVER_REMOTE) {
            usage = CarRenderUsage_RemotePlayer;
        } else {
            usage = CarRenderUsage_AIRacer;
        }
    } else {
        usage = CarRenderUsage_AIRacer;
    }

    return new DrawRaceCar(params, usage);
}

DrawRaceCar::DrawRaceCar(const BehaviorParams &params, CarRenderUsage usage)
    : DrawPerformanceCar(params, usage) {
    GetOwner()->QueryInterface(&mEngineDamage);
}

void DrawRaceCar::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_ENGINE) {
        GetOwner()->QueryInterface(&mEngineDamage);
    }
    DrawPerformanceCar::OnBehaviorChange(mechanic);
}

void DrawRaceCar::OnService(RenderConn::Pkt_Car_Service &pkt) {
    DrawPerformanceCar::OnService(pkt);
    if (mEngineDamage) {
        pkt.mEngineBlown = mEngineDamage->IsBlown();
    }
}

void DrawCar::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_SUSPENSION) {
        GetOwner()->QueryInterface(&mSuspension);
        GetOwner()->QueryInterface(&mNIS);
    }
    if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        GetOwner()->QueryInterface(&mRBVehicle);
    }
    if (mechanic == BEHAVIOR_MECHANIC_DAMAGE) {
        GetOwner()->QueryInterface(&mDamage);
        GetOwner()->QueryInterface(&mVehicleDamage);
        GetOwner()->QueryInterface(&mSpikeDamage);
    }
}

bool DrawCar::IsHidden() const {
    return mHidden || DrawVehicle::IsHidden() || mRenderService;
}

void DrawCar::HideModel() {
    DrawVehicle::HideModel();
    mHidden = true;
}

void DrawCar::ReleaseModel() {
    DrawVehicle::ReleaseModel();
    if (mRenderService) {
        CloseService(mRenderService);
        mRenderService = 0;
    }
}

void DrawCar::ReleaseChildModels() {
    DrawVehicle::ReleaseChildModels();
    if (!mParts.empty()) {
        mParts.clear();
    }
}

bool DrawCar::IsPartVisible(const UCrc32 &name) const {
    return mParts.find(name) == mParts.end();
}

void DrawCar::HidePart(const UCrc32 &name) {
    Parts::iterator iter = mParts.find(name);
    if (iter == mParts.end()) {
        Parts::iterator lower = mParts.lower_bound(name);
        if (lower == mParts.end() || name < lower->first) {
            lower = mParts.insert(lower, Parts::value_type(name, 0));
        }
        lower->second = 1;
    } else {
        iter->second = iter->second + 1;
    }
}

void DrawCar::ShowPart(const UCrc32 &name) {
    Parts::iterator iter = mParts.find(name);
    if (iter != mParts.end()) {
        iter->second = iter->second - 1;
        if (iter->second < 1) {
            mParts.erase(iter);
        }
    }
}

bool DrawCar::InView() const {
    return mInView;
}

bool DrawCar::IsRenderable() const {
    return CheckService(mRenderService) == Sim::CONNSTATUS_READY;
}

float DrawCar::DistanceToView() const {
    return mDistanceToView;
}

void DrawCar::Reset() {}

void DrawCar::OnTaskSimulate(float dT) {}
