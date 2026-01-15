
#include "Speed/Indep/Src/EAXSound/SoundConn.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAudible.h"
#include "Speed/Indep/Src/Physics/VehicleBehaviors.h"
#include "Speed/Indep/Src/Sim/SimServer.h"

// total size: 0x64
class SoundHeli : public VehicleBehavior, public IAudible {
    static Behavior *Construct(const BehaviorParams &params);

    SoundHeli(const BehaviorParams &params);
    // void OnService(Pkt_Car_Service &svc);

    // Virtual overrides
    // IUnknown
    ~SoundHeli() override;

    // IAudible
    bool IsAudible() const override {
        return CheckService(mSoundService) == Sim::CONNSTATUS_READY;
    }

    // Behavior
    void Reset() override {}

    void OnBehaviorChange(const struct UCrc32 &mechanic) override;

    void OnTaskSimulate(float dT) override {}

    // Overrides: IServiceable
    bool OnService(HSIMSERVICE hCon, Sim::Packet *pkt) override;

  private:
    HSIMSERVICE mSoundService;       // offset 0x58, size 0x4
    struct IHelicopter *mHelicopter; // offset 0x5C, size 0x4
    IVehicle *mVehicle;              // offset 0x60, size 0x4
};

SoundHeli::SoundHeli(const BehaviorParams &params) : VehicleBehavior(params, 0), IAudible(params.fowner), mSoundService(nullptr) {
    GetOwner()->QueryInterface(&mVehicle);

    SoundConn::Pkt_Heli_Open pkt(GetOwner()->GetAttributes().GetConstCollection(), GetOwner()->GetWorldID());
    mSoundService = OpenService(0xa3f44e2e, &pkt);
}
