#include "Speed/Indep/Src/EAXSound/SoundConn.h"
#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IAudible.h"
#include "Speed/Indep/Src/Interfaces/Simables/IHelicopter.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/VehicleBehaviors.h"
#include "Speed/Indep/Src/Sim/SimServer.h"

// total size: 0x64
class SoundHeli : public VehicleBehavior, public IAudible {
  public:
    static Behavior *Construct(const BehaviorParams &params);

  protected:
    SoundHeli(const BehaviorParams &params);
    ~SoundHeli() override;

    // IAudible
    bool IsAudible() const override {
        return CheckService(mSoundService) == Sim::CONNSTATUS_READY;
    }

    // Behavior
    void Reset() override {}
    void OnTaskSimulate(float dT) override {}

    // IServiceable
    bool OnService(HSIMSERVICE hCon, Sim::Packet *pkt) override;

    // Behavior
    void OnBehaviorChange(const UCrc32 &mechanic) override;

    void OnService(SoundConn::Pkt_Car_Service &svc);

  private:
    HSIMSERVICE mSoundService; // offset 0x58, size 0x4
    IHelicopter *mHelicopter;  // offset 0x5C, size 0x4
    IVehicle *mVehicle;        // offset 0x60, size 0x4
};

BIND_BEHAVIOR_FACTORY(SoundHeli);

Behavior *SoundHeli::Construct(const BehaviorParams &params) {
    return new SoundHeli(params);
}

SoundHeli::SoundHeli(const BehaviorParams &params) : VehicleBehavior(params, 0), IAudible(params.fowner), mSoundService(nullptr) {
    this->GetOwner()->QueryInterface(&this->mVehicle);

    SoundConn::Pkt_Heli_Open pkt(this->GetOwner()->GetAttributes().GetConstCollection(), GetOwner()->GetWorldID());
    this->mSoundService = this->OpenService(UCRC32_EAXSOUND, &pkt);
}

SoundHeli::~SoundHeli() {
    if (this->mSoundService != nullptr) {
        this->CloseService(this->mSoundService);
    }
}

void SoundHeli::OnService(SoundConn::Pkt_Car_Service &svc) {}

void SoundHeli::OnBehaviorChange(const UCrc32 &mechanic) {
    Behavior::OnBehaviorChange(mechanic);
}

bool SoundHeli::OnService(HSIMSERVICE hCon, Sim::Packet *pkt) {
    if (hCon == this->mSoundService && !IsPaused()) {
        this->OnService(*reinterpret_cast<SoundConn::Pkt_Car_Service *>(pkt));
        return true;
    }
    return false;
}
