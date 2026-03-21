#include "DrawVehicle.h"

#include "Speed/Indep/Src/Sim/SimConn.h"

namespace RenderConn {

class Pkt_Car_Service : public Sim::Packet {};

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
        *reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(&pkt) + 0x80) = mTransmission->GetGear();
        *reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(&pkt) + 0x7C) = mTransmission->GetShiftStatus();
        *reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(&pkt) + 0x74) = mEngine->IsNOSEngaged();

        if (mTransmission->GetGear() < 2 || GetVehicle()->GetDriverClass() != DRIVER_HUMAN) {
            float rpm_ratio = 0.0f;
            float rev = mEngine->GetRPM();
            float amin = mEngine->GetMinRPM();
            float amax = mEngine->GetMaxRPM();
            float arange = amax - amin;
            if (arange > 1e-06f) {
                float result = (rev - amin) / arange;
                rpm_ratio = 1.0f;
                if (result < 1.0f) {
                    rpm_ratio = result;
                }
                if (rpm_ratio < 0.0f) {
                    rpm_ratio = 0.0f;
                }
            }

            float vib = 0.0f;
            {
                float rev = mEngine->GetHorsePower();
                Hp min_power = mEngine->GetMinHorsePower();
                Hp max_power = mEngine->GetMaxHorsePower();
                float arange = max_power - min_power;
                if (arange > 1e-06f) {
                    float result = (rev - min_power) / arange;
                    vib = 1.0f;
                    if (result < 1.0f) {
                        vib = result;
                    }
                    if (vib < 0.0f) {
                        vib = 0.0f;
                    }
                }
            }

            *reinterpret_cast<float *>(reinterpret_cast<char *>(&pkt) + 0x88) = rpm_ratio;
            *reinterpret_cast<float *>(reinterpret_cast<char *>(&pkt) + 0x84) = vib;
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
        *reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(&pkt) + 0x78) = mEngineDamage->IsBlown();
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
