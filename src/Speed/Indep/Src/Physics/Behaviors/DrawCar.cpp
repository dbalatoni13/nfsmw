#include "DrawVehicle.h"

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
